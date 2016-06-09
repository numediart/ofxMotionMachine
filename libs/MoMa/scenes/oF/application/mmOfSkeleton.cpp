#include "mmOfSkeleton.h"
using namespace MoMa;
ofSkeleton::ofSkeleton(const MoMa::Track &pTrack) :
	track(pTrack), boneList(pTrack.boneList), nodeList(pTrack.nodeList)
{
	this->_isValid = true;
	if (track.hasNodeList == false)
		this->_isValid = false;
	if (track.hasBoneList == false)
		this->_isValid = false;
	if (track.hasRotation==false)
		this->_isValid = false;
	if (track.nOfFrames()==0)
		this->_isValid = false;
	if (this->_isValid == false)
		return;
	bones.resize(nodeList->size());
	arma::mat offset = track.getRotationOffset();
	MoMa::Frame frame = track.frame(0u);
	for (boneMapType::const_iterator it = boneList->begin(); it != boneList->end(); it++) {
		for (int bEnd = 0; bEnd < it->second.jointChildren.size(); bEnd++) {
			arma::colvec lOffRot = offset.col(it->second.jointChildren[bEnd]);
			if (lOffRot.is_finite()) {
				float s;
				if (track.hasGlobalCoordinate)
					s = arma::norm(frame.getPosition().col(it->second.jointParent) - frame.getPosition().col(it->second.jointChildren[bEnd]));
				else	
					s = arma::norm(frame.getPosition().col(it->second.jointChildren[bEnd]));
				bones[(it->second.jointChildren[bEnd])].setOrientation(toQuaternion(offset.col(it->second.jointChildren[bEnd])));//																											   //	bone.setOrientation(toQuaternion(frame.getRotationOffset().col(frame.boneList->at(b).second[bEnd])) *toQuaternion(frame.getRotation().col(frame.boneList->at(b).second[bEnd])));//
				bones[(it->second.jointChildren[bEnd])].setScale(s, 1, 1);//Rigid Skeleton
			}
		}
	}

}


ofSkeleton::~ofSkeleton()
{
}

void ofSkeleton::draw(unsigned int index){
	if (index < track.nOfFrames()) {
		const arma::mat & lRotation = track.rotation.getData().slice(index);
		const arma::mat & lPosition = track.position.getData().slice(index);

        for( int i = 0; i < boneList->rootIt.size(); i++ )
    		if (track.hasGlobalCoordinate)
	    	{
                    
    			boneGlobalDraw(lRotation, lPosition, boneList->rootIt[i]);

    		}
	    	else {
		    	boneLocalDraw(lRotation, lPosition, boneList->rootIt[i]);

		    }
	}
}

void ofSkeleton::boneGlobalDraw(const arma::mat &rotation, const arma::mat &position, boneMapType::iterator it) {

}

void ofSkeleton::boneLocalDraw(const arma::mat &rotation, const arma::mat &position, boneMapType::iterator it) {
	ofPushMatrix();
	int parent = it->second.jointParent;
	int boneId = it->second.boneId;
	arma::vec transValue = position.col(parent);
	ofTranslate(transValue(0), transValue(1), transValue(2));
	MoMa::quaternion lquat(rotation.col(boneId));
	double alpha, x, y, z;
	lquat.getRotate(alpha, x, y, z);
	ofRotate(alpha, x, y, z);
	ofVec3f beg(0, 0, 0);
	for (int bEnd = 0; bEnd < it->second.jointChildren.size(); bEnd++) {
		ofVec3f end = toVec3f(position.col(it->second.jointChildren[bEnd]));
		ofSetLineWidth(2);
		ofLine(beg, end);
		bones[it->second.jointChildren[bEnd]].draw();
	}
    for( int bEnd = 0; bEnd < it->second.boneChildrenIt.size(); bEnd++ )
        boneLocalDraw( rotation, position, it->second.boneChildrenIt[bEnd] );
	ofPopMatrix();
}
void ofSkeleton::draw(double time) {
}