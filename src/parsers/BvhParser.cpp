#include "BvhParser.h"

using namespace MoMa;
bvhParser::bvhParser(){
    axisTransfo
    <<   0<<   0<<   1<<   0<<arma::endr
    <<   1<<   0<<   0<<   0<<arma::endr
    <<   0<<   1<<   0<<   0<<arma::endr
    <<   0<<   0<<   0<<   1;

	nbBones = 0;
    
};
bvhParser::~bvhParser(){
    for (int i=0;i<mHierarchy.size();i++){
        delete mHierarchy[i];
    }
    mHierarchy.clear();
}

bool bvhParser::bvhJointRead(std::fstream &pFile,bvhJoint *pJoint,bool eos,std::string JointParentName){
    std::string bufString;
    if (!eos)
        pFile>>pJoint->jointName;
    else {
        pFile>>bufString;
        pJoint->jointName=JointParentName+std::string("EndOfSite");
    }
    pFile>>bufString;
    pFile>>bufString;
    if (bufString!=std::string("OFFSET")){
        return false;
    }
    pFile>>pJoint->offsetX;
    pFile>>pJoint->offsetY;
    pFile>>pJoint->offsetZ;
    if (eos){
        pJoint->eosFlag=true;
        pJoint->cartesianFlag=false;
        pJoint->rotationFlag=false;
        pFile>>bufString;
        return true;
    }
    else
        pJoint->eosFlag=false;
    pFile>>bufString;
    if (bufString!=std::string("CHANNELS"))
        return false;
        int nbChan;
    pFile>>nbChan;
    pJoint->cartesianFlag=false;
    pJoint->rotationFlag=false;
    std::string order("XYZ");
    int nbRot=0;
    for (int i=0;i<nbChan;i++){
        pFile>>bufString;
		if (bufString.find("position") < bufString.length()) {
			pJoint->cartesianFlag = true;
		}
        if (bufString.find("rotation")<bufString.length()){
			nbBones++;
            pJoint->rotationFlag=true;
			if (nbRot>=3)
				return false;
			order[nbRot]=bufString[0];
			nbRot++;
		}
    }
    //In bvh file they give the axis order for the multiplication order. It's not the rotation order that we use in MoMa 
    if (order=="XYZ")
        pJoint->rotationOrder=EULER_ZYX;
    else
        if (order=="XZY")
            pJoint->rotationOrder=EULER_YZX;
        else
            if (order=="YXZ")
                pJoint->rotationOrder=EULER_ZXY;
            else
                if (order=="YZX")
                    pJoint->rotationOrder=EULER_XZY;
                else
                    if (order=="ZXY")
                        pJoint->rotationOrder=EULER_YXZ;
                    else
                        if (order=="ZYX")
                            pJoint->rotationOrder=EULER_XYZ;
    
    pFile>>bufString;
    
    if (bufString=="End"){
        bvhJoint *lJoint= new bvhJoint;
        pJoint->child.push_back(lJoint);
        mHierarchy.push_back(lJoint);
        if (this->bvhJointRead(pFile, lJoint,true,pJoint->jointName)==false)
            return false;
        pFile>>bufString;
        
    }
    else{
        while (bufString=="JOINT"){
            bvhJoint *lJoint= new bvhJoint;
            pJoint->child.push_back(lJoint);
            mHierarchy.push_back(lJoint);
            if (this->bvhJointRead(pFile, lJoint)==false)
                return false;
            pFile>>bufString;
        }
        
    }
    // get } that ends a joint
    return true;
}

bool bvhParser::bvhJointFrameRead(std::fstream &pFile,bvhJoint *pJoint){
    int nbChan=0;
    if (pJoint->cartesianFlag)
        nbChan+=3;
    if (pJoint->rotationFlag)
        nbChan+=3;
    std::vector<float> chanData;
    for (int n=0;n<nbChan;n++){
        float data;
        pFile>>data;
        chanData.push_back(data);
    }
    pJoint->frame.push_back(chanData);
    for (int c=0;c<pJoint->child.size();c++){
        if (this->bvhJointFrameRead(pFile, pJoint->child[c])==false)
            return false;
    }
    return true;
    
}

