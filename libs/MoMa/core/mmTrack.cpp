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

Track::Track( Frame frame )  {

    init();
    push( frame );

    hasNodeList = frame.hasNodeList;
    if(hasNodeList) nodeList = new NodeList(*(frame.nodeList));
     
    hasBoneList = frame.hasBoneList;
    if(hasBoneList) boneList = new BoneList(*(frame.boneList));
}

Track::~Track( void ) {

    if( hasNodeList ) delete nodeList; // Deallocation
    if( hasBoneList ) delete boneList; // Deallocation
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

    //FIXME this should not be in constructor, among other things, it causes unnecessary
    //      load of the SynoList when creating a subtrack.
    this->synolist(SynoList::DefaultPath);
    
    ringSize = 0; // Init ring buffer size
    isRing = false; // Not ring buffer
    
    hasOrigNodeRot_as_boneRot=true;
    
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

    if( hasNodeList == true ) delete nodeList;

    nodeList = new NodeList( fileName );

    if( nOfNodes() == 0 || nOfNodes() == nodeList->size() ) {

        hasNodeList = true;

    } else {

        cout << "Track: node number does not match frame size" << endl;

        hasNodeList = false;
        delete nodeList;
    }
}

void Track::bones( string fileName ) {

    if( hasBoneList == true ) delete boneList;

    boneList = new BoneList( fileName );
    hasBoneList = true;
}

void Track::load( string const &fileName ) {

    Parser parser( fileName, this );
}

void Track::setRingBufferSize( int size ,bool pHasRotation,bool pTimed) {
	if (pTimed)
		position.setRealTimeMode(size,3u,nodeList->size());
	else
		position.setRealTimeMode(size,_frameRate,3u,nodeList->size());

	if (pHasRotation){
		if (pTimed)
			rotation.setRealTimeMode(size,4u,nodeList->size());
		else
			rotation.setRealTimeMode(size,_frameRate,4u,nodeList->size());

		hasRotation=true;
		this->hasOrigNodeRot_as_boneRot=false;
		this->setJointOffsetRotation();
	};
	ringSize = size;
    isRing = true;
}

