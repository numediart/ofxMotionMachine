/**
*
*  @file mmTrack.cpp
*  @brief MotionMachine source file for Track class.
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmTrack.h"
#include "mmParser.h"

using namespace std;
using namespace arma;
using namespace MoMa;

Track::Track( void ) {

    init();
}

Track::Track( Frame frame ) {

    init();
    push( frame );

    hasNodeList = frame.hasNodeList;
    if( hasNodeList ) nodeList = std::make_shared<NodeList>( *( frame.nodeList ) );

    hasBoneList = frame.hasBoneList;
    if( hasBoneList ) boneList = std::make_shared<BoneList>( *( frame.boneList ) );
}

MoMa::Track::Track(const Track & tr) : Track()
{
    tr.copy(*this);
}

Track::~Track( void ) {

    if( hasNodeList ) //delete 
        nodeList=0; // Deallocation
    if( hasBoneList ) //delete 
        boneList=0; // Deallocation
    // if( hasSynoList ) delete synoList; // Deallocation
}

void Track::init( void ) {

    easyName = ""; // No name
    fileName = ""; // No file name

    // synoList = NULL; // Not allocated
    // hasSynoList = false; // No synonym list

    hasRotation = false; // No rotation

    nodeList = NULL; // Not allocated
    hasNodeList = false; // No node name list

    boneList = NULL; // Not allocated
    hasBoneList = false; // No node name list
    hasGlobalCoordinate = true;
    //FIXME this should not be in constructor, among other things, it causes unnecessary
    //      load of the SynoList when creating a subtrack.
    this->synolist( SynoList::DefaultPath );

    ringSize = 0; // Init ring buffer size
    isRing = false; // Not ring buffer

  //  hasOrigNodeRot_as_boneRot=true;

    setFrameRate( 177.0f ); // Qualisys
}

bool Track::synolist( string fileName ) {

    /*
    if( !hasSynoList ) {
        this->synoList = new SynoList( );
        hasSynoList = true;
    }

    return this->synoList->load(fileName);
    */

    return false;
}

void Track::nodes( string fileName ) {

   // if( hasNodeList == true ) delete nodeList;

    nodeList = std::make_shared<NodeList>( fileName );

    if( nOfNodes() == 0 || nOfNodes() == nodeList->size() ) {

        hasNodeList = true;

    }
    else {

        cout << "Track: node number does not match frame size" << endl;

        hasNodeList = false;
        nodeList = 0;
       // delete nodeList;
    }
}

void Track::bones( string fileName ) {

    if( hasBoneList == true ) //delete 
        boneList=0;

    ifstream bonFile(fileName.c_str());

    if (!bonFile.is_open()) { //Temporary, find a better way... such as a load function instead of the constructor so we can return a boolean telling if the bone file could be loaded.

          hasBoneList = false;
    }
    else {

        bonFile.close();
        boneList = std::make_shared<BoneList>(fileName);
        hasBoneList = true;
    }
}

void Track::load( string const &fileName ) {

    //if( hasNodeList ) delete nodeList; // Deallocation
    //nodeList = NULL;
    //if( hasBoneList ) delete boneList; // Deallocation
    //boneList = NULL;

    //init();
    Parser parser( fileName, this );
}

void Track::setRingBufferSize( int size, bool pHasRotation, bool pTimed ) {
    if( pTimed )
        position.setRealTimeMode( size, 3u, nodeList->size() );
    else
        position.setRealTimeMode( size, _frameRate, 3u, nodeList->size() );

    if( pHasRotation ) {
        if( pTimed )
            rotation.setRealTimeMode( size, 4u, boneList->size() );
        else
            rotation.setRealTimeMode( size, _frameRate, 4u, boneList->size() );

        hasRotation = true;
        //this->hasOrigNodeRot_as_boneRot=false;
        this->initJointOffsetRotation();
    };
    ringSize = size;
    isRing = true;
}

