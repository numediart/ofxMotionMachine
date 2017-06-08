#include "mmXmlArchiver.h"

using namespace MoMa;


XmlArchiver::XmlArchiver() {
    mArchiver.LinkEndChild( new TiXmlDeclaration( "1.0", "UTF-8", "" ) );
    mRoot = new TiXmlElement( "MoMaArchive" );
    mArchiver.LinkEndChild( mRoot );
    trackNum = 0;
    labelNum = 0;
    featureNum = 0; 
    rootPositionOnlyFlag = false;
    binaryData = false;
}

XmlArchiver::~XmlArchiver() {
}



void XmlArchiver::addTrack( const Track& pTrack ){

    TiXmlElement* mTracks = mRoot->FirstChildElement( "tracks" );
    if( mTracks == 0 ) {
        mTracks = new TiXmlElement( "tracks" );
        mRoot->LinkEndChild( mTracks );
    }
    TiXmlElement* mTrack = new TiXmlElement( "track" );
    mTracks->LinkEndChild( mTrack );

    if (pTrack.easyName.length() > 0 )
        mTrack->SetAttribute( "name", pTrack.easyName);
    if (pTrack.fileName.length()>0)
        mTrack->SetAttribute(  "fileName", pTrack.fileName );
    mTrack->SetAttribute( "Id", trackNum );

    TiXmlElement* mProperties = new TiXmlElement( "properties" );
    mTrack->LinkEndChild( mProperties );
    mProperties->SetAttribute( "BonesOrientation", pTrack.hasRotation );
    mProperties->SetAttribute( "encoding", pTrack.hasGlobalCoordinate ? "Global" : "Local" );
    mProperties->SetAttribute( "RootPositionOnly", rootPositionOnlyFlag );
    mProperties->SetAttribute( "binaryData", binaryData );
    
    TiXmlElement* mSkeleton = new TiXmlElement( "skeleton" );
    mTrack->LinkEndChild( mSkeleton );

    if( pTrack.hasNodeList ) {

        TiXmlElement* mNodes = new TiXmlElement( "nodes" );
        mSkeleton->LinkEndChild( mNodes );
        for( int i = 0; i < pTrack.nodeList->size(); i++ ) {
            TiXmlElement* mNode = new TiXmlElement( "node" );
            mNodes->LinkEndChild( mNode );
            
            mNode->SetAttribute( "id", i);
            mNode->SetAttribute( "label", pTrack.nodeList->name( i ) );
        }

        if( pTrack.hasBoneList ) {
            TiXmlElement* mBones = new TiXmlElement( "bones" );
            mSkeleton->LinkEndChild( mBones );
            std::vector<TiXmlElement*> mBoneContainer;
            for( int i = 0; i < pTrack.boneList->size(); i++ ) {
                TiXmlElement* mBone = new TiXmlElement( "bone" );
                mBones->LinkEndChild( mBone );
                mBone->SetAttribute( "id", i);
                mBoneContainer.push_back( mBone );
            }
            for( boneMapType::iterator it = pTrack.boneList->begin(); it != pTrack.boneList->end(); it++ ) {
                mBoneContainer[it->second.boneId]->SetAttribute( "label", it->first );
                TiXmlElement* mNodeParent = new TiXmlElement( "nodeParent" );
                mBoneContainer[it->second.boneId]->LinkEndChild( mNodeParent );
                    
                TiXmlElement* mNodeChildrens = new TiXmlElement( "nodeChildrens" );
                mBoneContainer[it->second.boneId]->LinkEndChild( mNodeChildrens );
                {
                    TiXmlText Value( std::to_string( it->second.jointParent ) );
                    mNodeParent->InsertEndChild( Value );
                }
                for( int j = 0; j < it->second.jointChildren.size(); j++ ) {

                    TiXmlText Value(std::to_string( it->second.jointChildren[j] ));
                    mNodeChildrens->InsertEndChild( Value );
                    if ( j < it->second.jointChildren.size()-1 )
                        mNodeChildrens->InsertEndChild( TiXmlText(" ") );
                }
            }
        }
    }
    TiXmlElement* mFrames = new TiXmlElement( "frames" );
    mTrack->LinkEndChild( mFrames );
    mFrames->SetAttribute( "TimeStamped", pTrack.position.isTimed() );
    if( pTrack.position.isTimed() ) {
        mFrames->SetAttribute( "MinTime", pTrack.minTime() );
        mFrames->SetAttribute( "MaxTime", pTrack.minTime() );
    }
    else
        mFrames->SetAttribute( "FrameRate", pTrack.frameRate() );
    for( int i = 0; i < pTrack.nOfFrames(); i++ ) {
        TiXmlElement* mFrame = new TiXmlElement( "frame" );
        mFrames->LinkEndChild( mFrame );
        mFrame->SetAttribute( "id", i );
        if ( pTrack.position.isTimed() )
            mFrame->SetAttribute( "time", pTrack.position.time(i));
        TiXmlElement* mPosition = new TiXmlElement( "position" );
        mFrame->LinkEndChild( mPosition );
        if ( rootPositionOnlyFlag==false )
            for( int j = 0; j < pTrack.position.getData().slice( i ).n_elem; j++ ) {
                mPosition->InsertEndChild( TiXmlText(std::to_string( pTrack.position.getData().slice( i ).at( j ) )) );
                if( j <  pTrack.position.getData().slice( i ).n_elem-1 )
                    mPosition->InsertEndChild( TiXmlText( " " ) );
            }
        else
            for( int j = 0; j < pTrack.position.getData().slice( i ).col( 0 ).n_elem; j++ ) {
                mPosition->InsertEndChild( TiXmlText( std::to_string( pTrack.position.getData().slice( i ).at( j ) ) ) );
                if( j < pTrack.position.getData().slice( i ).col( 0 ).n_elem - 1 )
                    mPosition->InsertEndChild( TiXmlText( " " ) );

            }
        if( pTrack.hasRotation ) {
            TiXmlElement* mOrientation = new TiXmlElement( "orientation" );
            mFrame->LinkEndChild( mOrientation );
            for( int j = 0; j < pTrack.rotation.getData().slice( i ).n_elem; j++ ) {
               mOrientation->InsertEndChild( TiXmlText( std::to_string( pTrack.rotation.getData().slice( i ).at( j ) ) ) );

               if( j < pTrack.rotation.getData().slice( i ).n_elem - 1 )
                    mOrientation->InsertEndChild( TiXmlText( " " ) );
            }
        }
    }
    trackNum++;
}
void XmlArchiver::addFeature( const TimedVec feat, std::string name, std::string trackName ) {
    if( name == std::string( "" ) )
        throw std::runtime_error( "XmlArchiver::addFeature this feature must have a name to be archived" );

    TiXmlElement* mFeatRoot = mRoot->FirstChildElement( "features" );
    if( mFeatRoot == 0 ) {
        mFeatRoot = new TiXmlElement( "features" );
        mRoot->LinkEndChild( mFeatRoot );
    }
    TiXmlElement* mFeat = new TiXmlElement( "feature" );
    mFeatRoot->LinkEndChild( mFeat );
    mFeat->SetAttribute( "Id", featureNum );
    mFeat->SetAttribute( "name", name );
    if (trackName.length()>0 )
        mFeat->SetAttribute( "linkedTrackName", trackName );
    mFeat->SetAttribute( "numDim", 1 );
    mFeat->SetAttribute( "numElem", feat.getData().n_elem );

    TiXmlElement* mFrames = new TiXmlElement( "frames" );
    mFeat->LinkEndChild( mFrames );
    mFrames->SetAttribute( "numFrames", feat.getData().n_elem );
    mFrames->SetAttribute( "TimeStamped", feat.isTimed() );
    if( feat.isTimed() ) {
        mFrames->SetAttribute( "MinTime", feat.minTime() );
        mFrames->SetAttribute( "MaxTime", feat.minTime() );
    }
    else
        mFrames->SetAttribute( "FrameRate", feat.frameRate() );
    if( feat.isTimed() ) {
        TiXmlElement* mTimestamp = new TiXmlElement( "timestamps" );
        mTimestamp->LinkEndChild( mTimestamp );
        for( int i = 0; i < feat.nOfFrames(); i++ ) {
            mTimestamp->InsertEndChild( TiXmlText( std::to_string( feat.time( i ) ) ) );
        }

    }
    TiXmlElement* mValues = new TiXmlElement( "values" );
    mFrames->LinkEndChild( mValues );
    for( int i = 0; i < feat.nOfFrames(); i++ ) {
        if( feat.getData().at( i ) == arma::datum::nan )
            mValues->InsertEndChild( TiXmlText( "nan" ) );
        else
        mValues->InsertEndChild( TiXmlText( std::to_string( feat.getData().at(i) )) );
        if( i < feat.getData().n_elem - 1 )
            mValues->InsertEndChild( TiXmlText( " " ) );
    }


    featureNum++;
}
void XmlArchiver::addFeature( const TimedMat feat, std::string name, std::string trackName ) {

    if( name == std::string( "" ) )
        throw std::runtime_error( "XmlArchiver::addFeature this feature must have a name to be archived" );

    TiXmlElement* mFeatRoot = mRoot->FirstChildElement( "features" );
    if( mFeatRoot == 0 ) {
        mFeatRoot = new TiXmlElement( "features" );
        mRoot->LinkEndChild( mFeatRoot );
    }
    TiXmlElement* mFeat = new TiXmlElement( "feature" );
    mFeatRoot->LinkEndChild( mFeat );
    mFeat->SetAttribute( "Id", featureNum );
    mFeat->SetAttribute( "name", name );
    if( trackName.length()>0 )
        mFeat->SetAttribute( "linkedTrackName", trackName );
    mFeat->SetAttribute( "numDim", 2 );

    TiXmlElement* mFrames = new TiXmlElement( "frames" );
    mFeat->LinkEndChild( mFrames );
    mFrames->SetAttribute( "numFrames", feat.getData().n_cols );
    mFrames->SetAttribute( "numRows", feat.getData().n_rows );
    mFrames->SetAttribute( "TimeStamped", feat.isTimed() );
    if( feat.isTimed() ) {
        mFrames->SetAttribute( "MinTime", feat.minTime() );
        mFrames->SetAttribute( "MaxTime", feat.minTime() );
    }
    else
        mFrames->SetAttribute( "FrameRate", feat.frameRate() );
    for( int i = 0; i < feat.nOfFrames(); i++ ) {
        TiXmlElement* mFrame = new TiXmlElement( "frame" );
        mFrames->LinkEndChild( mFrame );
        mFrame->SetAttribute( "id", i );
        if( feat.isTimed() )
            mFrame->SetAttribute( "time", feat.time( i ) );
        for( int j = 0; j < feat.getData().n_rows; j++ ) {
            if ( feat.getData().at( j, i ) ==arma::datum::nan)
                mFrame->InsertEndChild( TiXmlText("nan" ) );
            else
                mFrame->InsertEndChild( TiXmlText( std::to_string( feat.getData().at( j, i ) ) ) );
            if ( j < feat.getData().n_rows-1 )  
                mFrame->InsertEndChild( TiXmlText( " ") );
        }
    }
    featureNum++;
}
void XmlArchiver::addFeature( const TimedCube feat, std::string name, std::string trackName ) {
    if( name == std::string( "" ) )
        throw std::runtime_error( "XmlArchiver::addFeature this feature must have a name to be archived" );
       
    TiXmlElement* mFeatRoot= mRoot->FirstChildElement( "features" );
    if( mFeatRoot == 0 ) {
        mFeatRoot = new TiXmlElement( "features" );
        mRoot->LinkEndChild( mFeatRoot );
    }
    TiXmlElement* mFeat = new TiXmlElement( "feature" );
    mFeatRoot->LinkEndChild( mFeat );
    mFeat->SetAttribute( "Id", featureNum );
    mFeat->SetAttribute( "name", name );
    if( trackName.length()>0 )
        mFeat->SetAttribute( "linkedTrackName", trackName );
    mFeat->SetAttribute( "numDim", 2 );

    TiXmlElement* mFrames = new TiXmlElement( "frames" );
    mFeat->LinkEndChild( mFrames );
    mFrames->SetAttribute( "numFrames", feat.getData().n_slices );
    mFrames->SetAttribute( "numCols", feat.getData().n_cols );
    mFrames->SetAttribute( "numRows", feat.getData().n_rows );
    mFrames->SetAttribute( "TimeStamped", feat.isTimed() );
    if( feat.isTimed() ) {
        mFrames->SetAttribute( "MinTime", feat.minTime() );
        mFrames->SetAttribute( "MaxTime", feat.minTime() );
    }
    else
        mFrames->SetAttribute( "FrameRate", feat.frameRate() );
    for( int i = 0; i < feat.nOfFrames(); i++ ) {
        TiXmlElement* mFrame = new TiXmlElement( "frame" );
        mFrames->LinkEndChild( mFrame );
        mFrame->SetAttribute( "id", i );
        if( feat.isTimed() )
            mFrame->SetAttribute( "time", feat.time( i ) );
        for( int j = 0; j < feat.getData().n_elem; j++ ) {
            if( feat.getData().at( j ) == arma::datum::nan )
                mFrame->InsertEndChild( TiXmlText( "nan" ) );
            else
                mFrame->InsertEndChild( TiXmlText( std::to_string( feat.getData().at( j ) ) ) );
            if( j < feat.getData().n_rows - 1 )
                mFrame->InsertEndChild( TiXmlText( " " ) );
        }
    }
    featureNum++;
}

