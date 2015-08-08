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
    
    easyName = ""; // No name
    fileName = ""; // No file name
    
    synoList = NULL; // Not allocated
    hasSynoList = false; // No synonym list

    frameRate = 177.0f; // Qualisys
    hasRotation = false; // No rotation

    nodeList = NULL; // Not allocated
    hasNodeList = false; // No node name list

    boneList = NULL; // Not allocated
    hasBoneList = false; // No node name list

#ifdef _WIN32
    synoList = new SynoList( "../../../../libs/MoMa/resources/SynoList.txt" );
#else
    synoList = new SynoList( "../../../../../../../libs/MoMa/resources/SynoList.txt" );
#endif

    if( synoList != NULL ) hasSynoList = true;

    ringSize = 0; // Init ring buffer size
    isRing = false; // Not ring buffer
}

Track::~Track( void ) {

    if( hasNodeList ) delete nodeList; // Deallocation
    if( hasBoneList ) delete boneList; // Deallocation
    if( hasSynoList ) delete synoList; // Deallocation
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

void Track::setRingBufferSize( int size ) {

    ringSize = size;
    isRing = true;
}

void Track::push( Frame _frame ) {
    
    if( _frame.hasRotation() && ( (!nOfFrames()) || hasRotation ) ) {
        
        hasRotation = true;
        
        if( rotation.isTimestamped() && _frame.hasTime() ) {
            
            rotation.pushTimedFrame( _frame.getRotation(), _frame.time() );
            
        } else {
            
            rotation.pushFrame( _frame.getRotation() );
        }
        
        if( rotationOffset.n_elem == 0 ) {
            
            rotationOffset = _frame.getRotationOffset();
        }
    }
    
    if( position.isTimestamped() && _frame.hasTime() ) {
        
        position.pushTimedFrame( _frame.getPosition(), _frame.time() );
        
    } else {
        
        arma::mat temp = _frame.getPosition();
        position.pushFrame( temp );
    }
}

Trace Track::trace( string name ) {
    
    // TODO Is that guy still compatible with containers?

    Trace oneTrace;
    int nIdx = -1; // Initialise
    bool isFound = false; // Flag
    
    if( hasNodeList ) {
        
        for( int n=0; n<nodeList->size(); n++ ) {
            
            // If we find a matching name, we save index
            if( nodeList->at(n).compare( name ) == 0 ) {
                
                isFound = true;
                nIdx = n;
            }
        }
        
        if( hasSynoList && !isFound ) {
            
            for( int n=0; n<nodeList->size(); n++ ) {
                
                std::string nNameQuery = "nq"; // Synonym of name query
                std::string nFromList = "fl"; // Synonym of nth in list
                
                synoList->search( name, nNameQuery ); // Search name
                synoList->search( nodeList->at(n), nFromList ); // & list
                
                if( nFromList.compare( nNameQuery ) == 0 ) {
                    
                    nIdx = n;
                    isFound = true;
                }
            }
        }
        
        if( isFound ) {
            
            
            return this->trace(nIdx);
            //oneTrace.setName( name );
            //oneTrace.setTimeFlag( true );
            //oneTrace.setRotationFlag( hasRotation );
            
            //for( int f=0; f<nOfFrames(); f++ ) {
                
            //    oneTrace.push( frame( f ).node( name ) );
            //}
            
        } else {
            
            std::cout << "Frame::node: Node not found" << std::endl;
        }
        
    } else {
        
        std::cout << "Frame::node: No node name list" << std::endl;
    }


    return( oneTrace );
}

Trace Track::trace( int index ) {

    Trace oneTrace;

    oneTrace.setTimeFlag( true );
    oneTrace.setRotationFlag( hasRotation );
    oneTrace.setName( nodeList->at(index) );
    
    if( position.isTimestamped() ) {
        
        oneTrace.setPosition( position.getData().tube( 0,index, 2 , index ), position.getTimeStamps() );
        
    } else {
        //arma::cube test=(position.getData().tube( 0,index, 2, index ));
        //test=reshape(test,test.n_rows,test.n_slices,1);
        //arma::mat test2=test.slice(0);
        oneTrace.setPosition( position.getData().tube( 0,index, 2, index ), position.getFrameRate() );
    }
    if (hasRotation){
        if( rotation.isTimestamped() ) {
        
            oneTrace.setRotation( rotation.getData().tube( 0,index, 2, index), position.getTimeStamps() );
            oneTrace.setRotationOffset(rotationOffset.col(index));
        
        }
        else {
            oneTrace.setRotation( rotation.getData().tube(0, index, 2, index ), position.getFrameRate() );
            oneTrace.setRotationOffset(rotationOffset.col(index));
        }
    }
    
    return( oneTrace );
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

    if( nOfFrames() > 0 && hasNodeList ) {

        Frame oneFrame = frame( 0 );
        nIdx = oneFrame.index( name );
    }

    return( nIdx );
}

void Track::clear( void ) {
    
    rotation.clear();
    position.clear();
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
    
    subTr.hasSynoList = hasSynoList;
    
    if( hasSynoList ) {
        
        subTr.synoList = new SynoList();
        *(subTr.synoList) = *synoList;
    }
    
    subTr.easyName = easyName;
    subTr.fileName = fileName;
    subTr.frameRate = frameRate;
    subTr.ringSize = ringSize;
    subTr.isRing = isRing;
    
    subTr.hasRotation = hasRotation;
    
    if( hasRotation ) {
        
        if( rotation.isTimestamped() ) {
            
            // TODO
            
            subTr.rotation.setTimedData( rotation.getTimeStamps().subvec( beg, end ), rotation.getData().slices( beg, end ) );
            subTr.rotationOffset = rotationOffset;
            
        } else {
            
            subTr.rotation.setData( rotation.getFrameRate(), rotation.getData().slices( beg, end ) );
            subTr.rotationOffset = rotationOffset;
        }
    }
    
    if( position.isTimestamped() ) {
        
        // TODO
        
        subTr.position.setTimedData( position.getTimeStamps().subvec( beg, end ),position.getData().slices( beg, end ) );
        
    } else {
        
        subTr.position.setData( position.getFrameRate(), position.getData().slices( beg, end ) );
    }
}