bool bvhParser::bvhRead(std::string fileName){
    if (mHierarchy.size()!=0){
        
        for (int i=0;i<mHierarchy.size();i++){
            delete mHierarchy[i];
        }
        mHierarchy.clear();
    }
    std::fstream lFile(fileName.c_str(), std::fstream::in);
    if (!lFile.is_open())
        return false;
    lFile.precision(6);
    //Read the Header
    std::string bufString;
    lFile>>bufString;
    lFile>>bufString;
    if (bufString==std::string("ROOT")){
        mHierarchy.push_back(new bvhJoint);
        if (this->bvhJointRead(lFile, mHierarchy[0])==false)
            return false;
    }
    //Read the frames
	nbBones /= 3;
    lFile>>bufString;
    if (bufString!=std::string("MOTION")){
        return false;
    }
    lFile>>bufString;
    if (bufString!=std::string("Frames:")){
        return false;
    }
    lFile>>this->mFrames;
    lFile>>bufString;
    if (bufString!=std::string("Frame")){
        return false;
    }
    lFile>>bufString;
    if (bufString!=std::string("Time:")){
        return false;
    }
    float sampleTime;
    lFile>>sampleTime;
    this->mFrameRate=1.f/sampleTime;
    
    
    for (int i=0;i<mFrames;i++){
        if (this->bvhJointFrameRead(lFile, mHierarchy[0])==false)
            return false;
    }
    return true;
    
}



std::vector<std::vector<float> > bvhParser::children2xyz(unsigned int frameId,unsigned int nodeId, arma::mat parentTransfo){
    std::vector<std::vector<float> > ret;
    arma::mat lTransfo;
    lTransfo.eye(4,4);
    if (mHierarchy[nodeId]->rotationFlag){
        if (mHierarchy[nodeId]->cartesianFlag&&(mHierarchy[nodeId]->frame[frameId].size()==6))
            lTransfo=coordMat(mHierarchy[nodeId]->frame[frameId][5],mHierarchy[nodeId]->frame[frameId][4],mHierarchy[nodeId]->frame[frameId][3],mHierarchy[nodeId]->rotationOrder);
        
        else
            if (mHierarchy[nodeId]->frame[frameId].size()==3)
                    lTransfo=coordMat(mHierarchy[nodeId]->frame[frameId][2],mHierarchy[nodeId]->frame[frameId][1],mHierarchy[nodeId]->frame[frameId][0],mHierarchy[nodeId]->rotationOrder);
        
    }
    arma::colvec offset;
    offset<<mHierarchy[nodeId]->offsetX<<mHierarchy[nodeId]->offsetY<<mHierarchy[nodeId]->offsetZ<<1.0;
    
    lTransfo.col(3)=offset;
    lTransfo=parentTransfo*lTransfo;
    
    if (mHierarchy[nodeId]->cartesianFlag){
        offset<<mHierarchy[nodeId]->frame[frameId][0]<<mHierarchy[nodeId]->frame[frameId][1]<<mHierarchy[nodeId]->frame[frameId][2]<<1.0;
        lTransfo.col(3)=offset;
    }
    std::vector<float> val;
    arma::mat lTransfo3;
    lTransfo3=axisTransfo*lTransfo*axisTransfo.t();
    //val.push_back(-lTransfo(2,3));
    //val.push_back(-lTransfo(0,3));
    //val.push_back(lTransfo(1,3));
    val.push_back(lTransfo3(0,3));
    val.push_back(lTransfo3(1,3));
    val.push_back(lTransfo3(2,3));
    
    ret.push_back(val);
    for (int i=0;i<mHierarchy[nodeId]->child.size();i++){
        int k=0;
        while (mHierarchy[nodeId]->child[i]!=mHierarchy[k]){
            k++;
        }
        if (mHierarchy[nodeId]->child[i]==mHierarchy[k]){
            std::vector<std::vector<float> > childRet=this->children2xyz(frameId, k, lTransfo);
            for (int j=0;j<childRet.size();j++){
                ret.push_back(childRet[j]);
            }
        }
    }
    return ret;
}