void XmlArchiver::addLabels(  LabelList label,std::string name, std::string trackName ) {
    TiXmlElement* mLabelRoot = mRoot->FirstChildElement( "labelCollections" );
    if( mLabelRoot == 0 ) {
        mLabelRoot = new TiXmlElement( "labelCollections" );
        mRoot->LinkEndChild( mLabelRoot );
    }
    TiXmlElement* mLabelHeader = new TiXmlElement( "labelCollection" );
    mLabelRoot->LinkEndChild( mLabelHeader );
    mLabelHeader->SetAttribute( "Id", labelNum );
    mLabelHeader->SetAttribute( "name", name );
    if( trackName.length()>0 )
        mLabelHeader->SetAttribute( "linkedTrackName", trackName );
    TiXmlElement* mSegments = new TiXmlElement( "segments" );
    mLabelHeader->LinkEndChild( mSegments );

    mSegments->SetAttribute( "numSegment", label.size() );
    for( int i = 0; i < label.size(); i++ ) {
        TiXmlElement* mSegment = new TiXmlElement( "segment" );
        mSegments->LinkEndChild( mSegment );
        mSegment->SetAttribute( "frameIndex", label[i].moment.index() );//TODO verify if index==frameIndex of the track
        mSegment->SetAttribute( "time", std::to_string( label[i].moment.time() ) );
        mSegment->SetAttribute( "frameRate", std::to_string( label[i].moment.frameRate() ) );
        mSegment->SetAttribute( "name", label[i].name);
        mSegment->SetAttribute( "state", label[i].state );
        
    }
    labelNum++;
}

