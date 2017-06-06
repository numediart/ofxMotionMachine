#include "mmOfMutedSkeleton.h"


using namespace MoMa;
ofMutedSkeleton::ofMutedSkeleton(const MoMa::Track &pTrack) : ofSkeleton(pTrack)
{
	if (track.hasGlobalCoordinate) {
		std::cerr << "Error ofMutedSkeleton::ofMutedSkeleton the linked track must have a local system";
	}
}


ofMutedSkeleton::~ofMutedSkeleton()
{
}

/*void ofMutedSkeleton::init() {
	ofSkeleton::init();

	this->_initiated == true;
}*/

void ofMutedSkeleton::displayNameListBone() {

	for (BoneList::iterator it = mutedBoneList.begin(); it != mutedBoneList.end(); it++) {
		std::cout << it->first << std::endl;
	}
	
}

bool ofMutedSkeleton::pushBone(std::string  parentMutedBoneName, std::string newChildrenBoneNameRad, std::string boneName, bool fullPath, std::string parentJointName) {

	if (_initiated == false) {
		this->init();
	}
	//Input test 
	std::string newChildrenBoneName = newChildrenBoneNameRad + boneName;
	if (boneList->find(boneName) == boneList->end())
		return false;
	if (parentJointName != std::string("") && nodeList->find(parentJointName) == nodeList->end())
		return false;
	if (this->checkBoneName(newChildrenBoneName) == false)
		return false;
	if (parentMutedBoneName != std::string("") && mutedBoneList.find(parentMutedBoneName) == mutedBoneList.end())
			return false;
	//copy of the normal bone in the muted structure
	mutedBoneList[newChildrenBoneName] = boneList->at(boneName);
	mutedBoneList[newChildrenBoneName].boneChildrenIt.clear();
	mutedBoneList[newChildrenBoneName].preTransl << 0.0 << 0.0 << 0.0 << arma::endr;
	//std::string parentMutedBoneName = mutedBoneList.getParentBoneName(nodeList->at(parentJointName));
	//
	if (parentJointName == std::string("") && parentJointName == std::string(""))
		mutedBoneList[newChildrenBoneName].jointParent = boneList->at(boneName).jointParent;
	if (parentJointName != std::string(""))
		mutedBoneList[newChildrenBoneName].jointParent = nodeList->at(parentJointName);

	//launch the bone copy for the next bone
	if (fullPath&&boneList->at(boneName).boneChildrenIt.size()>0) {
		for (std::vector<boneMapType::iterator>::iterator it = boneList->at(boneName).boneChildrenIt.begin(); it != boneList->at(boneName).boneChildrenIt.end(); it++) {
			this->pushBone(newChildrenBoneName, newChildrenBoneNameRad, (*it)->first, fullPath, nodeList->name((*it)->second.jointParent));
		}
	}

	if (parentMutedBoneName == std::string("")) {
		mutedBoneList.rootIt.push_back(mutedBoneList.find(newChildrenBoneName));
	}
	else {
		mutedBoneList[parentMutedBoneName].boneChildrenIt.push_back(mutedBoneList.find(newChildrenBoneName));
		if (parentJointName == std::string("")&& mutedBoneList[parentMutedBoneName].jointChildren.size()>0)
			mutedBoneList[newChildrenBoneName].jointParent = mutedBoneList[parentMutedBoneName].jointChildren[0];
	}
	/*for (std::vector<int>::iterator it = boneList->at(boneName).jointChildren.begin(); it != boneList->at(boneName).jointChildren.end(); it++) {

		mutedBoneList[newChildrenBoneName].jointChildren.push_back((*it));
	}*/

	return true;
}

bool ofMutedSkeleton::checkBoneName(std::string boneName) {

	return (mutedBoneList.find(boneName) == mutedBoneList.end());

}

void ofMutedSkeleton::draw(unsigned int index) {
	if (_initiated == false) {
		this->init();
	}
	if (index < track.nOfFrames() && track.hasNodeList && track.hasBoneList) {
		if (track.hasRotation) {
			const arma::cube & lRotation = track.rotation.getData();// .slice(index);
			const arma::cube & lPosition = track.position.getData();// .slice(index);

			for (int i = 0; i < mutedBoneList.rootIt.size(); i++)
				/*if (track.hasGlobalCoordinate)
				{

					boneGlobalDraw(lRotation, lPosition, boneList->rootIt[i]);

				}
				else {*/
				boneLocalDraw(index, lRotation, lPosition, mutedBoneList.rootIt[i]);

				//}
		}
		else {
			const arma::cube & lPosition = track.position.getData();// .slice(index);
			const arma::cube lRotation = arma::zeros(4, track.boneList->size(), lPosition.n_slices);

			for (int i = 0; i < mutedBoneList.rootIt.size(); i++)
				/*if (track.hasGlobalCoordinate)
				{

					boneGlobalDraw(lRotation, lPosition, boneList->rootIt[i]);

				}
				else {*/
				boneLocalDraw(index,lRotation, lPosition, mutedBoneList.rootIt[i]);

				//}
		}
	}
}

