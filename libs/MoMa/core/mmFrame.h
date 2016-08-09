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
#include "mmBone.h"

namespace MoMa {
    
    class Frame  {
        
    public:
        
        Frame( void ); // Various ways to construct frame: cartesian and/or rotations
        Frame( arma::mat cartesianData, arma::mat rotationData, arma::mat rotOffset );
        Frame( arma::mat cartesianData, arma::mat rotationData );
        Frame( arma::mat cartesianData );
        
        void setPositionData( arma::mat cartesianData ); // Set data directly
        void setRotationData( arma::mat rotationData, arma::mat rotOffset );
        
        void setNodeList( std::shared_ptr<NodeList> nList ); // Set node list
        void setBoneList( std::shared_ptr<BoneList> bList ); // Set bone list
        void setSynoList( SynoList *sList ); // Set syno list
        
        double time( void ) const { return( _time ); } // Get time
        bool hasTime( void ) const { return( _hasTime ); } // Flag
        void setTime( double t ); // Set time ( with flag )
        
        inline Node node( std::string name) const; // Query node by name ( from string )
		inline Node node(int index) const; // Query node by index ( from int )
        
        Bone bone( std::string name ) const; // Query node by name ( from string )
		Bone bone(int index) const; // Query node by index ( from int )
        
        inline Node operator()( std::string name ); // Short version of node()
        inline Node operator()( int index ); // Short version of node()
        
        bool hasRotation( void )const { return( _hasRotation ); } // Use rotation?
        inline void setRotationFlag( bool rot ) { _hasRotation = rot; } // Force it
        
        inline void push( Node node ) {

            //if (node.hasRotation()&&((!nOfNodes())||this->hasRotation())){
            //    _hasRotation=true;
            //    rotation.insert_cols(rotation.n_cols, node.rotation);

            //    rotationOffset.insert_cols(rotationOffset.n_cols,node.rotationOffset);
            //}
            position.insert_cols(position.n_cols, node.position);
            
        } // Add node//Can we push a node if there is a nodelist?? it will not be compatible, will it?
        
        arma::mat matrix( void ); // Extract Armadillo matrix from frame
        arma::mat m( void ) { return( matrix() ); } // same thing shorter
        
        int index( std::string name ); // Get index from name
        inline const arma::mat &getPosition()const {return position;};
        inline const arma::mat &getRotation()const {return rotation;};
        inline const arma::mat &getRotationOffset()const {return rotationOffset;};
        
        int nOfNodes( void ) const; // Get number of nodes in the frame
        void print( void ); // Print the content of the frame
        
        void clear( void ) {
            position.clear();
            rotation.clear();
            rotationOffset.clear();
            _hasRotation=false;
            
        }; // Clear frame
        
        std::shared_ptr<NodeList> nodeList; // Node list
        bool hasNodeList; // Check if set
        
        std::shared_ptr<BoneList> boneList; // Bone list
        bool hasBoneList; // Check if set
        
		bool hasGlobalCoordinate;//if true, coordinates are encoded in a global coordinate system. If false (rotation flag and skeleton necessary) coordinate of each segment are encoded in a local system link to the parent bone.

        // SynoList *synoList; // Synonym list
        // bool hasSynoList; // Check if set
        
    protected:
        arma::mat position;
        arma::mat rotation;
        arma::mat rotationOffset;
        
        bool _hasRotation; // Do we use angles?
        
        double _time; // Given time stamp ( if any )
        bool _hasTime; // Do we have a time stamp?
    };
    
    // Inlined functions
    
    Node Frame::node( std::string name ) const {
        
        Node nod;
        int nIdx = -1;
        
        if( hasNodeList ) {
        
            nIdx = nodeList->index( name );
            
            if( nIdx > -1 ) {
                
                nod.setPosition( position.col( nIdx ) );
                //nod.setRotationFlag( hasRotation() );
                
                //if( hasRotation() ) {
                    
                //    nod.setOffsetRotation( rotationOffset.col( nIdx ) );
                //    nod.setRotation( rotation.col(nIdx) );
                //}
                
                if( hasNodeList ) {
                    
                    nod.setName( nodeList->name( nIdx ) );
                }
                
                if( hasTime() ) {
                    
                    nod.setTime( _time );
                }
            
            } else {
            
                std::cout << "Frame::node(): Node not found" << std::endl;
            }
            
        } else {
            
            std::cout << "Frame::node(): No node name list" << std::endl;
        }
        
        /*
        // TODO Make this modular
        
        int nIdx = -1; // Initialise
        bool isFound = false; // Flag
        
        if( hasNodeList ) {
            
            for( int n=0; n<nodeList->size(); n++ ) {
                
                // If we find a matching name, we save index
                if( nodeList->name(n).compare( name ) == 0 ) {
                    
                    isFound = true;
                    nIdx = n;
                }
            }
            
            if( hasSynoList && !isFound ) {
                
                for( int n=0; n<nodeList->size(); n++ ) {
                    
                    std::string nNameQuery = "nq"; // Synonym of name query
                    std::string nFromList = "fl"; // Synonym of nth in list
                    
                    synoList->search( name, nNameQuery ); // Search name
                    synoList->search( nodeList->name(n), nFromList ); // & list
                    
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
                    
                    nod.setName( nodeList->name( nIdx ) );
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
        */
        
        return( nod );
    }
    
    Node Frame::node( int index ) const {
        
        Node nod;
        
        if( index >= 0 && index < nOfNodes() ) {
        
            nod.setPosition( position.col(index) );
            //nod.setRotationFlag( hasRotation() );
        
            //if( hasRotation() ) {
                
            //    nod.setOffsetRotation( rotationOffset.col(index) );
            //    nod.setRotation( rotation.col(index) );
            //}
            
            if( hasNodeList ) {
                
                nod.setName( nodeList->name( index ) );
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