void XmlArchiver::save( std::string archiveFileName ) {
    
    if( trackNum )
        mArchiver.SaveFile( archiveFileName );
    else
        throw std::runtime_error( "XmlArchiver::save : no added track to archive" );
}

void XmlArchiver::load( std::string archiveFileName ) {
    mArchiver.Clear();
    if( !mArchiver.LoadFile( archiveFileName ) )
        throw std::runtime_error( "XmlArchiver::load failed to open this file" );
    mRoot = mArchiver.FirstChildElement( "MoMaArchive" );
    if (mRoot==0)
        throw std::runtime_error( "XmlArchiver::load : This file doesn't contain a MoMa archive" );

    TiXmlElement* mHandle = mRoot->FirstChildElement( "tracks" );
    mHandle = mHandle->FirstChildElement( "track" );


    for( trackNum = 0; mHandle; mHandle = mHandle->NextSiblingElement( "track" ), ++trackNum ) {

        //nothing
    }
    std::cout << trackNum << std::endl;
}
void XmlArchiver::getTrack( Track& pTrack ,int index) {
    if (index>trackNum )
        throw std::runtime_error( "XmlArchiver::load : track index is not in this archive" );

    TiXmlElement* mTracks = mRoot->FirstChildElement( "tracks" );
    TiXmlElement* mTrackRoot = mTracks->FirstChildElement( "track" );
    int id=-1;
    if( mTrackRoot ==0)
        throw std::runtime_error( "XmlArchiver::load : no track is in this archive" );
        
    for( id = std::stoi( mTrackRoot->Attribute("Id")); mTrackRoot, id != index; mTrackRoot->NextSiblingElement( "track" ) ) {
    }
    if( mTrackRoot ) {
        pTrack.clear();
        TiXmlElement* mProperties = mTrackRoot->FirstChildElement( "properties" );
        pTrack.hasRotation = mProperties->Attribute( "BonesOrientation" ) == std::string("1");
        pTrack.hasGlobalCoordinate = mProperties->Attribute( "encoding" ) == std::string("Global");
        rootPositionOnlyFlag = mProperties->Attribute( "RootPositionOnly" ) == std::string( "1" );

        TiXmlElement* mSkeleton = mTrackRoot->FirstChildElement( "skeleton" );
        pTrack.nodeList = this->loadNodeList( mSkeleton->FirstChildElement( "nodes" ) );
        if( pTrack.nodeList )
            pTrack.hasNodeList = true;
        pTrack.boneList = this->loadBoneList( mSkeleton->FirstChildElement( "bones" ) );
        if( pTrack.boneList )
            pTrack.hasBoneList = true;

        loadData( mTrackRoot->FirstChildElement( "frames" ), pTrack );
        
    }

}
std::shared_ptr<NodeList> XmlArchiver::loadNodeList( TiXmlElement * nodeRoot) {
    if( nodeRoot == 0 )
        return 0;
    std::shared_ptr<NodeList> ret = std::make_shared<NodeList>();
    for ( TiXmlElement *mHandle = nodeRoot->FirstChildElement( "node" ); mHandle; mHandle = mHandle->NextSiblingElement( "node" )) {
       
        ret->emplace( mHandle->Attribute( "label" ),atoi( mHandle->Attribute( "id" )));
        //nothing
    }
    return ret;
}