void ofMutedSkeleton::boneLocalDraw(int index,const arma::cube &rotation, const arma::cube &position, boneMapType::iterator it) {

	ofPushMatrix();
	int parent = it->second.jointParent;
	int boneId = it->second.boneId;
	int lIndex = index + it->second.frameDelay;
	if (lIndex < 0)
		lIndex = 0;
	if (lIndex >= track.nOfFrames())
		lIndex = track.nOfFrames() - 1;
	arma::vec transValue = position.slice(lIndex).col(parent);
	ofTranslate(transValue(0), transValue(1), transValue(2));
	
	// we apply the preTrans and the PreRot betwween the parent node translation and the bone rotation
	arma::vec3 lPreTrans = it->second.preTransl;
	ofTranslate(lPreTrans(0), lPreTrans(1), lPreTrans(2));
	MoMa::quaternion lpreQuat=it->second.preRot;
	if (arma::norm((arma::colvec) lpreQuat) > arma::datum::eps) {
		double alpha, x, y, z;
		lpreQuat.getRotate(alpha, x, y, z);
		ofRotate(alpha, x, y, z);
	}
	arma::vec3 lPreScale = it->second.preScale;
	ofScale(lPreScale(0), lPreScale(1), lPreScale(2));

	MoMa::quaternion lquat(rotation.slice(lIndex).col(boneId));
	if (arma::norm((arma::colvec) lquat) > arma::datum::eps) {
		double alpha, x, y, z;
		lquat.getRotate(alpha, x, y, z);
		ofRotate(alpha, x, y, z);
		for (int bEnd = 0; bEnd < it->second.jointChildren.size(); bEnd++) {
			if (it->second.visible)
				bones[it->second.jointChildren[bEnd]].draw();
		}
	}
	else {
		ofVec3f beg(0, 0, 0);
		for (int bEnd = 0; bEnd < it->second.jointChildren.size(); bEnd++) {
			ofVec3f end = toVec3f(position.slice(lIndex).col(it->second.jointChildren[bEnd]));
			ofSetLineWidth(2);
			ofLine(beg, end);
			mNodeSphere.setPosition(end);
			mNodeSphere.draw();
		}
	}
	for (int bEnd = 0; bEnd < it->second.boneChildrenIt.size(); bEnd++) {

		boneLocalDraw(index,rotation, position, it->second.boneChildrenIt[bEnd]);
	}

	ofPopMatrix();

}

bool ofMutedSkeleton::setPreTrans(std::string boneName, arma::vec3 pTrans) {
	if (mutedBoneList.find(boneName) == mutedBoneList.end())
		return false;
	mutedBoneList.find(boneName)->second.preTransl = pTrans;
	return true;

}

bool ofMutedSkeleton::setPreRot(std::string boneName, double angle, arma::vec3 axis) {
	return this->setPreRot(boneName, quaternion(angle, axis));

}
bool ofMutedSkeleton::setPreRot(std::string boneName, quaternion pRot) {
	if (mutedBoneList.find(boneName) == mutedBoneList.end())
		return false;
	mutedBoneList.find(boneName)->second.preRot = pRot;
	return true;
}

bool ofMutedSkeleton::hideBone(std::string boneName) {
	if (mutedBoneList.find(boneName) == mutedBoneList.end())
		return false;
	mutedBoneList.find(boneName)->second.visible = false;
	return true;

}

bool ofMutedSkeleton::showBone(std::string boneName) {
	if (mutedBoneList.find(boneName) == mutedBoneList.end())
		return false;

	mutedBoneList.find(boneName)->second.visible = true;
	return true;

}
bool ofMutedSkeleton::setPreScale(std::string boneName, arma::vec3 pScale) {
	if (mutedBoneList.find(boneName) == mutedBoneList.end())
		return false;

	mutedBoneList.find(boneName)->second.preScale = pScale;
	return true;
}
bool ofMutedSkeleton::setFrameDelay(std::string boneName, int pDelay) {
	if (mutedBoneList.find(boneName) == mutedBoneList.end())
		return false;

	mutedBoneList.find(boneName)->second.frameDelay = pDelay;
	return true;
}
bool ofMutedSkeleton::hideAll() {
	for (BoneList::iterator it = mutedBoneList.begin(); it != mutedBoneList.end(); it++)
		it->second.visible = false;
	return true;
}
bool ofMutedSkeleton::showAll() {
	for (BoneList::iterator it = mutedBoneList.begin(); it != mutedBoneList.end(); it++)
		it->second.visible = true;
	return true;
}