std::vector<std::vector<float> > bvhParser::bvh2xyz(unsigned int frameId){
    
    arma::mat rootCoord=arma::eye(4,4);
    std::vector<std::vector<float> > ret=this->children2xyz(frameId, 0, rootCoord);
    return ret;
    
    
}
std::vector<std::vector<float> > bvhParser::children2quat(unsigned int frameId,unsigned int nodeId, arma::mat parentTransfo){
    std::vector<std::vector<float> > ret;
    arma::mat lTransfo;
    lTransfo.eye(4,4);
    if (mHierarchy[nodeId]->rotationFlag){
        if (mHierarchy[nodeId]->cartesianFlag&&(mHierarchy[nodeId]->frame[frameId].size()==6))
            lTransfo=coordMat(mHierarchy[nodeId]->frame[frameId][5],mHierarchy[nodeId]->frame[frameId][4],mHierarchy[nodeId]->frame[frameId][3],mHierarchy[nodeId]->rotationOrder);        
        else
            if (mHierarchy[nodeId]->frame[frameId].size()==3)
                lTransfo=coordMat(mHierarchy[nodeId]->frame[frameId][2],mHierarchy[nodeId]->frame[frameId][1],mHierarchy[nodeId]->frame[frameId][0],mHierarchy[nodeId]->rotationOrder);
    }
	else {
		int j = 0;
		j++;
	}
    arma::colvec offset;
    offset<<mHierarchy[nodeId]->offsetX<<mHierarchy[nodeId]->offsetY<<mHierarchy[nodeId]->offsetZ<<1.0;
    
    lTransfo.col(3)=offset;
    lTransfo=parentTransfo*lTransfo;
    
    if (mHierarchy[nodeId]->cartesianFlag){
        offset<<mHierarchy[nodeId]->frame[frameId][0]<<mHierarchy[nodeId]->frame[frameId][1]<<mHierarchy[nodeId]->frame[frameId][2]<<1.0;
        lTransfo.col(3)=offset;
    }
    
    arma::mat lTransfo3;
    
    lTransfo3=axisTransfo*lTransfo*axisTransfo.t();
    
    double w=sqrt(arma::trace(lTransfo3))/2;
    double x=(lTransfo3(2,1)-lTransfo3(1,2))/(4*w);
    double y=(lTransfo3(0,2)-lTransfo3(2,0))/(4*w);
    double z=(lTransfo3(1,0)-lTransfo3(0,1))/(4*w);
    
    std::vector<float> val;
    val.push_back(x);
    val.push_back(y);
    val.push_back(z);
    val.push_back(w);
    ret.push_back(val);
    for (int i=0;i<mHierarchy[nodeId]->child.size();i++){
        int k=0;
        while (mHierarchy[nodeId]->child[i]!=mHierarchy[k]){
            k++;
        }
        if (mHierarchy[nodeId]->child[i]==mHierarchy[k]&& (mHierarchy[nodeId]->child[i]->rotationFlag==true)){
            std::vector<std::vector<float> > childRet=this->children2quat(frameId, k, lTransfo);
            for (int j=0;j<childRet.size();j++){
                ret.push_back(childRet[j]);
            }
        }
    }
    return ret;
}

std::vector<std::vector<float> > bvhParser::bvh2quat(unsigned int frameId){
    
    arma::mat rootCoord=arma::eye(4,4);
    std::vector<std::vector<float> > ret=this->children2quat(frameId, 0, rootCoord);
    return ret;
}

std::vector<std::vector<float> > bvhParser::bvh2LocalXyz(unsigned int frameId) {
	std::vector<std::vector<float> > ret = this->children2LocalXyz(frameId, 0);
	return ret;

}

std::vector<std::vector<float> > bvhParser::bvh2LocalQuat(unsigned int frameId) {

	arma::mat rootCoord = arma::eye(4, 4);
	std::vector<std::vector<float> > ret = this->children2LocalQuat(frameId, 0);
	return ret;

}

