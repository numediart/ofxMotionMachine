
	/**
	*
	*  @file mmNode.h
	*  @brief MotionMachine header file for Node class.
	*  @copyright Numediart Institute, UMONS (c) 2014-2015
	*
	*/

#ifndef __mmBone__
#define __mmBone__

#include <cstring>
#include <armadillo>

	namespace MoMa {
#ifndef _PositionDimension_
#define _PositionDimension_
		enum PositionDimension {

			X, // Position or rotation @ x
			Y, // Position or rotation @ y
			Z, // Position or rotation @ z
			T // Index for the time-stamp
		};
#endif

#ifndef _RotationDimension_
#define _RotationDimension_
		enum RotationDimension {

			qX, // Quaternion @ x
			qY, // Quaternion @ y
			qZ, // Quaternion @ z
			qW // Quaternion @ w
		};
#endif
		class Bone {

		public:

			Bone(void); // Default constructor
			Bone(float x, float y, float z); // Other kinds of constructors
			Bone( float x, float y, float z, float qx, float qy, float qz, float qw );
			Bone(arma::vec coord, arma::vec rotation, arma::vec offsetRot, double length);
			Bone(arma::vec coord, arma::vec rot, std::vector< arma::vec> offsetRot, std::vector< double> lengths);

			void setPosition(float x, float y, float z); // Set position
			void setRotation( float x, float y, float z , float w); // Set rotation

			void setPosition(arma::vec pos); // Set position
			void setRotation( arma::vec rot); // Set rotation
			void setBoneData( std::vector<arma::vec> off, std::vector< double> lengths); // Set offset
			
			//inline void setBoneLength(std::vector<double> length) { this->boneLength = length; }; //we have the same number of length tnath the number of dest node 
			inline const int getSize() const { return boneLength.size(); };
			inline const double & getBoneLength(int i) const { return boneLength[i]; };
			inline const arma::vec & getRotationOffset(int i) const { return rotationOffset[i]; };
            inline const std::vector<arma::vec> & getRotationOffset() const { return rotationOffset; };

			inline bool hasRotation( void ) const { return( _hasRotation ); } // Use rotation?
			inline void setRotationFlag( bool rot ) { _hasRotation = rot; } // Force it

			double time(void)const { return(_time); } // Get time
			bool hasTime(void)const { return(_hasTime); } // Flag
			inline void setTime(double t); // Set time tag

			std::string name(void) const { return(_name); } // Get name
			inline void setName(std::string n) { _name = n; } // Set name

			int state(void) const { return(_state); }
			void setState(int st) { _state = st; }

			arma::vec position; // 3D position of the node
			arma::vec rotation; // Quaternion rotation
		protected:
			std::vector<arma::vec> rotationOffset; // Quaternion offset
			std::vector<double> boneLength;

			bool _hasRotation; // Do we use rotation?
			double _time; // Transfered time stamp
			bool _hasTime; // Any time stamp?
			std::string _name; // Bone name
			int _state; // Bone state
		};

		// Inlined functions

		void Bone::setTime(double t) {

			_hasTime = true;
			_time = t;
		}
	}

#endif

