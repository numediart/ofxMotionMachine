/**
 *
 *  @file mmFrame.h
 *  @brief MotionMachine header file for Frame class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmFrame__
#define __mmFrame__

#include <armadillo>

#include "mmNodeList.h"
#include "mmBoneList.h"
#include "mmSynoList.h"

#include "mmNode.h"

namespace MoMa {
    
    class Frame  {
        
    public:
        
        Frame( void ); // Various ways to construct frame: cartesian and/or rotations
        Frame( arma::mat cartesianData, arma::mat rotationData, arma::mat rotOffset );
        Frame( arma::mat cartesianData, arma::mat rotationData );
        Frame( arma::mat cartesianData );
        
        void setPositionData( arma::mat cartesianData ); // Set data directly
        void setRotationData( arma::mat rotationData, arma::mat rotOffset );
        
        void setNodeList( NodeList *nList ); // Set node list
        void setBoneList( BoneList *bList ); // Set bone list
        void setSynoList( SynoList *sList ); // Set syno list
        
        double time( void ) { return( _time ); } // Get time
        bool hasTime( void ) { return( _hasTime ); } // Flag
        void setTime( double t ); // Set time ( with flag )
        
        inline Node node( std::string name ); // Query node by name ( from string )
        inline Node node( int index ); // Query node by index ( from int )
        
        inline Node operator()( std::string name ); // Short version of node()
        inline Node operator()( int index ); // Short version of node()
        
        bool hasRotation( void ) { return( _hasRotation ); } // Use rotation?
        inline void setRotationFlag( bool rot ) { _hasRotation = rot; } // Force it
        
        inline void push( Node node ) {
            
            if (node.hasRotation()&&((!nOfNodes())||this->hasRotation())){
                _hasRotation=true;
                rotation.insert_cols(rotation.n_cols, node.rotation);

                rotationOffset.insert_cols(rotationOffset.n_cols,node.rotationOffset);
            }
            position.insert_cols(position.n_cols, node.position);
            
        } // Add node//Can we push a node if there is a nodelist?? it will not be compatible, will it?
        
        arma::mat matrix( void ); // Extract Armadillo matrix from frame
        arma::mat m( void ) { return( matrix() ); } // same thing shorter
        
        int index( std::string name ); // Get index from name
        inline arma::mat getPosition(){return position;};
        inline arma::mat getRotation(){return rotation;};
        inline arma::mat getRotationOffset(){return rotationOffset;};
        
        int nOfNodes( void ); // Get number of nodes in the frame
        void print( void ); // Print the content of the frame
        
        void clear( void ) {
            position.clear();
            rotation.clear();
            rotationOffset.clear();
            _hasRotation=false;
            
        }; // Clear frame
        
        NodeList *nodeList; // Node list
        bool hasNodeList; // Check if set
        
        BoneList *boneList; // Bone list
        bool hasBoneList; // Check if set
        
        SynoList *synoList; // Synonym list
        bool hasSynoList; // Check if set
        
    protected:
        arma::mat position;
        arma::mat rotation;
        arma::mat rotationOffset;
        
        bool _hasRotation; // Do we use angles?
        
        double _time; // Given time stamp ( if any )
        bool _hasTime; // Do we have a time stamp?
    };
    
    // Inlined functions
    
    Node Frame::node( std::string name ) {
        
        Node nod;
        
        // TODO Make this modular
        
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
                
                nod.setPosition( position.col(nIdx) );
                nod.setRotationFlag( hasRotation() );
                
                if( hasRotation() ) {
                    
                    nod.setOffsetRotation( rotationOffset.col(nIdx) );
                    nod.setRotation( rotation.col(nIdx) );
                    
                }
                
                if( hasNodeList ) {
                    
                    nod.setName( nodeList->at( nIdx ) );
                }
                
                if( hasTime() ) {
                    
                    nod.setTime( _time );
                }
                
            } else {
                
                std::cout << "Frame::node: Node not found" << std::endl;
            }
            
        } else {
            
            std::cout << "Frame::node: No node name list" << std::endl;
        }
        
        return( nod );
    }
    
    Node Frame::node( int index ) {
        
        Node nod;
        
        if( index >= 0 && index < nOfNodes() ) {
        
            nod.setPosition( position.col(index) );
            nod.setRotationFlag( hasRotation() );
        
            if( hasRotation() ) {
                
                nod.setOffsetRotation( rotationOffset.col(index) );
                nod.setRotation( rotation.col(index) );
            }
            
            if( hasNodeList ) {
                
                nod.setName( nodeList->at( index ) );
            }
            
            if( hasTime() ) {
                
                nod.setTime( _time );
            }
        }
        
        return( nod );
    }
    
    Node Frame::operator()( std::string name ){
    
        return( this->node( name ) );
    }
    
    Node Frame::operator()( int index ){
    
        return( this->node( index ) );
    }
}

#endif
