#include "mmKinectWrapper.h"

namespace MoMa {
	kinectWrapper::kinectWrapper(MoMa::Track &track):mTrack(track){
		bool camConnected = (int)SUCCEEDED(kin.Initialize()) == 0;
		groundDist = 0;
		groundVec = ofVec3f(0,1, 0);
		mSkeleton = std::make_shared<ofSkeleton>(track);
	};
	kinectWrapper::~kinectWrapper() {
		kin.Finalize();
	};
	void kinectWrapper::setup() {};
	void kinectWrapper::update() {
		HRESULT result = kin.Update();
		//std::cout << "istracked:"<< kin.getUsersTrackedCount() << std::endl;
		if (SUCCEEDED(result)&& kin.getUsersTrackedCount()) {
			KINECTV2::BODY_STRUCT * bodies = kin.GetBodies();
			int bodyId = -1;
			for (int i = 0; i < kin.GetBodiesSize(); i++)
				if (bodies[i].IsTracked)
					bodyId = i;
			arma::mat cartData(3,mTrack.nOfNodes());
			arma::mat rotData(4,mTrack.nOfBones());
			for (std::map<std::string, int>::iterator it= mTrack.nodeList->begin();it!= mTrack.nodeList->end();it++){
				cartData(0, it->second) = -bodies[bodyId].Joints[it->second].Joint.Z*1000;
				cartData(1, it->second) = -bodies[bodyId].Joints[it->second].Joint.X*1000;
				cartData(2, it->second) = bodies[bodyId].Joints[it->second].Joint.Y*1000;
			}
			if (mTrack.hasRotation) {
				for (boneMapType::iterator it = mTrack.boneList->begin(); it != mTrack.boneList->end(); it++) {
					rotData(0, it->second.boneId) = -bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.z;
					rotData(1, it->second.boneId) = -bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.x;
					rotData(2, it->second.boneId) = bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.y;
					rotData(3, it->second.boneId) = bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.w;
				}
				Frame lFrame(cartData,rotData);
				mTrack.push(lFrame);

			}
			else {
				Frame lFrame(cartData);
				mTrack.push(lFrame);
			}

		}
		if (mTrack.nOfFrames() == 1)
			mTrack.setJointOffsetRotation();
	};
	void kinectWrapper::drawSkeleton(unsigned int frameIndex) {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		ofMultMatrix(getLocalSystem());
		mSkeleton->draw(frameIndex);
		ofPopMatrix();
	};
	void kinectWrapper::drawSkeleton(double frameTime) {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		ofMultMatrix(getLocalSystem());
		mSkeleton->draw(frameTime);
		ofPopMatrix();
	};

	ofMatrix4x4 kinectWrapper::getLocalSystem() {
		Vector4 tempVec = kin.GetFloorClipPlane();
		if (tempVec.w != 0) {
			groundVec = ofVec3f(-tempVec.z, -tempVec.x, tempVec.y);
			groundDist = tempVec.w;
		}
		ofMatrix4x4 planeMatrix;
		planeMatrix.makeIdentityMatrix();
		/*;planeMatrix(0, 0) = 0;
		planeMatrix(1, 0) = 0;
		planeMatrix(2, 0) = 1;
		planeMatrix(0, 1) = 1;
		planeMatrix(1, 1) = 0;
		planeMatrix(2, 1) = 0;
		planeMatrix(0, 2) = 0;
		planeMatrix(1, 2) = 1;
		planeMatrix(2, 2) = 0;
		ofMultMatrix(planeMatrix)*/
		ofVec3f yVec;
		if (fabs(groundVec.dot(ofVec3f(0, 0, 1)))<fabs(groundVec.dot(ofVec3f(1, 0, 0))))
			yVec = ofVec3f(0, 0, 1).getCrossed(groundVec).normalized();
		else
			yVec = groundVec.getCrossed(ofVec3f(1, 0, 0)).normalized();

		ofVec3f xVec = yVec.getCrossed(groundVec).normalized();
		planeMatrix(0, 0) = xVec.x;
		planeMatrix(1, 0) = xVec.y;
		planeMatrix(2, 0) = xVec.z;
		planeMatrix(0, 1) = yVec.x;
		planeMatrix(1, 1) = yVec.y;
		planeMatrix(2, 1) = yVec.z;
		planeMatrix(0, 2) = groundVec.x;
		planeMatrix(1, 2) = groundVec.y;
		planeMatrix(2, 2) = groundVec.z;
		ofTranslate(0, 0, groundDist * 1000);
		return planeMatrix;
	}
	void kinectWrapper::drawPlane() {
		Vector4 tempVec = kin.GetFloorClipPlane();
		if (tempVec.w != 0) {
			groundVec = ofVec3f(- tempVec.z,-tempVec.x, tempVec.y);
			groundDist = tempVec.w;
		}
		ofPushMatrix();
		ofMatrix4x4 planeMatrix;
		planeMatrix.makeIdentityMatrix();
		/*;planeMatrix(0, 0) = 0;
		planeMatrix(1, 0) = 0;
		planeMatrix(2, 0) = 1;
		planeMatrix(0, 1) = 1;
		planeMatrix(1, 1) = 0;
		planeMatrix(2, 1) = 0;
		planeMatrix(0, 2) = 0;
		planeMatrix(1, 2) = 1;
		planeMatrix(2, 2) = 0;
		ofMultMatrix(planeMatrix)*/
		ofPushStyle();
		ofSetLineWidth(10);
		//ofTranslate(0, 0, 1000);
		ofDrawAxis(1000);
		ofPopStyle();
		ofVec3f yVec;
		if (fabs(groundVec.dot(ofVec3f(0, 0, 1)))<fabs(groundVec.dot(ofVec3f(1, 0, 0))))
			yVec = ofVec3f(0, 0, 1).getCrossed(groundVec).normalized();
		else
			yVec = groundVec.getCrossed(ofVec3f(1, 0, 0)).normalized();

		ofVec3f xVec = yVec.getCrossed(groundVec).normalized();
		planeMatrix(0, 0) = xVec.x;
		planeMatrix(1, 0) = xVec.y;
		planeMatrix(2, 0) = xVec.z;
		planeMatrix(0, 1) = yVec.x;
		planeMatrix(1, 1) = yVec.y;
		planeMatrix(2, 1) = yVec.z;
		planeMatrix(0, 2) = groundVec.x;
		planeMatrix(1, 2) = groundVec.y;
		planeMatrix(2, 2) = groundVec.z;
		ofTranslate(0, 0,groundDist * 1000);
		ofMultMatrix(planeMatrix);
		ofPushStyle();
		ofSetLineWidth(10);
		ofDrawAxis(1000);
		ofPopStyle();
		ofDrawGrid(1000,8,true,false,false,true);
		ofPopMatrix();
	};

}