void Track::push( Frame _frame ) {

    if( _frame.hasRotation() && ( ( !nOfFrames() ) || hasRotation ) ) {

        hasRotation = true;

        if( rotation.isTimed() && _frame.hasTime() ) {

            rotation.push( _frame.getRotation(), _frame.time() );

        }
        else {

            rotation.push( _frame.getRotation() );
        }

        if( rotationOffset.n_elem == 0 ) {

            rotationOffset = _frame.getRotationOffset();
        }
    }

    if( position.isTimed() && _frame.hasTime() ) {

        position.push( _frame.getPosition(), _frame.time() );

    }
    else {

        arma::mat temp = _frame.getPosition();
        position.push( temp );
    }

    if( isRing ) {

        if( position.nOfFrames() > ringSize ) {}// position.pop(); // Ring buffer behavior
        if( _frame.hasRotation() && ( rotation.nOfFrames() > ringSize ) ) {}// rotation.pop();
    }
}

bool Track::localToGlobal() {

    if( hasNodeList == false || hasBoneList == false || hasGlobalCoordinate==true )
        return false;
    for( int i = 0; i < boneList->rootIt.size(); i++)
        localToGlobal( boneList->rootIt[i] );
    hasGlobalCoordinate = true;
	return this->setJointOffsetRotation();
}

void Track::localToGlobal( boneMapType::iterator it ){
    int indMax = max( it->second.jointChildren.size(), it->second.boneChildrenIt.size() );
    for( int bEnd = 0; bEnd < it->second.jointChildren.size(); bEnd++ ) {


        for( int idFrame = 0; idFrame < nOfFrames(); idFrame++ ) {
            MoMa::quaternion lQuat( rotation.getRefData().slice( idFrame ).col( it->second.boneId ) );
            if( arma::norm( ( arma::colvec ) lQuat ) < arma::datum::eps )
                continue;
            arma::mat lMat, lMat2;
            lQuat.get( lMat );
            //lQuat.clear();
            lQuat.set( lMat );
            lMat.submat( 0, 3, 2, 3 ) = position.getData().slice( idFrame ).col( it->second.jointParent );//last column is the translation column

            arma::vec lVec = lMat.submat( 0, 3, 2, 3 );
            lMat = lMat.submat( 0, 0, 2, 2 );
            position.getRefData().slice( idFrame ).col( it->second.jointChildren[bEnd] ) = lMat*position.getRefData().slice( idFrame ).col( it->second.jointChildren[bEnd] ) + lVec;

        }

    }
    for( int bEnd = 0; bEnd < it->second.boneChildrenIt.size(); bEnd++ ) {


        for( int idFrame = 0; idFrame < nOfFrames(); idFrame++ ) {
            MoMa::quaternion lQuat( rotation.getRefData().slice( idFrame ).col( it->second.boneId ) );
            if( arma::norm( ( arma::colvec ) lQuat ) < arma::datum::eps ) {
                throw std::runtime_error( " Track::globalToLocal, no Valid orientation for a non endpoint node" );
            }
            arma::mat lMat, lMat2;
            lQuat.get( lMat );
            //lQuat.clear();
            lQuat.set( lMat );
            lMat.submat( 0, 3, 2, 3 ) = position.getData().slice( idFrame ).col( it->second.jointParent );//last column is the translation column
            
            MoMa::quaternion lQuat2( rotation.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.boneId ) );
                
            lQuat2.get( lMat2 );
            if( arma::norm( ( arma::colvec ) lQuat2 ) > arma::datum::eps ) {
                lQuat.set( ( arma::mat )( lMat*lMat2 ) );
                rotation.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.boneId ) = lQuat;
            }
            else {
                lMat = lMat.submat( 0, 0, 2, 2 );
                arma::vec lVec2 = position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointParent );
                for( int b2 = 0; b2 < it->second.boneChildrenIt[bEnd]->second.jointChildren.size(); b2++ ) {
                    arma::vec lVec3 = lMat*position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointChildren[b2] );
                    position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointChildren[b2] ) = lVec3 + lVec2;// = lMat*position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointChildren[bEnd] ) + lVec;
                }
            }
        }

        localToGlobal( it->second.boneChildrenIt[bEnd] );


    }
}

