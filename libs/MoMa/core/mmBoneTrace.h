/**
 *
 *  @file mmBoneTrace.h
 *  @brief MotionMachine header file for BoneTrace class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmBoneTrace__
#define __mmBoneTrace__

#include <vector>
#include <iostream>

#include <armadillo>
#include "mmContainer.h"

#include "mmBone.h"

namespace MoMa {

    class BoneTrace {
        
        // TODO It seems the new traces are "transposed"
        // compared to what is expected. It gives me the
        // nOfFrames() = 3 and lots of dims vs. lots of
        // frames and 3 dims, i.e. x,y,z of the node.
        
      public:
        
        BoneTrace( void ); // Default constructor
        
        std::string name( void ) const{ return( _name ); } // Get name
        inline void setName( std::string n ) { _name = n; } // Set name
        
        bool hasTime( void ) { return( position.isTimed() &&(hasRotation()?rotation.isTimed():1)); } // Flag
        inline void setTimeFlag( bool tim ) { _hasTime = tim; } // Force it//TODO verify if it is necessary, a trace has necessarely a time, but not a timedstamp time scale.
        
        bool hasRotation( void ) const{ return( _hasRotation ); } // Use rotation?
        inline void setRotationFlag( bool rot ) { _hasRotation = rot; } // Force it
        arma::mat matrix( void ) ;
        inline arma::vec getFramePosition( unsigned int index ) { return( position.get(index) ); } // By index
        arma::vec getFramePosition( double time ){ return( position.get(time) ); }; // Query bone by time in the trace
        inline arma::vec getFramerotation( unsigned int index ) { return( rotation.get(index) ); } // By index
        arma::vec getFramerotation( double time ){ return( rotation.get(time) ); }; // Query bone by time in the trace
        
        inline void setPosition(const arma::mat &data,float fr, float initT = 0.0f){position.setData(fr, data, initT);};
        inline void setRotation(const arma::mat &data,float fr, float initT = 0.0f){rotation.setData(fr, data, initT);};
        inline void setRotationOffset(const arma::vec &data){ rotationOffset.resize(1); rotationOffset[0] = data; };
        inline void setRotationOffset(const std::vector<arma::vec> &data) { rotationOffset = data; };
        inline void setPosition(const arma::mat &data,const arma::vec &time){position.setData(time, data);};
        inline void setRotation(const arma::mat &data,const arma::vec &time ){rotation.setData(time, data);};
        inline void setBoneLength(std::vector<double> bLength) { boneLength = bLength; };

        inline const double & getBoneLength(int i) const { return boneLength[i]; };
        inline const arma::vec & getRotationOffset(int i) const { return rotationOffset[i]; };
        
        inline Bone bone( unsigned int index );
        inline Bone bone( double time );

        inline Bone operator[]( unsigned int index ); // Short version of bone()
        inline Bone operator[]( double time ); // Short version of bone()
        
        inline void push( Bone bone ) {
            if (!position.isTimed()){
                position.push( bone.position );
					rotation.push(bone.rotation );
					if (rotationOffset.size()==0)
						rotationOffset = bone.getRotationOffset();
            }
            else{
                position.push( bone.position,bone.time() );
               
                    rotation.push(bone.rotation, bone.time() );
                    if (rotationOffset.size() == 0)
                        rotationOffset= bone.getRotationOffset();
            }
        
        } // Add bone
        
        inline arma::mat getRotation( void ) {return rotation.getData();}; // Extract Armadillo matrix from trace
        inline arma::mat getPosition( void ) {return position.getData();}; // Extract Armadillo matrix from trace
        arma::mat m( void ){return _hasRotation?arma::join_rows(position.getData(),rotation.getData()):position.getData();}; // same thing shorter
        // TODO void setMatrix( mat data ); // Set trace from matrix
        
        inline int nOfFrames( void ){ // Get number of frmae in the trace
            return( std::max(position.nOfFrames(),rotation.nOfFrames()));
        }
        void print( void ); // Print the content of the trace
        
    // protected:
        
        // TODO Re-protect this
        
        TimedMat position;
        TimedMat rotation;
        std::vector<arma::vec> rotationOffset; // Quaternion offset
        std::vector<double> boneLength;
        bool _hasTime;
        bool _hasRotation;
        std::string _name;
    };
    
    // Inlined functions

    Bone BoneTrace::bone(unsigned int index) {

        return Bone(position.get(index), rotation.get(index), rotationOffset, boneLength);
    }

    Bone BoneTrace::bone(double time) {

        return Bone(position.get(time), rotation.get(time), rotationOffset, boneLength);
    }
    
    Bone BoneTrace::operator[]( unsigned int index ){
        
        return Bone(position.get(index), rotation.get(index), rotationOffset, boneLength);
    }
    
    Bone BoneTrace::operator[]( double time ){
        
        return Bone(position.get(time), rotation.get(time), rotationOffset, boneLength);
    }
}

#endif
