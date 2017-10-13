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
		mTrans = ofVec3f(0, 0, 0);
		mBodyId = -1;
	};
	kinectWrapper::~kinectWrapper() {
		kin.Finalize();
	};
	bool kinectWrapper::checkUserId(arma::vec head) {
		//HRESULT result = kin.Update();
		if (kin.getUsersTrackedCount() > 0) {
			KINECTV2::BODY_STRUCT * bodies = kin.GetBodies();
			if (mBodyId >= kin.GetBodiesSize())
				return false;//need catch new mBodyId

			arma::mat rotMat = MoMa::rotMat(mRot, 2);
			arma::mat transMat = MoMa::translMat(arma::conv_to< arma::vec >::from(arma::fvec(mTrans.getPtr(),3))) ;
			
			arma::mat transfMat = mLocMatrix;
			//arma::mat transfMat = rotMat*transMat*mLocMatrix;

			unsigned int headId = mTrack.nodeList->index("Head");
			arma::vec lDist(kin.GetBodiesSize());

			for (int i = 0; i < kin.GetBodiesSize(); i++) {
				if (bodies[i].IsTracked) {
					arma::vec lVec(4);
					lVec << -bodies[i].Joints[headId].Joint.Z * 1000
						<< -bodies[i].Joints[headId].Joint.X * 1000
						<< bodies[i].Joints[headId].Joint.Y * 1000
						<< 1.0;
					//std::cout << lVec << std::endl;
					lVec = rotMat*transMat*transfMat*lVec;
					//std::cout << lVec.subvec(0, 2) / lVec(3) << std::endl;
					lDist(i) = arma::norm(lVec.subvec(0, 2) / lVec(3) - head);
				}
				else
					lDist(i) = arma::datum::inf;
			}
			arma::uword lTempId;
			if (lDist.min(lTempId))
			{
				mBodyId = lTempId;
				std::cout << "mBodyId" << mBodyId << std::endl;
				return true;
			}
			else {
				mBodyId = -1;
				return false;
			}

		}
		else
			return false;
	}
	void kinectWrapper::setup() {};
	arma::vec kinectWrapper::camSpaceVecToMoMa(const CameraSpacePoint pVec){
		arma::vec lVec(4);
		lVec << -pVec.Z * 1000
			<< -pVec.X * 1000
			<< pVec.Y * 1000
			<< 1.0;
		lVec = mLocMatrix*lVec;
		lVec = lVec.subvec(0, 2) / lVec(3);
		return lVec;
	}
	arma::vec kinectWrapper::camSpaceQuatToMoMa(const Vector4 pQuat){
		MoMa::quaternion lQuat = quaternion(
			-pQuat.z,
			-pQuat.x,
			pQuat.y,
			pQuat.w);
		arma::mat lMat;
		lQuat.get(lMat);
		lMat = (mLocMatrix*lMat*arma::inv(mLocMatrix));
		lQuat.set(lMat);
		return lQuat;
	}
		
	bool kinectWrapper::update() {
		HRESULT result = kin.Update();
		//std::cout << "istracked:"<< kin.getUsersTrackedCount() << std::endl;
		if (SUCCEEDED(result)&& kin.getUsersTrackedCount()) {
			KINECTV2::BODY_STRUCT * bodies = kin.GetBodies();
			//int mBodyId = -1;
			if (mBodyId==-1)
				for (int i = 0; i < kin.GetBodiesSize(); i++)
					if (bodies[i].IsTracked)
						mBodyId = i;
			if (mBodyId >= kin.GetBodiesSize())
				return false;//need catch new mBodyId
			if (bodies[mBodyId].IsTracked == false)
				return false;
			arma::mat cartData(3,mTrack.nOfNodes());
			arma::mat rotData(4,mTrack.nOfBones());
			if (mLocMatrix.n_elem == 0) {
				mOfLocMatrix = this->getLocalSystem();
				mLocMatrix = arma::conv_to<arma::mat>::from(arma::fmat(mOfLocMatrix.getPtr(), 4, 4).t());//pay attention: memory management is not in the same between ofMat and arma::mat (then we use t())
				std::cout << mOfLocMatrix << std::endl;
				std::cout << mLocMatrix << std::endl;

			}

			arma::mat rotMat = MoMa::rotMat(mRot, 2);
			arma::mat transMat = MoMa::translMat(arma::conv_to< arma::vec >::from(arma::fvec(mTrans.getPtr(),3)));

			arma::mat transfMat = mLocMatrix;
			for (std::map<std::string, int>::iterator it= mTrack.nodeList->begin();it!= mTrack.nodeList->end();it++){
				cartData.col(it->second) = this->camSpaceVecToMoMa(bodies[mBodyId].Joints[it->second].Joint);
			}
			if (mTrack.hasRotation) {
				for (boneMapType::iterator it = mTrack.boneList->begin(); it != mTrack.boneList->end(); it++) {
					rotData.col(it->second.boneId) = this->camSpaceQuatToMoMa(bodies[mBodyId].Joints[it->second.jointChildren[0]].JointOrientation);
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
		return true;
	};
	void kinectWrapper::drawSkeleton(unsigned int frameIndex) {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		//ofMultMatrix(getLocalSystem());
		ofRotateZ(mRot);
		ofTranslate(mTrans);
		mSkeleton->draw(frameIndex);
		ofPopMatrix();
	};
	void kinectWrapper::drawKinectBox() {
		ofPushMatrix();
		//ofTranslate(0, 0, groundDist * 1000);
		//ofMultMatrix(getLocalSystem());
		ofBoxPrimitive kinectBox(50, 300, 50);
		ofRotateZ(mRot);
		ofTranslate(mTrans);
		ofMultMatrix(mOfLocMatrix);
		//ofTranslate(mTrans);
		ofPushStyle();
		ofSetColor(0, 0, 0);
		kinectBox.draw();
		ofPopStyle();

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
		ofDrawAxis(100);
		mSkeleton->draw(mTrack.maxTime());
		ofPopMatrix();
	};

	void kinectWrapper::calib(arma::vec pLeftHand, arma::vec pRightHand){
		if (mTrack.nOfFrames() == 0)
			return;
		ofMatrix4x4 locOfMat = this->getLocalSystem();
		mLocMatrix = arma::conv_to<arma::mat>::from(arma::fmat(locOfMat.getPtr(), 4, 4).t());//pay attention: memory management is not in the same between ofMat and arma::mat (then we use t())
		mTrack.setJointOffsetRotation();
		//unsigned int headId = mTrack.nodeList->index("Head");
		unsigned int rHandId = mTrack.nodeList->index("RWrist");
		unsigned int lHandId = mTrack.nodeList->index("LWrist");

		arma::vec kinRHand = mTrack.position.getLast().col(rHandId);
		arma::vec kinLHand = mTrack.position.getLast().col(lHandId);
		//arma::vec kinHead = mTrack.position.getLast().col(headId);
		
			arma::mat transfMat = mLocMatrix;
			KINECTV2::BODY_STRUCT * bodies = kin.GetBodies();
			//arma::mat transfMat = rotMat*transMat*mLocMatrix;

			arma::vec lHeight(kin.GetBodiesSize());

			for (int i = 0; i < kin.GetBodiesSize(); i++) {
				if (bodies[i].IsTracked) {
					arma::vec lVec = this->camSpaceVecToMoMa(bodies[i].Joints[lHandId].Joint);
					arma::vec rVec = this->camSpaceVecToMoMa(bodies[i].Joints[rHandId].Joint);
					lHeight(i) = (lVec(2) + rVec(2)) / 2.0;
				}
				else
					lHeight(i) = 0;
			}

			arma::uword lTempId;
			if (lHeight.max(lTempId) == 0) {
				mBodyId = -1;
				return;
			}
			if (mBodyId != lTempId) {
				mBodyId = lTempId;
				std::cout << "mBodyId" << mBodyId << std::endl;

				kinRHand = this->camSpaceVecToMoMa(bodies[mBodyId].Joints[rHandId].Joint);
				kinLHand = this->camSpaceVecToMoMa(bodies[mBodyId].Joints[lHandId].Joint);
			}
			
			arma::vec kinDir = kinRHand - kinLHand;
			kinDir(2) = 0;
			kinDir = arma::normalise(kinDir);
			arma::vec pDir = pRightHand - pLeftHand;
			pDir(2) = 0;
			pDir = arma::normalise(pDir);
			double rot = acos(dot(pDir, kinDir));
			arma::vec temp = arma::cross(kinDir, pDir);
			if (temp(2) > 0)
				rot = rot;
			else
				rot = -rot;
			mRot = RAD_TO_DEG*rot;
			std::cout << "rot:" << mRot << std::endl;
			arma::mat rotMat = MoMa::rotMat(mRot, 2).t();
			pRightHand = rotMat.submat(0, 0, 2, 2)*pRightHand;
			pLeftHand = rotMat.submat(0, 0, 2, 2)*pLeftHand;

			arma::vec trans = ((pLeftHand + pRightHand - kinRHand - kinLHand) / 2);
			trans(2) = 0;
			mTrans = ofVec3f(trans(0), trans(1), trans(2));

			mSkeleton = std::make_shared<ofSkeleton>(mTrack); 
	}


	ofMatrix4x4 kinectWrapper::getLocalSystem() {
		Vector4 tempVec = kin.GetFloorClipPlane();
		if (tempVec.w != 0) {
			groundVec = ofVec3f(-tempVec.z, -tempVec.x, tempVec.y);
			groundDist = tempVec.w;
		}
		ofMatrix4x4 planeMatrix;
		planeMatrix.makeIdentityMatrix();
		ofVec3f yVec;
		if (fabs(groundVec.dot(ofVec3f(0, 0, 1)))<fabs(groundVec.dot(ofVec3f(1, 0, 0))))
			yVec = ofVec3f(0, 0, 1).getCrossed(groundVec).normalized();
		else
			yVec = groundVec.getCrossed(ofVec3f(1, 0, 0)).normalized();

		ofVec3f xVec = yVec.getCrossed(groundVec).normalized();
		planeMatrix(0, 0) = xVec.x;
		planeMatrix(0, 1) = xVec.y;
		planeMatrix(0, 2) = xVec.z;
		planeMatrix(1, 0) = yVec.x;
		planeMatrix(1, 1) = yVec.y;
		planeMatrix(1, 2) = yVec.z;
		planeMatrix(2, 0) = groundVec.x;
		planeMatrix(2, 1) = groundVec.y;
		planeMatrix(2, 2) = groundVec.z;
		planeMatrix(2, 3) = groundDist * 1000;
		return planeMatrix;
	}
	void kinectWrapper::drawPlane() {
		Vector4 tempVec = kin.GetFloorClipPlane();
		if (tempVec.w != 0) {
			groundVec = ofVec3f(- tempVec.z,-tempVec.x, tempVec.y);
			groundDist = tempVec.w;
		}
		ofPushMatrix();
		ofPushStyle();
		ofSetLineWidth(10);
		//ofTranslate(0, 0, 1000);
		ofDrawAxis(1000);
		ofPopStyle();
		ofTranslate(0, 0,groundDist * 1000);
		ofMultMatrix(this->getLocalSystem());
		ofPushStyle();
		ofSetLineWidth(10);
		ofDrawAxis(1000);
		ofPopStyle();
		ofDrawGrid(1000,8,true,false,false,true);
		ofPopMatrix();
	};

}