#include "mmBvhParser.h"
#include "BvhParser.h"

using namespace std;

namespace MoMa {
    bool BvhParser::load( string fileName, Track *track, bool hasRotation ) {
        
        vector<string> rawJoint;
        vector<int> axisIndex;
        
        NodeList *nodeList;
        nodeList = new NodeList();
        
#ifdef WIN32 // Added to manage special characters (accents for example) in file path (extended ASCII table)
        
        string tmp = fileName;
        
        int j=0;
        bool accented=false;
        
        for(int i=0;i<tmp.size();++i){
            /*if(tmp[i]<127&&tmp[i]>0){// Basic ASCII table (not safe)
             fileName[j]=tmp[i];
             ++j;
             }*/
            if(tmp[i]!=-61){// Basic ASCII table (safer)
                fileName[j]=tmp[i];
                ++j;
            }
            else{ // Manage buggy special characters
                // the special character is divided in two characters : -61 and X (between -1 and -127); to get a valid character the conversion is c=X+64
                accented=true;
                ++i; //tmp[i] was -61
                fileName[j]=tmp[i]+64;
                ++j;
            }
        }
        
        if(accented){
            fileName[j]='\0';
        }
        
#endif
        
        //cout << "--- Attempt: " << fileName << " ---" << endl;
        bvhParser parser;
        if (!(parser.bvhRead(fileName)))
            return false;
        
        //track->clear();
        unsigned int nFrames=parser.mFrames;
        unsigned int nNodes=parser.getNofJoints();
        track->frameRate=parser.mFrameRate;
        //vector<vector<float> > jointOffsetRotation;
        //jointOffsetRotation=parser.getJointOffsetRotation();
        for (int i=0;i<nFrames;i++){
            std::cout<<i<<std::endl;
            vector<vector<float> > bvhFrame=parser.bvh2xyz(i);
            vector<vector<float> > bvhRotationFrame;
            if (hasRotation){
                bvhRotationFrame=parser.bvh2quat(i);
            }
            MoMa::Frame lFrame;
            lFrame.setRotationFlag(hasRotation);
            lFrame.setSynoList(track->synoList);

            for (int j=0;j<bvhFrame.size();j++){
                MoMa::Node lNode(bvhFrame[j][0]*10,bvhFrame[j][1]*10,bvhFrame[j][2]*10);
                if (hasRotation){
                    lNode.setRotation(bvhRotationFrame[j][0],bvhRotationFrame[j][1],bvhRotationFrame[j][2],bvhRotationFrame[j][3]);
                    //std::cout<<" "<<bvhRotationFrame[j][0]<<" "<<bvhRotationFrame[j][1]<<" "<<bvhRotationFrame[j][2]<<" "<<bvhRotationFrame[j][3]<<std::endl;
                    //lNode.setOffsetRotation(jointOffsetRotation[j][0],jointOffsetRotation[j][1],jointOffsetRotation[j][2],jointOffsetRotation[j][3]);
                }
                lNode.setName(parser.getNodeName(j));
                lNode.setTime(i*parser.mFrameRate);
                lFrame.push(lNode);
            }
            track->push(lFrame);
        }
        track->nodeList=new NodeList;
        track->hasNodeList=true;
        for (unsigned int i =0;i<nNodes;i++){
            track->nodeList->push_back(parser.getNodeName(i));
        }
        
        track->boneList=new BoneList;
        std::vector<std::pair<int,int> > lBones=parser.getBonesIndices();
        track->hasBoneList=true;
        unsigned int nBones=lBones.size();
        for (unsigned int i=0;i<nBones;i++){
            track->boneList->push_back(lBones[i]);
        }
//        track->hasSynoList=false;
        track->hasRotation=hasRotation;
        return true;
    }
}
