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
    binaryData = true;
}

XmlArchiver::~XmlArchiver() {
}


void XmlArchiver::addTrack( const Track& pTrack ){
    TiXmlElement* mTrack = new TiXmlElement( "track" );
    mRoot->LinkEndChild( mTrack );

    if (pTrack.easyName.length() > 0 )
        mTrack->SetAttribute( "Name", pTrack.easyName);
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
    TiXmlElement* mFeat = new TiXmlElement( "feature" );
    mRoot->LinkEndChild( mFeat );
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
        mValues->InsertEndChild( TiXmlText( std::to_string( feat.getData().at(i) )) );
        if( i < feat.getData().n_elem - 1 )
            mValues->InsertEndChild( TiXmlText( " " ) );
    }


    featureNum++;
}
void XmlArchiver::addFeature( const TimedMat feat, std::string name, std::string trackName ) {

    TiXmlElement* mFeat = new TiXmlElement( "feature" );
    mRoot->LinkEndChild( mFeat );
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
        for( int j = 0; j < feat.getData().n_rows;j++)
            mFrame->InsertEndChild( TiXmlText( std::to_string( feat.getData().at(j,i) ) ) );
    }
    featureNum++;
}
void XmlArchiver::addFeature( const TimedCube feat, std::string name, std::string trackName ) {
    TiXmlElement* mFeat = new TiXmlElement( "feature" );
    mRoot->LinkEndChild( mFeat );
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
        for( int j = 0; j < feat.getData().n_elem; j++ )
            mFrame->InsertEndChild( TiXmlText( std::to_string( feat.getData().at( j ) ) ) );
    }
    featureNum++;
}

void XmlArchiver::addLabels(  LabelList label,std::string name, std::string trackName ) {
    TiXmlElement* mLabelHeader = new TiXmlElement( "LabelCollection" );
    mRoot->LinkEndChild( mLabelHeader );
    mLabelHeader->SetAttribute( "Id", labelNum );
    mLabelHeader->SetAttribute( "name", name );
    if( trackName.length()>0 )
        mLabelHeader->SetAttribute( "linkedTrackName", trackName );
    TiXmlElement* mSegments = new TiXmlElement( "Segments" );
    mLabelHeader->LinkEndChild( mSegments );

    mSegments->SetAttribute( "numSegment", label.size() );
    for( int i = 0; i < label.size(); i++ ) {
        TiXmlElement* mSegment = new TiXmlElement( "Segment" );
        mSegments->LinkEndChild( mSegment );
        mSegment->SetAttribute( "FrameIndex", label[i].moment.index() );//TODO verify if index==frameIndex of the track
        mSegment->SetAttribute( "Time", std::to_string( label[i].moment.time() ) );
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
    mArchiver.LoadFile( archiveFileName );
    mRoot = mArchiver.FirstChildElement( "MoMaArchive" );
    if (mRoot==0)
        throw std::runtime_error( "XmlArchiver::load : This file doesn't contain a MoMa archive" );

    TiXmlElement* mHandle=mRoot->FirstChildElement("track");

   
    for( trackNum = 0; mHandle; mHandle = mHandle->NextSiblingElement("track"), ++trackNum ) {
        //nothing
    }
    std::cout <<trackNum<< std::endl;

}
void XmlArchiver::getTrack( Track& pTrack ,int index) {

}

void XmlArchiver::clear() {
}
