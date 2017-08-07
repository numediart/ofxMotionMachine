#include "mmOfMutatedSkeleton.h"


using namespace MoMa;
ofMutatedSkeleton::ofMutatedSkeleton(const MoMa::Track &pTrack) : ofSkeleton(pTrack)
{
	if (track.hasGlobalCoordinate) {
		std::cerr << "Error ofMutatedSkeleton::ofMutatedSkeleton the linked track must have a local system";
	}
	trackContainer.push_back(&pTrack);
}

bool ofMutatedSkeleton::addNewTrack(const MoMa::Track &pTrack) {
	if (this->track.nOfNodes() != pTrack.nOfNodes() || this->track.nOfBones() != pTrack.nOfBones()) {
		std::cerr << "Error ofMutatedSkeleton::addNewTrack this track has not the good skeleton format";
		return false;
	}
	/*if (this->track.frameRate() != pTrack.frameRate() ) {
		std::cerr << "Error ofMutatedSkeleton::addNewTrack this track has not the good framerate";
		return false;
	}*/
	trackContainer.push_back(&pTrack);
	return true;
}

ofMutatedSkeleton::~ofMutatedSkeleton()
{
}

/*void ofMutatedSkeleton::init() {
	ofSkeleton::init();

	this->_initiated == true;
}*/

void ofMutatedSkeleton::displayNameListBone() const {

	for (MutatedBoneList::const_iterator it = mutatedBoneList.begin(); it != mutatedBoneList.end(); it++) {
		std::cout << it->first << std::endl;
	}
	
}

bool ofMutatedSkeleton::pushBone(std::string  parentMutatedBoneName, std::string newChildrenBoneNameRad, std::string boneName, bool fullPath, int trackId, std::string parentJointName) {

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
	if (parentMutatedBoneName != std::string("") && mutatedBoneList.find(parentMutatedBoneName) == mutatedBoneList.end())
			return false;
	//copy of the normal bone in the mutated structure
	mutatedBoneList[newChildrenBoneName] = boneList->at(boneName);
	mutatedBoneList[newChildrenBoneName].boneChildrenIt.clear();
	mutatedBoneList[newChildrenBoneName].preTransl << 0.0 << 0.0 << 0.0 << arma::endr;
	mutatedBoneList[newChildrenBoneName].trackIndex = trackId;
	//std::string parentMutatedBoneName = mutatedBoneList.getParentBoneName(nodeList->at(parentJointName));
	//
	if (parentJointName == std::string("") && parentJointName == std::string(""))
		mutatedBoneList[newChildrenBoneName].jointParent = boneList->at(boneName).jointParent;
	if (parentJointName != std::string(""))
		mutatedBoneList[newChildrenBoneName].jointParent = nodeList->at(parentJointName);

	//launch the bone copy for the next bone
	if (fullPath&&boneList->at(boneName).boneChildrenIt.size()>0) {
		for (std::vector<boneMapType::iterator>::iterator it = boneList->at(boneName).boneChildrenIt.begin(); it != boneList->at(boneName).boneChildrenIt.end(); it++) {
			this->pushBone(newChildrenBoneName, newChildrenBoneNameRad, (*it)->first, fullPath, trackId,nodeList->name((*it)->second.jointParent));
		}
	}

	if (parentMutatedBoneName == std::string("")) {
		mutatedBoneList.rootIt.push_back(mutatedBoneList.find(newChildrenBoneName));
	}
	else {
		mutatedBoneList[parentMutatedBoneName].boneChildrenIt.push_back(mutatedBoneList.find(newChildrenBoneName));
		if (parentJointName == std::string("")&& mutatedBoneList[parentMutatedBoneName].jointChildren.size()>0)
			mutatedBoneList[newChildrenBoneName].jointParent = mutatedBoneList[parentMutatedBoneName].jointChildren[0];
	}
	/*for (std::vector<int>::iterator it = boneList->at(boneName).jointChildren.begin(); it != boneList->at(boneName).jointChildren.end(); it++) {

		mutatedBoneList[newChildrenBoneName].jointChildren.push_back((*it));
	}*/

	return true;
}

bool ofMutatedSkeleton::checkBoneName(std::string boneName) {

	return (mutatedBoneList.find(boneName) == mutatedBoneList.end());

}

