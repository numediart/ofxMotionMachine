#ifndef __BvhParser__
#define __BvhParser__

#include <iostream>
#include <vector>
#include <armadillo>

#include <fstream>
#include <cmath>

#include "mmQuaternion.h"
#include "mmArmaUtils.h"

class bvhJoint{
 public:
     std::string jointName;
    float offsetX,offsetY,offsetZ;
    bool cartesianFlag;
    bool rotationFlag;
    MoMa::rotationOrder rotationOrder;
    std::vector<std::vector<float> > frame;
    std::vector<bvhJoint*> child;
    bool eosFlag;
    //float eosOffsetX,eosOffsetY,eosOffsetZ;
    
};

class bvhParser{
public:
    bvhParser();
    ~bvhParser();
    bool bvhRead(std::string fileName);
	std::vector<std::vector<float> > bvh2xyz(unsigned int frameId);
	std::vector<std::vector<float> > bvh2quat(unsigned int frameId);
	std::vector<std::vector<float> > bvh2LocalXyz(unsigned int frameId);
	std::vector<std::vector<float> > bvh2LocalQuat(unsigned int frameId);
    std::vector<std::vector<float> > getJointOffsetRotation();
    
    bool bvhWrite(std::string fileName);
    bool newRoot(std::string jointName);
    bool newJoint(std::string jointName,std::string JointParentName);
    bvhJoint* getJoint(std::string jointName);
	unsigned int getNofJoints() { return mHierarchy.size(); }
	unsigned int getNofBones() { return nbBones; }
    std::string getNodeName(unsigned int i){return (mHierarchy.size()>0)?mHierarchy[i]->jointName:std::string("");}
	std::vector<std::pair<int, std::vector<int> > > getBonesIndices();
    unsigned int mFrames;
    double mFrameRate;
protected:
    std::string mFileName;
    std::vector<bvhJoint*> mHierarchy;
    arma::mat rotMat(double angle,unsigned int axisId);
	std::vector<std::vector<float> > children2xyz(unsigned int frameId, unsigned int nodeId, arma::mat parentTransfo);
	std::vector<std::vector<float> > children2quat(unsigned int frameId, unsigned int nodeId, arma::mat parentTransfo);
	std::vector<std::vector<float> > children2LocalXyz(unsigned int frameId, unsigned int nodeId);
	std::vector<std::vector<float> > children2LocalQuat(unsigned int frameId, unsigned int nodeId);
    bool bvhJointHeaderWrite(std::fstream &pFile,bvhJoint *pJoint,int nTab);
    bool bvhJointFrameWrite(std::fstream &pFile,bvhJoint *pJoint,int pFrameIndex);
    bool bvhJointRead(std::fstream &pFile,bvhJoint *pJoint,bool eos=false,std::string JointParentName=std::string(""));
    bool bvhJointFrameRead(std::fstream &pFile,bvhJoint *pJoint);
    arma::mat axisTransfo;
	int nbBones;
};

#endif