std::shared_ptr<BoneList> XmlArchiver::loadBoneList( TiXmlElement * boneRoot ) {
    if( boneRoot == 0 )
        return 0;
    std::shared_ptr<BoneList>  ret = std::make_shared<BoneList>();
    for( TiXmlElement *mHandle = boneRoot->FirstChildElement( "bone" ); mHandle; mHandle = mHandle->NextSiblingElement( "bone" ) ) {
        BoneData lBone;
        TiXmlElement *mLoc = mHandle->FirstChildElement( "nodeParent" );
        int nodeParent;

        std::string parentString= mLoc->FirstChild()->ToText()->Value();
        lBone.jointParent = std::stoi( parentString ); 
        
        mLoc = mHandle->FirstChildElement( "nodeChildrens" );
        std::string childrenString = mLoc->FirstChild()->ToText()->Value();

        std::string::size_type sz=0;   // alias of size_t
        while( sz<childrenString.length()) {
            lBone.jointChildren.push_back( std::stoi( childrenString ) );
            sz = childrenString.find_first_of( " " );
            childrenString = childrenString.substr( sz + 1 );
        }
        lBone.boneId = atoi( mHandle->Attribute( "id" ));
        ret->emplace( mHandle->Attribute( "label" ),  lBone) ;
    }
    ret->updateBoneChildrenName();
    return ret;
}