void ofMutatedSkeleton::draw(unsigned int index) {
	if (_initiated == false) {
		this->init();
	}
	if (index < track.nOfFrames() && track.hasNodeList && track.hasBoneList) {
		if (track.hasRotation) {
			//const arma::cube & lRotation = track.rotation.getData();// .slice(index);
			//const arma::cube & lPosition = track.position.getData();// .slice(index);

			for (int i = 0; i < mutatedBoneList.rootIt.size(); i++)
				/*if (track.hasGlobalCoordinate)
				{

					boneGlobalDraw(lRotation, lPosition, boneList->rootIt[i]);

				}
				else {*/
				boneLocalDraw(index, mutatedBoneList.rootIt[i]);

				//}
		}
		else {
			//const arma::cube & lPosition = track.position.getData();// .slice(index);
			//const arma::cube lRotation = arma::zeros(4, track.boneList->size(), lPosition.n_slices);

			for (int i = 0; i < mutatedBoneList.rootIt.size(); i++)
				/*if (track.hasGlobalCoordinate)
				{

					boneGlobalDraw(lRotation, lPosition, boneList->rootIt[i]);

				}
				else {*/
				boneLocalDraw(index, mutatedBoneList.rootIt[i]);

				//}
		}
	}
}

void ofMutatedSkeleton::boneLocalDraw(int index, MutatedBoneMapType::iterator it) {

	
	ofPushMatrix();
	int parent = it->second.jointParent;
	int boneId = it->second.boneId;
	int lIndex = index + it->second.frameDelay;
	if (lIndex < 0)
		lIndex = 0;
	if (lIndex >= track.nOfFrames())
		lIndex = track.nOfFrames() - 1;
	arma::vec transValue = trackContainer[it->second.trackIndex]->position.getData().slice(lIndex).col(parent);
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

	MoMa::quaternion lquat(trackContainer[it->second.trackIndex]->rotation.getData().slice(lIndex).col(boneId));
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
			ofVec3f end = toVec3f(trackContainer[it->second.trackIndex]->position.getData().slice(lIndex).col(it->second.jointChildren[bEnd]));
			ofSetLineWidth(2);
			ofLine(beg, end);
			mNodeSphere.setPosition(end);
			mNodeSphere.draw();
		}
	}
	for (int bEnd = 0; bEnd < it->second.boneChildrenIt.size(); bEnd++) {

		boneLocalDraw(index, it->second.boneChildrenIt[bEnd]);
	}

	ofPopMatrix();

}

bool ofMutatedSkeleton::setPreTrans(std::string boneName, arma::vec3 pTrans) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;
	mutatedBoneList.find(boneName)->second.preTransl = pTrans;
	return true;

}

bool ofMutatedSkeleton::setPreRot(std::string boneName, double angle, arma::vec3 axis) {
	return this->setPreRot(boneName, quaternion(angle, axis));

}
bool ofMutatedSkeleton::setPreRot(std::string boneName, quaternion pRot) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;
	mutatedBoneList.find(boneName)->second.preRot = pRot;
	return true;
}

bool ofMutatedSkeleton::hideBone(std::string boneName) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;
	mutatedBoneList.find(boneName)->second.visible = false;
	return true;

}

bool ofMutatedSkeleton::showBone(std::string boneName) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;

	mutatedBoneList.find(boneName)->second.visible = true;
	return true;

}
bool ofMutatedSkeleton::setPreScale(std::string boneName, arma::vec3 pScale) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;

	mutatedBoneList.find(boneName)->second.preScale = pScale;
	return true;
}
bool ofMutatedSkeleton::setFrameDelay(std::string boneName, int pDelay) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;

	mutatedBoneList.find(boneName)->second.frameDelay = pDelay;
	return true;
}
bool ofMutatedSkeleton::setTrackIndex(std::string boneName, int pTrackIndex) {
	if (mutatedBoneList.find(boneName) == mutatedBoneList.end())
		return false;
	if (pTrackIndex >= this->nOfTrack())
		return false;
	mutatedBoneList.find(boneName)->second.trackIndex = pTrackIndex;


	return true;
}
bool ofMutatedSkeleton::hideAll() {
	for (MutatedBoneList::iterator it = mutatedBoneList.begin(); it != mutatedBoneList.end(); it++)
		it->second.visible = false;
	return true;
}
bool ofMutatedSkeleton::showAll() {
	for (MutatedBoneList::iterator it = mutatedBoneList.begin(); it != mutatedBoneList.end(); it++)
		it->second.visible = true;
	return true;
}

