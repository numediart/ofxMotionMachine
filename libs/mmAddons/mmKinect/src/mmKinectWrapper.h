#ifndef MMKINECTWRAPPER_H
#define MMKINECTWRAPPER_H
#include "MoMa.h"
#include "ofMain.h"
#include "KinectV2.h"
#include "mmOfSkeleton.h"

//If necessary: divide the pure MoMa code from the OF part of the code. But the threading is implemented by of then, it's difficult to plan a dividedarchitecture
namespace MoMa {
	class kinectWrapper {
		typedef shared_ptr<MoMa::ofSkeleton> ofSkeletonPtr;
	public:
		kinectWrapper(MoMa::Track &track);
		~kinectWrapper();
		void setup();
		void update();
		void drawSkeleton(unsigned int frameIndex);
		void drawSkeleton(double frametime);
		void drawPlane();
	protected:
		ofMatrix4x4 getLocalSystem();
		KINECTV2::KinectV2 kin;
		ofVec3f groundVec;
		float groundDist;
		MoMa::Track &mTrack;
		ofSkeletonPtr mSkeleton;
	};
};


#endif//MMKINECTWRAPPER_H