void XmlArchiver::loadData( TiXmlElement * frameRoot, MoMa::Track &pTrack) {
    int numFrames = 0;
    int numNodes = pTrack.nodeList->size();
    int numBones = pTrack.boneList->size();
    
    bool isTimedStamped = std::stoi( frameRoot->Attribute( "TimeStamped" ) );
    int frameRate = 0;
    if ( isTimedStamped == false)
        frameRate=std::stoi( frameRoot->Attribute( "FrameRate" ) );
    pTrack.setFrameRate(frameRate);
    TiXmlElement* mFrame = frameRoot->FirstChildElement( "frame" );
    for( numFrames = 0; mFrame; mFrame = mFrame->NextSiblingElement( "frame" ), ++numFrames ) {

        //nothing
    }
    arma::cube positionCube( 3, numNodes, numFrames );

    arma::cube rotationCube;
    arma::vec timeStamp;
    if( pTrack.hasRotation )
        rotationCube.resize( 4, numBones, numFrames );
    if( isTimedStamped )
        timeStamp.resize( numFrames );
    for( mFrame = frameRoot->FirstChildElement( "frame" ); mFrame; mFrame = mFrame->NextSiblingElement( "frame" )){
        int frameId = std::stoi( mFrame->Attribute( "id" ) );
        if ( isTimedStamped )
            timeStamp( frameId )= std::stod( mFrame->Attribute( "time" ) );
        std::string frameString = mFrame->FirstChild("position")->FirstChild()->ToText()->Value();

        std::string::size_type sz=0;   // alias of size_t
        int j = 0;
        while( sz<frameString.length() ) {
            for( int i = 0; i < 3; i++ ) {
                positionCube( i, j, frameId ) = std::stod( frameString);
                sz = frameString.find_first_of( " " );
                frameString=frameString.substr( sz+1 );
            }
            sz = frameString.find_first_of( " " );
            j++;
        }
        if( pTrack.hasRotation ) {
            frameString = mFrame->FirstChild( "orientation" )->FirstChild()->ToText()->Value();
            std::string::size_type sz = 0;   // alias of size_t
            int j = 0;
            while( sz < frameString.length() ) {
                for( int i = 0; i < 4; i++ ) {
                    rotationCube( i, j, frameId ) = std::stod( frameString );
                    sz = frameString.find_first_of( " " );
                    frameString = frameString.substr( sz + 1 );
                }
                sz = frameString.find_first_of( " " );
                j++;
            }
        }

    }
    if( isTimedStamped == false ){
        pTrack.position.swapData( frameRate, positionCube );
        if (pTrack.hasRotation )
            pTrack.rotation.swapData( frameRate, rotationCube );
    }
    else {
        pTrack.position.swapData( timeStamp, positionCube );
        if( pTrack.hasRotation )
            pTrack.rotation.swapData( timeStamp, rotationCube );
    }
    pTrack.setJointOffsetRotation();
}