bool Track::globalToLocal() {
    if( hasRotation==false || hasNodeList == false || hasBoneList == false || hasGlobalCoordinate==false )
        return false;
    for( int i = 0; i < boneList->rootIt.size(); i++ )
        globalToLocal( boneList->rootIt [i]);
    hasGlobalCoordinate = false;
    this->setJointOffsetRotation();

    return true;

};
void Track::globalToLocal( boneMapType::iterator it ){
    int indMax=max( it->second.jointChildren.size(), it->second.boneChildrenIt.size());
   
    for( int bEnd = 0; bEnd <  it->second.boneChildrenIt.size(); bEnd++ ) {
        globalToLocal( it->second.boneChildrenIt[bEnd] );
        for( int idFrame = 0; idFrame < nOfFrames(); idFrame++ ) {
            MoMa::quaternion lQuat( rotation.getRefData().slice( idFrame ).col( it->second.boneId ) );
            if( arma::norm( ( arma::colvec ) lQuat ) < arma::datum::eps ) {
                throw std::runtime_error( " Track::globalToLocal, no Valid orientation for a non endpoint node" );
            }
            arma::mat lMat,lMat2;
            lQuat.get( lMat );
            lMat.submat( 0, 3, 2, 3 ) = position.getData().slice( idFrame ).col( it->second.jointParent );
            lMat=inv(lMat);

            MoMa::quaternion lQuat2( rotation.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.boneId ) );

            if( arma::norm( ( arma::colvec ) lQuat2 ) > arma::datum::eps ) {
                lQuat2.get( lMat2 );
                lQuat.set( ( arma::mat )( lMat*lMat2 ) );
                rotation.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.boneId ) = lQuat;
            }
            else {
                arma::vec lVec = lMat.submat( 0, 3, 2, 3 );
                lMat = lMat.submat( 0, 0, 2, 2 );
                arma::vec lVec2 = lMat*position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointParent );
                for( int b2 = 0; b2 < it->second.boneChildrenIt[bEnd]->second.jointChildren.size(); b2++ ) {
                    arma::vec lVec3 = lMat*position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointChildren[b2] );
                    position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointChildren[b2] ) = lVec3 - lVec2;// = lMat*position.getRefData().slice( idFrame ).col( it->second.boneChildrenIt[bEnd]->second.jointChildren[bEnd] ) + lVec;
                }
            }
        }

    }
    for( int bEnd = 0; bEnd < it->second.jointChildren.size(); bEnd++ ) {

        for( int idFrame = 0; idFrame < nOfFrames(); idFrame++ ) {
            MoMa::quaternion lQuat( rotation.getRefData().slice( idFrame ).col( it->second.boneId ) );
            if( arma::norm( ( arma::colvec ) lQuat ) < arma::datum::eps )
                continue;
            arma::mat lMat, lMat2;
            lQuat.get( lMat );
            arma::vec lVec3= position.getData().slice( idFrame ).col( it->second.jointParent );
            lMat.submat( 0, 3, 2, 3 ) = position.getData().slice( idFrame ).col( it->second.jointParent );
            lMat = inv( lMat );
            arma::vec lVec = lMat.submat( 0, 3, 2, 3 );
            lMat = lMat.submat( 0, 0, 2, 2 );
            position.getRefData().slice( idFrame ).col( it->second.jointChildren[bEnd] ) = lMat*position.getRefData().slice( idFrame ).col( it->second.jointChildren[bEnd] ) + lVec;
            
        }

    }
}


void Track::setName( string name ) {

    easyName = name;
}

void Track::setFileName( string name ) {

    fileName = name;

    // If the track doesn't have an easy name at the
    // time we load a file on it ( should only happens
    // in drag-and-drop scenarios ), we give the track
    // the file name as its default easy name at time.

    if( easyName == "" || easyName == "Unnamed" ) {

        easyName = name;
    }
}