std::vector<std::vector<float> > bvhParser::children2LocalXyz(unsigned int frameId, unsigned int nodeId) {
	std::vector<std::vector<float> > ret;
	arma::mat lTransfo;
	lTransfo.eye(4, 4);
/*	if (mHierarchy[nodeId]->rotationFlag) {
		if (mHierarchy[nodeId]->cartesianFlag && (mHierarchy[nodeId]->frame[frameId].size() == 6))
			lTransfo = coordMat(mHierarchy[nodeId]->frame[frameId][5], mHierarchy[nodeId]->frame[frameId][4], mHierarchy[nodeId]->frame[frameId][3], mHierarchy[nodeId]->rotationOrder);

		else
			if (mHierarchy[nodeId]->frame[frameId].size() == 3)
				lTransfo = coordMat(mHierarchy[nodeId]->frame[frameId][2], mHierarchy[nodeId]->frame[frameId][1], mHierarchy[nodeId]->frame[frameId][0], mHierarchy[nodeId]->rotationOrder);

	}*/
	arma::colvec offset;
	offset << mHierarchy[nodeId]->offsetX << mHierarchy[nodeId]->offsetY << mHierarchy[nodeId]->offsetZ << 1.0;

	lTransfo.col(3) = offset;
	//lTransfo = parentTransfo*lTransfo;

	if (mHierarchy[nodeId]->cartesianFlag) {
		offset << mHierarchy[nodeId]->frame[frameId][0] << mHierarchy[nodeId]->frame[frameId][1] << mHierarchy[nodeId]->frame[frameId][2] << 1.0;
		lTransfo.col(3) = offset;
	}
	std::vector<float> val;
	arma::mat lTransfo3;
	lTransfo3 = axisTransfo*lTransfo*axisTransfo.t();
	//val.push_back(-lTransfo(2,3));
	//val.push_back(-lTransfo(0,3));
	//val.push_back(lTransfo(1,3));
	val.push_back(lTransfo3(0, 3));
	val.push_back(lTransfo3(1, 3));
	val.push_back(lTransfo3(2, 3));

	ret.push_back(val);
	for (int i = 0; i<mHierarchy[nodeId]->child.size(); i++) {
		int k = 0;
		while (mHierarchy[nodeId]->child[i] != mHierarchy[k]) {
			k++;
		}
		if (mHierarchy[nodeId]->child[i] == mHierarchy[k]) {
			std::vector<std::vector<float> > childRet = this->children2LocalXyz(frameId, k);
			for (int j = 0; j<childRet.size(); j++) {
				ret.push_back(childRet[j]);
			}
		}
	}
	return ret;
}

std::vector<std::vector<float> > bvhParser::children2LocalQuat(unsigned int frameId, unsigned int nodeId) {

	std::vector<std::vector<float> > ret;
	arma::mat lTransfo;
	lTransfo.eye(4, 4);
	if (mHierarchy[nodeId]->rotationFlag) {
		if (mHierarchy[nodeId]->cartesianFlag && (mHierarchy[nodeId]->frame[frameId].size() == 6))
			lTransfo = coordMat(mHierarchy[nodeId]->frame[frameId][5], mHierarchy[nodeId]->frame[frameId][4], mHierarchy[nodeId]->frame[frameId][3], mHierarchy[nodeId]->rotationOrder);
		else
			if (mHierarchy[nodeId]->frame[frameId].size() == 3)
				lTransfo = coordMat(mHierarchy[nodeId]->frame[frameId][2], mHierarchy[nodeId]->frame[frameId][1], mHierarchy[nodeId]->frame[frameId][0], mHierarchy[nodeId]->rotationOrder);
	}
	else {
		int j = 0;
		j++;
	}
	arma::colvec offset;
	offset << mHierarchy[nodeId]->offsetX << mHierarchy[nodeId]->offsetY << mHierarchy[nodeId]->offsetZ << 1.0;

	lTransfo.col(3) = offset;
//	lTransfo = parentTransfo*lTransfo;

	if (mHierarchy[nodeId]->cartesianFlag) {
		offset << mHierarchy[nodeId]->frame[frameId][0] << mHierarchy[nodeId]->frame[frameId][1] << mHierarchy[nodeId]->frame[frameId][2] << 1.0;
		lTransfo.col(3) = offset;
	}

	arma::mat lTransfo3;

	lTransfo3 = axisTransfo*lTransfo*axisTransfo.t();

	double w = sqrt(arma::trace(lTransfo3)) / 2;
	double x = (lTransfo3(2, 1) - lTransfo3(1, 2)) / (4 * w);
	double y = (lTransfo3(0, 2) - lTransfo3(2, 0)) / (4 * w);
	double z = (lTransfo3(1, 0) - lTransfo3(0, 1)) / (4 * w);

	std::vector<float> val;
	val.push_back(x);
	val.push_back(y);
	val.push_back(z);
	val.push_back(w);
	ret.push_back(val);
	for (int i = 0; i<mHierarchy[nodeId]->child.size(); i++) {
		int k = 0;
		while (mHierarchy[nodeId]->child[i] != mHierarchy[k]) {
			k++;
		}
		if (mHierarchy[nodeId]->child[i] == mHierarchy[k] && (mHierarchy[nodeId]->child[i]->rotationFlag == true)) {
			std::vector<std::vector<float> > childRet = this->children2LocalQuat(frameId, k);
			for (int j = 0; j<childRet.size(); j++) {
				ret.push_back(childRet[j]);
			}
		}
	}
	return ret;
}