bool XmlArchiver::getFeature( std::string featureName, MoMa::TimedCube &feat ) {

    std::string name;
    return this->getFeature( featureName, feat, name );
}

bool XmlArchiver::getFeature( std::string featureName, MoMa::TimedCube &feat, std::string &trackName ) {

    TiXmlElement* featRoot = mRoot->FirstChildElement( "features" );
    if( featRoot==0 )
        throw std::runtime_error( "XmlArchiver::getFeature no feature in this archive" );

    TiXmlElement* searchedFeat=0;
    for(TiXmlElement* xmlFeat = featRoot->FirstChildElement( "feature" ); xmlFeat; xmlFeat = xmlFeat->NextSiblingElement( "feature" ) ) {
        if( xmlFeat->Attribute( "name" ) == featureName ) {
            searchedFeat = xmlFeat;
            break;
        }
    }
    if( searchedFeat==0 || searchedFeat->Attribute("numDim")!=std::string("3") )//check cube dimension
        throw std::runtime_error( "XmlArchiver::getFeature no such feature in this archive" );
    const char *tempName = searchedFeat->Attribute( "trackName" );
    if( tempName )
        trackName = std::string( tempName );
    TiXmlElement* frames = searchedFeat->FirstChildElement( "frames" );
    int numFrames = std::stoi( frames->Attribute( "numFrames" ) );
    int numCols = std::stoi( frames->Attribute( "numCols" ) );
    int numRows = std::stoi( frames->Attribute( "numRows" ) );

    bool isTimeStamped = std::stoi( frames->Attribute( "TimeStamped" ) );
    arma::vec timeStamped;
    double frameRate;
    if( isTimeStamped )
        timeStamped.resize( numFrames );
    else
        frameRate = std::stod( frames->Attribute( "FrameRate" ) );
    arma::cube featData( numRows , numCols , numFrames );
    
    TiXmlElement* frame = frames->FirstChildElement( "frame" );
    for( frame = frames->FirstChildElement( "frame" ); frame; frame = frame->NextSiblingElement( "frame" ) ) {
        int frameId = std::stoi( frame->Attribute( "id" ) );
        if( isTimeStamped )
            timeStamped( frameId ) = std::stod( frame->Attribute( "time" ) );
        std::string frameString = frame->FirstChild()->ToText()->Value();

        std::string::size_type sz = 0;   // alias of size_t
        int i = 0;
        while( sz < frameString.length() && i<(numRows*numCols) ) {
            sz = frameString.find_first_of( " " );
            if( frameString.substr( 0, sz ) == std::string( "nan" ) )
                featData.slice( frameId ).at( i ) = arma::datum::nan;
            else
            featData.slice( frameId ).at( i ) = std::stod( frameString );
            frameString = frameString.substr( sz + 1 );
            i++;
        }
    }
    if( isTimeStamped )
        feat.setData( timeStamped, featData );
    else
        feat.setData( frameRate, featData );

    return true;
}

bool XmlArchiver::getFeature( std::string featureName, MoMa::TimedMat &feat ) {

    std::string name;
    return this->getFeature( featureName, feat, name );
}

bool XmlArchiver::getFeature( std::string featureName, MoMa::TimedMat &feat, std::string &trackName ) {

    TiXmlElement* featRoot = mRoot->FirstChildElement( "features" );
    if( featRoot == 0 )
        throw std::runtime_error( "XmlArchiver::getFeature no feature in this archive" );

    TiXmlElement* searchedFeat = 0;
    for( TiXmlElement* xmlFeat = featRoot->FirstChildElement( "feature" ); xmlFeat; xmlFeat = xmlFeat->NextSiblingElement( "feature" ) ) {
        if( xmlFeat->Attribute( "name" ) == featureName ) {
            searchedFeat = xmlFeat;
            break;
        }
    }
    if( searchedFeat == 0 || searchedFeat->Attribute( "numDim" ) != std::string( "2" ) )//check cube dimension
        throw std::runtime_error( "XmlArchiver::getFeature no such feature in this archive" );
    
    const char *tempName = searchedFeat->Attribute( "trackName" );
    if( tempName )
        trackName = std::string( tempName );
    TiXmlElement* frames = searchedFeat->FirstChildElement( "frames" );
    int numFrames = std::stoi( frames->Attribute( "numFrames" ) );
    int numRows = std::stoi( frames->Attribute( "numRows" ) );

    bool isTimeStamped = std::stoi( frames->Attribute( "TimeStamped" ) );
    arma::vec timeStamped;
    double frameRate;
    if( isTimeStamped )
        timeStamped.resize( numFrames );
    else
        frameRate = std::stod( frames->Attribute( "FrameRate" ) );
    arma::mat featData( numRows, numFrames );

    TiXmlElement* frame = frames->FirstChildElement( "frame" );
    for( frame = frames->FirstChildElement( "frame" ); frame; frame = frame->NextSiblingElement( "frame" ) ) {
        int frameId = std::stoi( frame->Attribute( "id" ) );
        if( isTimeStamped )
            timeStamped( frameId ) = std::stod( frame->Attribute( "time" ) );
        std::string frameString = frame->FirstChild()->ToText()->Value();

        std::string::size_type sz = 0;   // alias of size_t
        int i = 0;
        while( sz < frameString.length() && i<( numRows ) ) {
            sz = frameString.find_first_of( " " );
            if ( frameString.substr( 0, sz ) == std::string( "nan" ) )
                featData( i, frameId ) = arma::datum::nan;
            else
                featData( i, frameId )= std::stod( frameString );
            frameString = frameString.substr( sz + 1 );
            i++;
        }
    }
    if( isTimeStamped )
        feat.setData( timeStamped, featData );
    else
        feat.setData( frameRate, featData );

    return true;
}