int Track::index( std::string name ) {

    // TODO There is a better way

    int nIdx = -1;

    if( hasNodeList ) {

        nIdx = nodeList->index( name );
    }

    /*
    if( nOfFrames() > 0 && hasNodeList ) {

        Frame oneFrame = frame( (unsigned int)0 );
        nIdx = oneFrame.index( name );
    }
    */

    return( nIdx );
}

void Track::setFrameRate( float rate ) {

    _frameRate = rate;

    position.setFrameRate( rate );

    if( hasRotation ) {

        rotation.setFrameRate( rate );
    }
}

void Track::clear( void ) {

    if( hasNodeList ) //delete 
        nodeList=0; // Deallocation
    //nodeList = NULL;
    if( hasBoneList ) //delete 
        boneList=0; // Deallocation
    //boneList = NULL;
    rotationOffset.clear();
    rotation.clear();
    position.clear();
    hasRotation = false;
}

void Track::clearData( void ) {
    rotation.clear();
    position.clear();
    hasRotation = false;
}

void Track::cut( int beg, int end ) {

    position = position.sub( beg, end );

    if( hasRotation ) {

        rotation = rotation.sub( beg, end );
    }

    /*if (hasRotation) {

        if (rotation.isTimed()) {

            // TODO
            rotation.setData(rotation.getTimeVec().subvec(beg, end), rotation.getData().slices(beg, end));
        }
        else {


            //rotation.getRefData().shed_slices(end+1, rotation.nOfFrames());
            //rotation.getRefData().shed_slices(0,beg-1);
            //rotation.setData(rotation.frameRate(), rotation.getData().slices(beg, end), (double)(beg / _frameRate));
        }
    }

    if (position.isTimed()) {

        // TODO

        position.setData(position.getTimeVec().subvec(beg, end), position.getData().slices(beg, end));
        position.setInitialTime((double)(beg / _frameRate));

    }
    else {

        position.setData(position.frameRate(), position.getData().slices(beg, end));
        position.setInitialTime((double)(beg / _frameRate));
    }*/
}

void Track::copy( Track &tr ) const {

    if( tr.nodeList ) 
        //delete tr.nodeList;
        tr.nodeList=0;
    if( tr.boneList ) //delete 
        tr.boneList=0;

    tr = *this;
    //create independant nodelist and bonelist (to avoid conflicts when modifying/deleting tracks)
    if( hasNodeList ) {

        tr.nodeList = std::make_shared<NodeList>();
        *( tr.nodeList ) = *nodeList;
    }
    if( hasBoneList ) {

        tr.boneList = std::make_shared<BoneList>();
        *( tr.boneList ) = *boneList;
		tr.boneList->updateBoneChildrenName();
    }
}

void Track::subTrack( Track &subTr, int beg, int end ) {

    subTr.hasNodeList = hasNodeList;

    if( hasNodeList ) {

        subTr.nodeList = std::make_shared<NodeList>();
        *( subTr.nodeList ) = *nodeList;
    }

    subTr.hasBoneList = hasBoneList;

    if( hasBoneList ) {

        subTr.boneList = std::make_shared<BoneList>();
        *( subTr.boneList ) = *boneList;
    }

    // subTr.hasSynoList = hasSynoList;

    /*
    if( hasSynoList ) {

        subTr.synoList = new SynoList();
        *(subTr.synoList) = *synoList;
    }
    */

    subTr.easyName = easyName;
    subTr.fileName = fileName;
    subTr._frameRate = _frameRate;
    subTr.ringSize = ringSize;
    subTr.isRing = isRing;

    subTr.hasRotation = hasRotation;

    if( hasRotation ) {

        if( rotation.isTimed() ) {

            // TODO

            subTr.rotation.setData( rotation.getTimeVec().subvec( beg, end ), rotation.getData().slices( beg, end ) );
            subTr.rotationOffset = rotationOffset;
            subTr.rotation.setInitialTime( (double)( beg / _frameRate ) );

        }
        else {

            subTr.rotation.setData( rotation.frameRate(), rotation.getData().slices( beg, end ) );
            subTr.rotationOffset = rotationOffset;
            subTr.rotation.setInitialTime( (double)( beg / _frameRate ) );
        }
    }

    if( position.isTimed() ) {

        // TODO

        subTr.position.setData( position.getTimeVec().subvec( beg, end ), position.getData().slices( beg, end ) );
        subTr.position.setInitialTime( (double)( beg / _frameRate ) );
    }
    else {

        subTr.position.setData( position.frameRate(), position.getData().slices( beg, end ) );
        subTr.position.setInitialTime( (double)( beg / _frameRate ) );
    }
}