std::vector<std::vector<float> > bvhParser::getJointOffsetRotation(){
    
    std::vector<std::vector<float> > ret;
    {
    }
    int rightIndex=-1;
    int leftIndex=-1;
    int headIndex=-1;
    int spineIndex=-1;
    std::vector<std::vector<float> > coord=this->bvh2xyz(0);
    for (int i=0;i<mHierarchy.size();i++){
        if (mHierarchy[i]->jointName.find(std::string("arm"))!=std::string::npos||mHierarchy[i]->jointName.find(std::string("Arm"))!=std::string::npos){
            if (mHierarchy[i]->jointName.find(std::string("left"))!=std::string::npos||mHierarchy[i]->jointName.find(std::string("Left"))!=std::string::npos)
                if (leftIndex==-1){
                    leftIndex=i;
                }
            if (mHierarchy[i]->jointName.find(std::string("right"))!=std::string::npos||mHierarchy[i]->jointName.find(std::string("Right"))!=std::string::npos)
                if (rightIndex==-1){
                    rightIndex=i;
                }
        }
        if (mHierarchy[i]->jointName.find(std::string("head"))!=std::string::npos||mHierarchy[i]->jointName.find(std::string("Head"))!=std::string::npos){
            if (headIndex==-1){
                headIndex=i;
            }
            
        }
        
        if (mHierarchy[i]->jointName.find(std::string("Spine"))!=std::string::npos||mHierarchy[i]->jointName.find(std::string("spine"))!=std::string::npos){
            if (spineIndex==-1){
                spineIndex=i;
            }
        }
    }
    arma::colvec frontalAxis;
    if (rightIndex==-1||leftIndex==-1){
        frontalAxis<<0<<1<<0<<0;
        
    }
    else{
        arma::colvec tempVec;
        tempVec<<coord[leftIndex][0]<<coord[leftIndex][1]<<coord[leftIndex][2];
        frontalAxis<<coord[rightIndex][0]<<coord[rightIndex][1]<<coord[rightIndex][2];
        frontalAxis=normalise(tempVec-frontalAxis);
    }
    
    arma::colvec longAxis;
    if (headIndex==-1||spineIndex==-1){
        longAxis<<0<<0<<1;
        
    }
    else{
        arma::colvec tempVec;
        tempVec<<coord[headIndex][0]<<coord[headIndex][1]<<coord[headIndex][2];
        longAxis<<coord[spineIndex][0]<<coord[spineIndex][1]<<coord[spineIndex][2];
        longAxis=normalise(tempVec-longAxis);
    }
    arma::colvec sagAxis;
    sagAxis=arma::cross(longAxis,frontalAxis);
    
    for (int i=0;i<mHierarchy.size();i++){
        
        std::cout<<mHierarchy[i]->jointName<<std::endl;
        
        std::vector<float> val;
        arma::mat offsetMatrix;
        offsetMatrix.eye(3,3);
        if (mHierarchy[i]->jointName.find(std::string("hip"))!=std::string::npos||mHierarchy[i]->jointName.find(std::string("Hip"))!=std::string::npos){
            val.push_back(0.f);
            val.push_back(0.f);
            val.push_back(0.f);
            val.push_back(1.f);
            
            ret.push_back(val);
            continue;
        }
        arma::colvec tempVecX,tempVecY,tempVecZ;
        tempVecX<<mHierarchy[i]->offsetX<<mHierarchy[i]->offsetY<<mHierarchy[i]->offsetZ;
        tempVecX=axisTransfo*arma::normalise(tempVecX);if (std::abs(arma::dot(tempVecX,sagAxis))>std::abs(arma::dot(tempVecX,longAxis))&&std::abs(arma::dot(tempVecX,sagAxis))>std::abs(arma::dot(tempVecX,frontalAxis))){
            
            tempVecZ=arma::cross(tempVecX,frontalAxis);
            tempVecY=arma::cross(tempVecZ,tempVecX);
            
        }
        else if (std::abs(arma::dot(tempVecX,frontalAxis))>std::abs(arma::dot(tempVecX,longAxis))){
            
            tempVecY=arma::cross(longAxis,tempVecX);
            tempVecZ=arma::cross(tempVecX,tempVecY);
            
        }
        else if (arma::dot(tempVecX,longAxis)>0){
            
            tempVecZ=arma::cross(frontalAxis,tempVecX);
            tempVecY=arma::cross(tempVecZ,tempVecX);
        }
        else {
            tempVecZ=arma::cross(tempVecX,frontalAxis);
            tempVecY=arma::cross(tempVecZ,tempVecX);
        }
        
        offsetMatrix.col(0)=arma::normalise( tempVecX);
        offsetMatrix.col(1)=arma::normalise( tempVecY);
        offsetMatrix.col(2)=arma::normalise( tempVecZ);
        std::cout<<mHierarchy[i]->jointName<<std::endl;
        std::cout<<offsetMatrix<<std::endl;
        MoMa::quaternion lquat;
        lquat.set(offsetMatrix);
        
        std::cout<<lquat(0)<<" "<<lquat(1)<<" "<<lquat(2)<<" "<<lquat(3);
        val.push_back(lquat(0));
        val.push_back(lquat(1));
        val.push_back(lquat(2));
        val.push_back(lquat(3));
        
        ret.push_back(val);
    }
    
    return ret;
    
};


