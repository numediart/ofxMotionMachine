#include "mmBvhParser.h"
#include "BvhParser.h"

using namespace std;

namespace MoMa {
    bool BvhParser::load( string fileName, Track *track, bool hasRotation, bool globalCoordinate ) {
        
        vector<string> rawJoint;
        vector<int> axisIndex;
        
        std::shared_ptr<NodeList> nodeList;
        nodeList = std::make_shared<NodeList>();
        
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
			unsigned int nFrames = parser.mFrames;
			unsigned int nNodes = parser.getNofJoints();
			track->setFrameRate(parser.mFrameRate);
			//vector<vector<float> > jointOffsetRotation;
			//jointOffsetRotation=parser.getJointOffsetRotation();
			arma::cube positionData(3, nNodes, nFrames);
			for (int i = 0; i < nFrames; i++) {
				if (i % 100 == 0)
					std::cout << i << std::endl;
				vector<vector<float> > bvhFrame;
		
				if (globalCoordinate) {

					bvhFrame = parser.bvh2xyz(i);
				}
				else{
					bvhFrame = parser.bvh2LocalXyz(i);
				}

				//MoMa::Frame lFrame;
				//lFrame.setRotationFlag(hasRotation);
				//lFrame.setSynoList(track->synoList);

				for (int j = 0; j < bvhFrame.size(); j++) {
					//MoMa::Node lNode(bvhFrame[j][0]*10,bvhFrame[j][1]*10,bvhFrame[j][2]*10);
					positionData(0, j, i) = bvhFrame[j][0] * 10;
					positionData(1, j, i) = bvhFrame[j][1] * 10;
					positionData(2, j, i) = bvhFrame[j][2] * 10;
				}

				//track->push(lFrame);


				track->position.setData(parser.mFrameRate, positionData);
				track->setFrameRate(parser.mFrameRate);

			}
			if (hasRotation) {
				arma::cube rotationData(4, parser.getNofBones(), nFrames);
				for (int i = 0; i < nFrames; i++) {
					if (i % 100 == 0)
						std::cout << i << std::endl;
					vector<vector<float> > bvhFrame;
					vector<vector<float> > bvhRotationFrame;

					if (globalCoordinate) {
						bvhRotationFrame = parser.bvh2quat(i);
					}
					else {
						bvhRotationFrame = parser.bvh2LocalQuat(i);
					}

						for (int j = 0; j < bvhRotationFrame.size(); j++) {
							rotationData(0, j, i) = bvhRotationFrame[j][0];
							rotationData(1, j, i) = bvhRotationFrame[j][1];
							rotationData(2, j, i) = bvhRotationFrame[j][2];
							rotationData(3, j, i) = bvhRotationFrame[j][3];
						}

						track->rotation.setData(parser.mFrameRate, rotationData);
					

					track->setFrameRate(parser.mFrameRate);

				}

			}
			track->hasRotation = hasRotation;
			track->hasGlobalCoordinate = globalCoordinate;
			

			track->nodeList = std::make_shared<NodeList>();
			track->hasNodeList = true;

			for (unsigned int i = 0; i < nNodes; i++) {

				// track->nodeList->push_back(parser.getNodeName(i));
				track->nodeList->insert(make_pair(parser.getNodeName(i), i));
			}
			if (track->boneList){
				//delete track->boneList;
				track->boneList = NULL;
			}

			track->boneList = std::make_shared<BoneList>();
			std::vector<std::pair<int, std::vector<int> > > lBones = parser.getBonesIndices();
			track->hasBoneList = true;
			unsigned int nBones = lBones.size();
			for (unsigned int i = 0; i < nBones; i++) {
				track->boneList->emplace(parser.getNodeName(lBones[i].first), BoneData(i, lBones[i].first, lBones[i].second));//By convention for BVH, bone name is the name of the origin node 
			}
			//        track->hasSynoList=false;
//        track->hasOrigNodeRot_as_boneRot=true;
        return true;
    }
}