bool XmlArchiver::getFeature( std::string featureName, MoMa::TimedVec &feat ) {

    std::string name;
    return this->getFeature( featureName, feat, name );
}

bool XmlArchiver::getFeature( std::string featureName, MoMa::TimedVec &feat, std::string &trackName ) {

    TiXmlElement* featRoot = mRoot->FirstChildElement( "features" );
    if( featRoot == 0 )
        throw std::runtime_error( "XmlArchiver::getFeature no feature in this archive" );

    TiXmlElement* searchedFeat = 0;
    for( TiXmlElement* xmlFeat = featRoot->FirstChildElement( "feature" ); xmlFeat; xmlFeat = xmlFeat->NextSiblingElement( "feature" ) ) {
        if( xmlFeat->Attribute( "name" ) == featureName ) {
            searchedFeat = xmlFeat;
            break;
        }
    }
    if( searchedFeat == 0 || searchedFeat->Attribute( "numDim" ) != std::string( "1" ) )//check cube dimension
        throw std::runtime_error( "XmlArchiver::getFeature no such feature in this archive" );

    const char *tempName = searchedFeat->Attribute( "trackName" );
    if( tempName )
        trackName = std::string( tempName );
    TiXmlElement* frames = searchedFeat->FirstChildElement( "frames" );
    int numFrames = std::stoi( frames->Attribute( "numFrames" ) );

    bool isTimeStamped = std::stoi( frames->Attribute( "TimeStamped" ) );
    arma::vec timeStamped;
    double frameRate;
    if( isTimeStamped )
        timeStamped.resize( numFrames );
    else
        frameRate = std::stod( frames->Attribute( "FrameRate" ) );
    arma::vec featData( numFrames );

    std::string::size_type sz = 0;   // alias of size_t
    int frameId = 0;
    if( isTimeStamped ) {
        std::string timeString = frames->FirstChild( "timestamps" )->FirstChild()->ToText()->Value();
        while( sz < timeString.length() && frameId < ( numFrames ) ) {
            timeStamped( frameId ) = std::stod( timeString );
            sz = timeString.find_first_of( " " );
            timeString= timeString.substr( sz + 1 );
            frameId++;
        }
    }

    sz = 0;   // alias of size_t
    frameId = 0;
    std::string frameString = frames->FirstChild( "values" )->FirstChild()->ToText()->Value();
    while( sz < frameString.length() && frameId < ( numFrames ) ) {
        sz = frameString.find_first_of( " " );
        if( frameString.substr( 0, sz ) == std::string( "nan" ) )
            featData( frameId ) = arma::datum::nan;
        else
            featData( frameId ) = std::stod( frameString );
        frameString = frameString.substr( sz + 1 );
        frameId++;
    }

    if( isTimeStamped )
        feat.setData( timeStamped, featData );
    else
        feat.setData( frameRate, featData );

    return true;
}

bool XmlArchiver::getLabel( std::string labelName,LabelList &pLabelList ) {

    std::string name;
    return this->getLabel( labelName, pLabelList, name);
}

