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
		const MoMa::BoneList *boneList;
		const MoMa::NodeList *nodeList;
		std::vector<MoMa::ofBone> bones;
        ofSpherePrimitive mNodeSphere;
        double mNodeSize;
	};
}
#endif