void Track::push( Frame _frame ) {
    
    if( _frame.hasRotation() && ( ( !nOfFrames() ) || hasRotation ) ) {
        
        hasRotation = true;
        
        if( rotation.isTimed() && _frame.hasTime() ) {
            
            rotation.push( _frame.getRotation(), _frame.time() );
            
        } else {
            
            rotation.push( _frame.getRotation() );
        }
        
        if( rotationOffset.n_elem == 0 ) {
            
            rotationOffset = _frame.getRotationOffset();
        }
    }
    
    if( position.isTimed() && _frame.hasTime() ) {
        
        position.push( _frame.getPosition(), _frame.time() );
        
    } else {
        
        arma::mat temp = _frame.getPosition();
        position.push( temp );
    }
    
    if( isRing ) {
        
		if( position.nOfFrames() > ringSize ){}// position.pop(); // Ring buffer behavior
		if( _frame.hasRotation() && (rotation.nOfFrames() > ringSize )){}// rotation.pop();
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
    
    rotationOffset.clear();
    rotation.clear();
    position.clear();
    hasRotation = false;
}

void Track::cut(int beg, int end) {

    position = position.sub(beg, end);

    if (hasRotation) {

        rotation = rotation.sub(beg, end);
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

void Track::copy(Track &tr) {

    if (tr.nodeList) delete tr.nodeList;
    if (tr.boneList) delete tr.boneList;

    tr = *this;
    //create independant nodelist and bonelist (to avoid conflicts when modifying/deleting tracks)
    if (hasNodeList) {

        tr.nodeList = new NodeList();
        *(tr.nodeList) = *nodeList;
    }
    if (hasBoneList) {

        tr.boneList = new BoneList();
        *(tr.boneList) = *boneList;
    }
}

void Track::subTrack( Track &subTr, int beg, int end) {
        
    subTr.hasNodeList = hasNodeList;
    
    if( hasNodeList ) {

        subTr.nodeList = new NodeList();
        *(subTr.nodeList) = *nodeList;
    }

    subTr.hasBoneList = hasBoneList;
    
    if( hasBoneList ) {
        
        subTr.boneList = new BoneList();
        *(subTr.boneList) = *boneList;
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
            subTr.rotation.setInitialTime((double)(beg / _frameRate));
            
        } else {
            
            subTr.rotation.setData( rotation.frameRate(), rotation.getData().slices( beg, end ) );
            subTr.rotationOffset = rotationOffset;
            subTr.rotation.setInitialTime((double)(beg / _frameRate));
        }
    }
    
    if( position.isTimed() ) {
        
        // TODO
        
        subTr.position.setData( position.getTimeVec().subvec( beg, end ),position.getData().slices( beg, end ) );
        subTr.position.setInitialTime((double)(beg / _frameRate));
    } else {
        
        subTr.position.setData( position.frameRate(), position.getData().slices( beg, end ) );
        subTr.position.setInitialTime((double)(beg / _frameRate));
    }
}

bool Track::setJointOffsetRotation() {
    
    if (this->hasBoneList==false||this->hasNodeList==false)
        return false;
    boneList->hasOrigNodeRot_as_boneRot=this->hasOrigNodeRot_as_boneRot;
    if (hasOrigNodeRot_as_boneRot){
        if (this->rotation.getData().size()==0)
            return false;
        this->rotationOffset.resize(4,this->rotation.nOfCols());
        bool debug = false;
        if (!this->boneList || !this->hasNodeList || !this->hasRotation /*|| !this->hasSynoList*/ )
            return false;
        
        this->rotationOffset.resize(4,this->nOfNodes());
        Frame frame0 = this->frame((unsigned int)0);
        arma::colvec frontalAxis;
        arma::colvec tempVec;
        
        tempVec<<frame0.node("LShoulder").position[0] <<frame0.node("LShoulder").position[1]<<frame0.node("LShoulder").position[2];
        frontalAxis<<frame0.node("RShoulder").position[0] <<frame0.node("RShoulder").position[1]<<frame0.node("RShoulder").position[2];
        frontalAxis=normalise(tempVec-frontalAxis);
        
        
        arma::colvec longAxis;
        
        tempVec.clear();
        
        tempVec<<frame0.node("Head").position[0] <<frame0.node("Head").position[1]<<frame0.node("Head").position[2];
        longAxis<<frame0.node("Pelvis").position[0] <<frame0.node("Pelvis").position[1]<<frame0.node("Pelvis").position[2];
        
        longAxis=normalise(tempVec-longAxis);
        arma::colvec sagAxis;
        sagAxis=arma::cross(frontalAxis,longAxis);
        if( debug ) std::cout<<"frontal"<<std::endl;
        if( debug ) std::cout<<frontalAxis<<std::endl;
        
        if( debug ) std::cout<<"long"<<std::endl;
        if( debug ) std::cout<<longAxis<<std::endl;
        if( debug ) std::cout<<"sag"<<std::endl;
        if( debug ) std::cout<<sagAxis  <<std::endl;
        
        for (int i=0;i<this->boneList->size();i++){
            int orig=this->boneList->at(i).first;
            int dest=this->boneList->at(i).second;
            if( debug ){
                std::cout<<orig<<" "<<dest<<std::endl;
            }
            std::vector<float> val;
            arma::mat offsetMatrix;
            offsetMatrix.eye(3,3);
            arma::colvec tempVecX,tempVecY,tempVecZ;
            tempVecX<<frame0.node(dest).position[0]-frame0.node(orig).position[0]<<frame0.node(dest).position[1]-frame0.node(orig).position[1]<<frame0.node(dest).position[2]-frame0.node(orig).position[2];
            tempVecX=arma::normalise(tempVecX);
            if( debug ){
                std::cout<<tempVecX<<std::endl;
            }
            if (std::abs(arma::dot(tempVecX,sagAxis))>std::abs(arma::dot(tempVecX,longAxis))&&std::abs(arma::dot(tempVecX,sagAxis))>std::abs(arma::dot(tempVecX,frontalAxis))){
                
                tempVecZ=arma::cross(tempVecX,frontalAxis);
                tempVecY=arma::cross(tempVecZ,tempVecX);
                
            }
            else if (std::abs(arma::dot(tempVecX,frontalAxis))>std::abs(arma::dot(tempVecX,longAxis))){
                
                tempVecY=arma::cross(longAxis,tempVecX);
                tempVecZ=arma::cross(tempVecX,tempVecY);
                
            }
            else if (arma::dot(tempVecX,longAxis)>0){
                
                tempVecZ=arma::cross(frontalAxis,tempVecX);
                tempVecY=arma::cross(tempVecZ,tempVecX);
            }
            else {
                tempVecZ=arma::cross(tempVecX,frontalAxis);
                tempVecY=arma::cross(tempVecZ,tempVecX);
            }
            
            offsetMatrix.col(0)=arma::normalise( tempVecX);
            offsetMatrix.col(1)=arma::normalise( tempVecY);
            offsetMatrix.col(2)=arma::normalise( tempVecZ);
            if( debug ) std::cout<<offsetMatrix<<std::endl;
            
            quaternion origQuat(frame0.node(orig).rotation);
            quaternion offsetQuat;
            offsetQuat.set(offsetMatrix);
            quaternion lquat(origQuat.inverse()*offsetQuat);
            
            if( debug ) std::cout<<lquat(0)<<" "<<lquat(1)<<" "<<lquat(2)<<" "<<lquat(3)<<std::endl;
            
            this->rotationOffset.col(dest)=lquat;
        }
    }
    else{
        
        this->rotationOffset.zeros(4, nodeList->size());
        for (int i=0;i<boneList->size();i++){
            int orig,dest;
            MoMa::quaternion lquat,lquat2;
            
            orig=this->boneList->at(i).first;
            dest=this->boneList->at(i).second;
            lquat<<0<<-0.7<<0<<0.7;
            this->rotationOffset.col(dest)=lquat;
            
        }
    }
    
    return true;
}