bool XmlArchiver::getLabel( std::string labelName,LabelList &pLabelList, std::string &trackName  ) {

    TiXmlElement* labelRoot = mRoot->FirstChildElement( "labelCollections" );
    if( labelRoot == 0 )
        throw std::runtime_error( "XmlArchiver::getLabel no label collection in this archive" );

    TiXmlElement* searchedLabel = 0;
    for( TiXmlElement* xmlLabel = labelRoot->FirstChildElement( "labelCollection" ); xmlLabel; xmlLabel = xmlLabel->NextSiblingElement( "LabelCollection" ) ) {
        if( xmlLabel->Attribute( "name" ) == labelName ) {
            searchedLabel = xmlLabel;
            break;
        }
    }
    if( searchedLabel == 0 )//check cube dimension
        throw std::runtime_error( "XmlArchiver::getLabel no such label collection in this archive" );

    const char *tempName = searchedLabel->Attribute( "trackName" );
    if( tempName )
        trackName = std::string( tempName );
    TiXmlElement* segments = searchedLabel->FirstChildElement( "segments" );
    int numSegment = std::stoi( segments->Attribute( "numSegment" ) );

    pLabelList.clear();
    for( TiXmlElement* segment = segments->FirstChildElement( "segment" ); segment; segment = segment->NextSiblingElement( "segment" ) ) {
        MoMa::Moment mo;
        mo.setIndex( std::stoi( segment->Attribute( "frameIndex" ) ) );
        mo.setFrameRate( std::stod( segment->Attribute( "frameRate" ) ) );
        mo.setTime( std::stod( segment->Attribute( "time" ) ));
        MoMa::Label lLabel(mo, segment->Attribute( "name" ) );

        lLabel.state = std::stoi( segment->Attribute( "state" ) );
        pLabelList.push_back( lLabel );

    
    }

    return true;
}


void XmlArchiver::contentPrint() {
    std::cout << "tracks"<<":"<< std::endl;
    TiXmlElement* mTemp = mRoot->FirstChildElement( "tracks" );
    for( TiXmlElement *mTrack = mTemp->FirstChildElement("track"); mTrack; mTrack = mTrack->NextSiblingElement( "track" ) ) {
        std::cout << mTrack->Attribute( "Id" ) <<"\t"<< mTrack->Attribute( "name" ) << std::endl;
    }
    std::cout <<"features" << ":" << std::endl;
    mTemp = mRoot->FirstChildElement( "features" );
    for( TiXmlElement *mFeature = mTemp->FirstChildElement("feature"); mFeature; mFeature = mFeature->NextSiblingElement( "feature" ) ) {
        std::cout << mFeature->Attribute( "Id" ) << "\t" << mFeature->Attribute( "name" ) << std::endl;
    }
    std::cout <<"labelCollections" << ":" << std::endl;
    mTemp = mRoot->FirstChildElement( "labelCollections" );
    for( TiXmlElement *mLabelCollection = mTemp->FirstChildElement("labelCollection"); mLabelCollection; mLabelCollection = mLabelCollection->NextSiblingElement( "labelCollection") ) {
        std::cout << mLabelCollection->Attribute( "Id" ) << "\t" << mLabelCollection->Attribute( "name" ) << std::endl;
    }
}

std::vector<std::string> XmlArchiver::getTrackNames() {
    std::vector<std::string> ret;
    TiXmlElement* mTemp = mRoot->FirstChildElement( "tracks" );
    for( TiXmlElement *mTrack = mTemp->FirstChildElement( "track" ); mTrack; mTrack = mTrack->NextSiblingElement( "track" ) ) {
        
        ret.push_back( mTrack->Attribute( "name" ));
    }
    return ret;
}

std::vector< std::pair<std::string,  int > > XmlArchiver::getFeatureNames() {
    std::vector<std::pair<std::string,  int> > ret;
    TiXmlElement* mTemp = mRoot->FirstChildElement( "features" );
    for( TiXmlElement *mFeature = mTemp->FirstChildElement( "feature" ); mFeature; mFeature = mFeature->NextSiblingElement( "feature" ) ) { 
        ret.push_back(std::pair< std::string,int> (mFeature->Attribute( "name" ),std::stoi( mFeature->Attribute( "numDim" ) )));
    }
    return ret;
}
std::vector<std::string> XmlArchiver::getLabelNames() {
    std::vector<std::string> ret;
    TiXmlElement* mTemp = mRoot->FirstChildElement( "labelCollections" );
    for( TiXmlElement *mLabelCollection = mTemp->FirstChildElement( "labelCollection" ); mLabelCollection; mLabelCollection = mLabelCollection->NextSiblingElement( "labelCollection" ) ) {
        ret.push_back( mLabelCollection->Attribute( "name" ) );
    }
    return ret;
}
void XmlArchiver::clear() {
    mArchiver.Clear();
    mRoot = 0;
    trackNum=0;
    featureNum=0;
    labelNum=0;
}