bool bvhParser::bvhJointHeaderWrite(std::fstream &pFile,bvhJoint *pJoint,int nTab){
    std::string JName=pJoint->jointName;
    int n=JName.find(" ");
    while (n!=std::string::npos){
        JName[n]='_';
        n=JName.find(" ");
    }
    
    if (!pJoint->eosFlag){
        pFile<<JName<<std::endl;
    }
    std::string lTab("\t" );
    std::string lTabTot("");
    for (int j=0;j<nTab-1;j++)
        lTabTot+=lTab;
    pFile<<lTabTot<<"{"<<std::endl;
    lTabTot=std::string("");
    for (int j=0;j<nTab;j++)
        lTabTot+=lTab;
    pFile<<lTabTot<<"OFFSET\t"<<pJoint->offsetX<<"\t"<<pJoint->offsetY<<"\t"<<pJoint->offsetZ<<std::endl;
    int nbChan=0;
    if (pJoint->cartesianFlag)
        nbChan+=3;
    if (pJoint->rotationFlag)
        nbChan+=3;
    if (nbChan>0){
        pFile<<lTabTot<<"CHANNELS "<<nbChan<<" ";
        if (pJoint->cartesianFlag)
            pFile<<"Xposition Yposition Zposition ";
        if (pJoint->rotationFlag)
            switch (pJoint->rotationOrder) {
                case EULER_ZYX:
                    pFile<<"Xrotation Yrotation Zrotation ";
                    break;
                case EULER_YZX:
                    pFile<<"Xrotation Zrotation Yrotation ";
                    break;
                case EULER_ZXY:
                    pFile<<"Yrotation Xrotation Zrotation ";
                    break;
                case EULER_XZY:
                    pFile<<"Yrotation Zrotation Xrotation ";
                    break;
                case EULER_YXZ:
                    pFile<<"Zrotation Xrotation Yrotation ";
                    break;
                case EULER_XYZ:
                    pFile<<"Zrotation Yrotation Xrotation ";
                    break;
                default:
                    break;
            }
        pFile<<std::endl;
    }
    if (pJoint->child.size()>0)
        for (int i=0;i<pJoint->child.size();i++){
            if (pJoint->child[i]){
                if (pJoint->child[i]->eosFlag)
                    pFile<<lTabTot<<"End Site"<<std::endl;
                else
                    pFile<<lTabTot<<"JOINT ";
                if (this->bvhJointHeaderWrite(pFile, pJoint->child[i],nTab+1)==false)
                    return false;
            }
        }
    
    lTabTot=std::string("");
    for (int j=0;j<nTab-1;j++)
        lTabTot+=lTab;
    pFile<<lTabTot<<"}"<<std::endl;
    return true;
}

