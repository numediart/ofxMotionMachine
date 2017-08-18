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
		kinectWrapper(MoMa::Track &track, int bufferSize=120);
		~kinectWrapper();
		void setup();
		bool checkUserId(arma::vec head);
		bool update();//return true if a new frame is acquired
		void drawSkeleton(unsigned int frameIndex);
		void drawSkeleton(double frametime);
		void drawSkeleton();
		void drawKinectBox();
		void drawPlane();
		void calib(arma::vec pLeftHand, arma::vec pRightHand);// , arma::vec pHead);
	protected:
		ofMatrix4x4 getLocalSystem();
		ofMatrix4x4 mOfLocMatrix;
		arma::mat mLocMatrix;
		KINECTV2::KinectV2 kin;
		ofVec3f groundVec;
		float groundDist;
		MoMa::Track &mTrack;
		ofSkeletonPtr mSkeleton;
		float mRot;
		ofVec3f mTrans;
		int mBodyId ;
		int numBodies;
	};
};


#endif//MMKINECTWRAPPER_H