bool Track::setJointOffsetRotation() {

    if( this->hasBoneList == false || this->hasNodeList == false || !this->hasRotation )
        return false;

    if( this->rotation.getData().size() == 0 )
        throw std::runtime_error( "Track::setJointOffsetRotation : empty rotation matrix with a rotation true flag" );
    bool debug = false;

    this->rotationOffset.resize( 4, this->nOfNodes() );//for each mocap format, the orientation offset is stored in the destination node of the bone
    Frame frame0 = this->frame( (unsigned int)0 );
    if( this->hasGlobalCoordinate == false ) {
        for( boneMapType::iterator it = this->boneList->begin(); it != this->boneList->end(); it++ ) {
            for( int j = 0; j < it->second.jointChildren.size(); j++ ) {//loop on the ids of the destination joints of the current bone.
                int dest = it->second.jointChildren[j];

                arma::colvec xVec, yVec, zVec;
                xVec = frame0.getPosition().col( dest );
                yVec << 0.0 << 1.0 << 0.0;
                zVec << -1.0 << 0.0 << 0.0;
                yVec = arma::cross( zVec, xVec );
                zVec = arma::cross( xVec, yVec );
                arma::mat offsetMatrix( 3, 3 );
                offsetMatrix.col( 0 ) = arma::normalise( xVec );
                offsetMatrix.col( 1 ) = arma::normalise( yVec );
                offsetMatrix.col( 2 ) = arma::normalise( zVec );

                //				quaternion origQuat(frame0.node(orig).rotation);
                quaternion offsetQuat;
                offsetQuat.set( offsetMatrix );
                quaternion lquat( offsetQuat );
                this->rotationOffset.col( dest ) = lquat;

            }
        }
        return true;
    }
	if (frame0.getPosition().n_elem == 0)
		return false;
    arma::colvec frontalAxis;
    arma::colvec longAxis;
    arma::colvec tempVec;

    tempVec << frame0.node( "LShoulder" ).position[0] << frame0.node( "LShoulder" ).position[1] << frame0.node( "LShoulder" ).position[2];
    frontalAxis << frame0.node( "RShoulder" ).position[0] << frame0.node( "RShoulder" ).position[1] << frame0.node( "RShoulder" ).position[2];
    frontalAxis = normalise( tempVec - frontalAxis );

    tempVec.clear();
    tempVec << frame0.node( "Head" ).position[0] << frame0.node( "Head" ).position[1] << frame0.node( "Head" ).position[2];
    longAxis << frame0.node( "Pelvis" ).position[0] << frame0.node( "Pelvis" ).position[1] << frame0.node( "Pelvis" ).position[2];

    longAxis = normalise( tempVec - longAxis );
    arma::colvec sagAxis;
    sagAxis = arma::cross( frontalAxis, longAxis );
    if( debug ) std::cout << "frontal" << std::endl;
    if( debug ) std::cout << frontalAxis << std::endl;
    if( debug ) std::cout << "long" << std::endl;
    if( debug ) std::cout << longAxis << std::endl;
    if( debug ) std::cout << "sag" << std::endl;
    if( debug ) std::cout << sagAxis << std::endl;

    //        for (int i=0;i<this->boneList->size();i++){

    for( boneMapType::iterator it = this->boneList->begin(); it != this->boneList->end(); it++ ) {
        int i = it->second.boneId;
        int orig = it->second.jointParent;//id of the origin joint of the current bone.
        for( int j = 0; j <it->second.jointChildren.size(); j++ ) {//loop on the ids of the destination joints of the current bone.
            int dest = it->second.jointChildren[j];
            if( debug ) {
                std::cout << orig << " " << dest << std::endl;
            }
            std::vector<float> val;
            arma::colvec tempVecX, tempVecY, tempVecZ;
            tempVecX = frame0.getPosition().col( dest ) - frame0.getPosition().col( orig );//<<  frame0.node(dest).position[0] - frame0.node(orig).position[0] << frame0.node(dest).position[1] - frame0.node(orig).position[1] << frame0.node(dest).position[2] - frame0.node(orig).position[2];
            tempVecX = arma::normalise( tempVecX );
            if( debug ) {
                std::cout << tempVecX << std::endl;
            }
            if( std::abs( arma::dot( tempVecX, sagAxis ) )>std::abs( arma::dot( tempVecX, longAxis ) ) && std::abs( arma::dot( tempVecX, sagAxis ) ) > std::abs( arma::dot( tempVecX, frontalAxis ) ) ) {
                tempVecZ = arma::cross( tempVecX, frontalAxis );
                tempVecY = arma::cross( tempVecZ, tempVecX );
            }
            else if( std::abs( arma::dot( tempVecX, frontalAxis ) ) > std::abs( arma::dot( tempVecX, longAxis ) ) ) {

                tempVecY = arma::cross( longAxis, tempVecX );
                tempVecZ = arma::cross( tempVecX, tempVecY );
            }
            else if( arma::dot( tempVecX, longAxis ) > 0 ) {
                tempVecZ = arma::cross( frontalAxis, tempVecX );
                tempVecY = arma::cross( tempVecZ, tempVecX );
            }
            else {
                tempVecZ = arma::cross( tempVecX, frontalAxis );
                tempVecY = arma::cross( tempVecZ, tempVecX );
            }

            arma::mat offsetMatrix;
            offsetMatrix.eye( 3, 3 );
            offsetMatrix.col( 0 ) = arma::normalise( tempVecX );
            offsetMatrix.col( 1 ) = arma::normalise( tempVecY );
            offsetMatrix.col( 2 ) = arma::normalise( tempVecZ );
            if( debug ) std::cout << offsetMatrix << std::endl;

            //				quaternion origQuat(frame0.node(orig).rotation);
            quaternion origQuat( frame0.getRotation().col( i ) );
            quaternion offsetQuat;
            offsetQuat.set( offsetMatrix );
            quaternion lquat( origQuat.inverse()*offsetQuat );

            if( debug ) std::cout << lquat( 0 ) << " " << lquat( 1 ) << " " << lquat( 2 ) << " " << lquat( 3 ) << std::endl;

            this->rotationOffset.col( dest ) = lquat;
        }
    }


    /*    else{//Kinect

            this->rotationOffset.zeros(4, nodeList->size());

            int i = 0;
            for (boneMapType::iterator it = this->boneList->begin(); it != this->boneList->end(); it++) {
                int orig,dest;
                MoMa::quaternion lquat,lquat2;

                orig=it->second.jointParent;
                for (int j = 0; j < it->second.jointChildren.size(); j++) {
                    dest = it->second.jointChildren[j];
                    lquat << 0 << -0.7 << 0 << 0.7;
                    this->rotationOffset.col(dest) = lquat;
                }

            }
        }*/

    return true;
}

bool Track::initJointOffsetRotation() {

	if (this->hasBoneList == false || this->hasNodeList == false || !this->hasRotation)
		return false;

	if (this->rotation.getData().size() == 0)
		throw std::runtime_error("Track::setJointOffsetRotation : empty rotation matrix with a rotation true flag");
	bool debug = false;

	this->rotationOffset.resize(4, this->nOfNodes());//for each mocap format, the orientation offset is stored in the destination node of the bone
	this->rotationOffset.zeros();
	this->rotationOffset.row(3).ones();

	return true;
}