#include "mmKinectWrapper.h"

namespace MoMa {
	kinectWrapper::kinectWrapper(MoMa::Track &track, int bufferSize):mTrack(track){
		bool camConnected = (int)SUCCEEDED(kin.Initialize()) == 0;
		groundDist = 0;
		groundVec = ofVec3f(0,1, 0);
		mTrack.load(getAbsoluteResPath() + "kinect2.nodes");
		mTrack.load(getAbsoluteResPath() + "kinect2.bones");
		mTrack.setRingBufferSize(bufferSize, true);
		mSkeleton = std::make_shared<ofSkeleton>(track);
		mRot = 0;
	};
	kinectWrapper::~kinectWrapper() {
		kin.Finalize();
	};
	void kinectWrapper::setup() {};
	bool kinectWrapper::update() {
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
			if (mLocMatrix.n_elem == 0) {
				ofMatrix4x4 locOfMat = this->getLocalSystem();
				mLocMatrix = arma::conv_to<arma::mat>::from(arma::fmat(locOfMat.getPtr(), 4, 4));
			}
			arma::mat transfMat = mLocMatrix;
			//arma::fmat transfMat(locOfMat.getPtr(),4,4);
			//std::cout << locOfMat << std::endl;
			//std::cout << transfMat << std::endl;

			for (std::map<std::string, int>::iterator it= mTrack.nodeList->begin();it!= mTrack.nodeList->end();it++){
				arma::vec lVec(4);
				lVec << -bodies[bodyId].Joints[it->second].Joint.Z * 1000
					<< -bodies[bodyId].Joints[it->second].Joint.X * 1000
					<< bodies[bodyId].Joints[it->second].Joint.Y * 1000
					<< 1.0;
				//std::cout << lVec << std::endl;
				lVec = transfMat*lVec;
				//std::cout << lVec.subvec(0, 2) / lVec(3) << std::endl;
				cartData.col(it->second) = lVec.subvec(0, 2)/lVec(3);
				/*cartData(0, it->second) = -bodies[bodyId].Joints[it->second].Joint.Z*1000;
				cartData(1, it->second) = -bodies[bodyId].Joints[it->second].Joint.X*1000;
				cartData(2, it->second) = bodies[bodyId].Joints[it->second].Joint.Y*1000;*/
			}
			if (mTrack.hasRotation) {
				for (boneMapType::iterator it = mTrack.boneList->begin(); it != mTrack.boneList->end(); it++) {
					MoMa::quaternion lQuat = quaternion(
						-bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.z,
						-bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.x,
						bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.y,
						bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.w);
					rotData(0, it->second.boneId) = -bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.z;
					rotData(1, it->second.boneId) = -bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.x;
					rotData(2, it->second.boneId) = bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.y;
					rotData(3, it->second.boneId) = bodies[bodyId].Joints[it->second.jointChildren[0]].JointOrientation.w;
					arma::mat lMat;
					lQuat.get(lMat);
					lMat = (transfMat*lMat*arma::inv(transfMat));
					lQuat.set(lMat);
					rotData.col(it->second.boneId) = lQuat;

				}
				Frame lFrame(cartData,rotData);
				mTrack.push(lFrame);

			}
			else {
				Frame lFrame(cartData);
				mTrack.push(lFrame);
			}
			if (mTrack.nOfFrames() == 1)
				mTrack.setJointOffsetRotation();
			return true;

		}
		return false;
	};
	void kinectWrapper::drawSkeleton(unsigned int frameIndex) {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		//ofMultMatrix(getLocalSystem());
		ofRotateZ(mRot);
		//ofTranslate(mTrans);
		mSkeleton->draw(frameIndex);
		ofPopMatrix();
	};
	void kinectWrapper::drawSkeleton(double frameTime) {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		//ofMultMatrix(getLocalSystem());
		ofRotateZ(mRot);
		ofTranslate(mTrans);
		mSkeleton->draw(frameTime);
		ofPopMatrix();
	};


	void kinectWrapper::drawSkeleton() {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		//ofMultMatrix(getLocalSystem());
		ofRotateZ(mRot);
		ofTranslate(mTrans);
		mSkeleton->draw(mTrack.maxTime());
		ofPopMatrix();
	};
	void kinectWrapper::calib(arma::vec pLeftHand, arma::vec pRightHand){//, arma::vec pHead) {
		if (mTrack.nOfFrames() == 0)
			return;
		ofMatrix4x4 locOfMat = this->getLocalSystem();
		mLocMatrix = arma::conv_to<arma::mat>::from(arma::fmat(locOfMat.getPtr(), 4, 4));
		mTrack.setJointOffsetRotation();
		//unsigned int headId = mTrack.nodeList->index("Head");
		unsigned int rHandId = mTrack.nodeList->index("RWrist");
		unsigned int lHandId = mTrack.nodeList->index("LWrist");
		//arma::vec kinHead = mTrack.position.getLast().col(headId);
		arma::vec kinRHand = mTrack.position.getLast().col(rHandId);
		arma::vec kinLHand = mTrack.position.getLast().col(lHandId);
		arma::vec kinDir = kinRHand - kinLHand;
		kinDir(2) = 0;
		kinDir = arma::normalise(kinDir);
		arma::vec pDir = pRightHand - pLeftHand;
		pDir(2) = 0;
		pDir = arma::normalise(pDir);
		double rot = acos(dot(pDir,kinDir ));
		arma::vec temp = arma::cross(kinDir, pDir);
		if (temp(2) > 0)
			rot = rot;
		else
			rot = -rot;
		mRot = RAD_TO_DEG*rot;
		std::cout << "rot:" << mRot << std::endl;
		arma::mat rotMat(4, 4);
		rotMat << cos(rot) << sin(rot) << 0 << 0 << arma::endr
			<< -sin(rot) << cos(rot) << 0 << 0 <<arma::endr
			<< 0 << 0 << 1 << 0 <<arma::endr
			<< 0 << 0 << 0 << 1;
		//kinHead = rotMat.submat(0, 0, 2, 2)*kinHead;
		//kinRHand = rotMat.submat(0, 0, 2, 2)*kinRHand;
		//kinLHand = rotMat.submat(0, 0, 2, 2)*kinLHand;
		pRightHand = rotMat.submat(0, 0, 2, 2)*pRightHand;
		pLeftHand = rotMat.submat(0, 0, 2, 2)*pLeftHand;

		arma::vec trans = ((  pLeftHand + pRightHand-kinRHand - kinLHand) / 2);
		trans(2) = 0;
		arma::mat transMat(4, 4);
		transMat.eye();
		transMat(0, 3) = trans(0);
		transMat(1, 3) = trans(1);
		mTrans = ofVec3f(trans(0), trans(1), trans(2));
		//mLocMatrix = rotMat*mLocMatrix;
		//mLocMatrix = transMat*mLocMatrix;


	}


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
		//ofTranslate(0, 0, groundDist * 1000);
		//planeMatrix(0, 3) = 0;
		//planeMatrix(1, 3) = 0;
		planeMatrix(3, 2) = groundDist * 1000;
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