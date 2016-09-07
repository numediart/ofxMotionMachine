/**
*
*  @file mmOfSkeleton.h
 * @brief Utilitary functions to use MoMa in OF.
*  @copyright Numediart Institute, UMONS (c) 2014-2016
*
*/

#ifndef __mmOfSkeleton__
#define __mmOfSkeleton__
namespace MoMa { class ofSkeleton; }

#include <armadillo>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxUI.h"

#include "MoMa.h"
#include "mmCam.h"
#include "mmOfBone.h"
#include "mmOfUtils.h"
#include "mmCanvas.h"
namespace MoMa {
    class ofSkeleton
    {
        /**
        * A class that provides a way to display a skeleton in a MotionMachine OFX windows
        *
        */
    public:
        ofSkeleton( const MoMa::Track &pTrack );
        ~ofSkeleton();
        void draw( unsigned int index );
        void draw( double time );

        void boneLocalDraw( const arma::mat &rotation, const arma::mat &position, boneMapType::iterator it );
        void boneGlobalDraw( const arma::mat &rotation, const arma::mat &position, boneMapType::iterator it );
        inline bool isValid() const { return _isValid; };
        inline void setNodeSize( double nodeSize ) { mNodeSize = nodeSize; };
	protected:
		bool _isValid;
		const MoMa::Track &track;
		const std::shared_ptr<MoMa::BoneList>boneList;
		const std::shared_ptr<MoMa::NodeList> nodeList;
		std::vector<MoMa::ofBone> bones;
        ofSpherePrimitive mNodeSphere;
        double mNodeSize;
	};
}
#endif