bool ofMutatedSkeleton::save(std::string fileName) {

	TiXmlDocument lArchiver;
	TiXmlElement* lRoot;

	lArchiver.LinkEndChild(new TiXmlDeclaration("1.0", "UTF-8", ""));
	lRoot = new TiXmlElement("MutatedBone");
	lArchiver.LinkEndChild(lRoot);


	TiXmlElement* lTracks = new TiXmlElement("tracks");
	lRoot->LinkEndChild(lTracks);

	for (int i = 0; i < this->trackContainer.size(); i++){
		TiXmlElement* lTrack = new TiXmlElement("track");
		lTracks->LinkEndChild(lTrack);
		lTrack->SetAttribute("name", this->trackContainer[i]->fileName);
		lTrack->SetAttribute("path", this->trackContainer[i]->pathfileName);
	}
	


	TiXmlElement* lBones = new TiXmlElement("MutatedBones");
	lRoot->LinkEndChild(lBones);

	for (int i = 0; i < this->mutatedBoneList.rootIt.size(); i++) {
		addBoneToSave(lBones, this->mutatedBoneList.rootIt[i]);
		TiXmlElement* lBone = new TiXmlElement("Bone");
		lBones->LinkEndChild(lBone);
	}


	lArchiver.SaveFile(fileName);
	return true;
}
bool ofMutatedSkeleton::addBoneToSave(TiXmlElement* lParent, MutatedBoneMapType::iterator bone) {

	TiXmlElement* lBone = new TiXmlElement("Bone");
	lParent->LinkEndChild(lBone);
	lBone->SetAttribute("name", bone->first);
	lBone->SetAttribute("boneId", bone->second.boneId);
	lBone->SetAttribute("trackId", bone->second.trackIndex);
	TiXmlElement* lKeys = new TiXmlElement("Keys");
	lBone->LinkEndChild(lKeys);
	TiXmlElement* lKey = new TiXmlElement("Key");
	lKeys->LinkEndChild(lKey);
	lKey->SetAttribute("frameId", 0);
	TiXmlElement* lProp = new TiXmlElement("frameDelay");
	lKey->LinkEndChild(lProp);
	lProp->InsertEndChild(TiXmlText(std::to_string(bone->second.frameDelay)));
	lProp = new TiXmlElement("shown");
	lKey->LinkEndChild(lProp);
	lProp->InsertEndChild(TiXmlText(std::to_string(bone->second.visible)));

	TiXmlElement* lScale = new TiXmlElement("Scale");
	lKey->LinkEndChild(lScale);
	for (int i = 0; i < 3; i++) {
		lScale->InsertEndChild(TiXmlText(std::to_string(bone->second.preScale(i))));
		if (i < 2)
			lScale->InsertEndChild(TiXmlText(" "));
	}
	TiXmlElement* lTransl = new TiXmlElement("Translation");
	lKey->LinkEndChild(lTransl);
	for (int i = 0; i < 3; i++) {
		lTransl->InsertEndChild(TiXmlText(std::to_string(bone->second.preTransl(i))));
		if (i < 2)
			lTransl->InsertEndChild(TiXmlText(" "));
	}
	TiXmlElement* lRot = new TiXmlElement("Rotation");
	lKey->LinkEndChild(lRot);
	for (int i = 0; i < 4; i++) {
		lRot->InsertEndChild(TiXmlText(std::to_string(bone->second.preRot(i))));
		if (i < 3)
			lRot->InsertEndChild(TiXmlText(" "));
	}

	TiXmlElement* lChildren = new TiXmlElement("Children");
	lBone->LinkEndChild(lChildren);
	for (int i = 0; i < bone->second.boneChildrenIt.size(); i++)
		ofMutatedSkeleton::addBoneToSave(lChildren, bone->second.boneChildrenIt[i]);
	return true;
}

bool ofMutatedSkeleton::load(std::string fileName, std::vector<MoMa::Track> &Tracks, ofMutatedSkeletonPtr &mySkel) {
	TiXmlDocument lArchiver;
	TiXmlElement* lRoot;
	lArchiver.Clear();
	if (!lArchiver.LoadFile(fileName))
		throw std::runtime_error("XmlArchiver::load failed to open this file");
	lRoot = lArchiver.FirstChildElement("MoMaArchive");
	if (lRoot == 0)
		throw std::runtime_error("XmlArchiver::load : This file doesn't contain a MoMa archive");
	return true;
}