bool bvhParser::bvhJointFrameWrite(std::fstream &pFile,bvhJoint *pJoint,int pFrameIndex){
    if (pJoint->frame.size()<pFrameIndex)
        return false;
    for (int i=0;i<pJoint->frame[pFrameIndex].size();i++)
        pFile<<pJoint->frame[pFrameIndex][i]<<" ";
    for (int i=0;i<pJoint->child.size();i++){
        if (pJoint->child[i]){
            if (this->bvhJointFrameWrite(pFile, pJoint->child[i],pFrameIndex)==false)
                return false;
        }
    }
    
    return true;
}

bool bvhParser::bvhWrite(std::string fileName){
    //TODO
    std::fstream lFile(fileName.c_str(), std::fstream::out);
    if (!lFile.is_open()||mHierarchy.size()==0)
        return false;
    lFile.precision(6);
    lFile.setf(std::ios::fixed,std::ios::floatfield);
    lFile<<"HIERARCHY"<<std::endl;
    lFile<<"ROOT ";
    this->bvhJointHeaderWrite(lFile, mHierarchy[0],1);
    lFile<<"MOTION"<<std::endl ;
    lFile<<"Frames:\t"<<mFrames<<std::endl ;
    lFile<<"Frame Time:\t"<<mFrameRate<<std::endl;
    lFile.precision(6);
    lFile.setf(std::ios::fixed,std::ios::floatfield);
    bool flag=true;
    for (int frameIndex=0;frameIndex<mFrames;frameIndex++){
        flag=this->bvhJointFrameWrite(lFile, mHierarchy[0],frameIndex);
        if (flag==false){
            
            break;
        }
        lFile<<std::endl;
    }
    lFile.close();
    return flag;
    
}

bool bvhParser::newJoint(std::string jointName,std::string JointParentName){
    
    for (int i=0;i<mHierarchy.size();i++){
        if (mHierarchy[i]&&mHierarchy[i]->jointName==JointParentName){
            bvhJoint* localJoint=new bvhJoint;
            localJoint->jointName=jointName;
            mHierarchy.push_back(localJoint);
            mHierarchy[i]->child.push_back(localJoint);
            
            return true;
        }
    }
    return false;
}

bool bvhParser::newRoot(std::string jointName){
    
    for (int i=0;i<mHierarchy.size();i++){
        delete mHierarchy[i];
    }
    mHierarchy.clear();
    bvhJoint* localJoint=new bvhJoint;
    localJoint->jointName=jointName;
    mHierarchy.push_back(localJoint);
    return false;
}

bvhJoint* bvhParser::getJoint(std::string jointName){
    for (int i=0;i<mHierarchy.size();i++){
        if (mHierarchy[i]&&mHierarchy[i]->jointName==jointName){
            return mHierarchy[i];
        }
    }
    return 0;
}

std::vector<std::pair<int, std::vector<int> > > bvhParser::getBonesIndices(){
    std::vector<std::pair<int, std::vector<int> > > ret;
    for (unsigned int i=0;i<mHierarchy.size();i++){
        unsigned int k=i+1;
		std::vector<int> lVec;
        for (unsigned int j=0;j<mHierarchy[i]->child.size();j++){
            while (mHierarchy[i]->child[j]!=mHierarchy[k]){
                k++;
            }
			if (mHierarchy[i]->child[j] == mHierarchy[k]) 
                lVec.push_back(k);
        }
		if (lVec.size()>0)
			ret.push_back(std::pair<int, std::vector<int> >  (i, lVec));
    }
    return ret;
}
