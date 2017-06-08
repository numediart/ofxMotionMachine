/**
 *
 *  @file mmBoneList.cpp
 *  @brief MotionMachine source file for BoneList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmBoneList.h"

using namespace std;

MoMa::BoneList::BoneList( string fileName ) {
    
    load( fileName );
}

bool MoMa::BoneList::load( string fileName ) {
    
    ifstream bonFile( fileName.c_str() );
    
    if( !bonFile.is_open() ) {
        
        cout << "BoneList: File could not be opened!" << endl;
        return false; // We alert in stdout and quit if no/wrong file!
    }
	int boneId = 0;
    while ( bonFile.good() ) {
        
        string curLine;
		string boneName;
		stringstream curStrm;
        int begIdx, endIdx;
        
        getline( bonFile, curLine ); // Grab the line
        
        if( curLine != "" && curLine != " "
        && curLine != "\t" && curLine != "\n" ) {
            
            curStrm << curLine;
			curStrm >> boneName;
            curStrm >> begIdx; curStrm >> endIdx;
			std::vector<int> lVec;
			lVec.push_back(endIdx);
			BoneData lBone(boneId, begIdx, lVec);
            (*this)[boneName]= lBone;
			boneId++;
        }
    }

    return true;
}

void MoMa::BoneList::print( void ) {
	for (boneMapType::iterator it = this->begin(); it != this->end(); it++) {
		cout << it->first << " ";
		cout << it->second.jointParent << " ";
		for (int m = 0; m <it->second.jointChildren.size(); m++)
			cout << it->second.jointChildren[m] << " ";
		std::cout << endl;

	}
}

void MoMa::BoneList::updateBoneChildrenName() {
	for (boneMapType::iterator it = this->begin(); it != this->end(); it++) {
		it->second.boneChildrenIt.clear();
		for (int i = 0; i < it->second.jointChildren.size(); i++) {
			int nodeId = it->second.jointChildren[i];
			for (boneMapType::iterator it2 = this->begin(); it2 != this->end(); it2++) {
				if (it2->second.jointParent == nodeId) {
					it->second.boneChildrenIt.push_back(it2);
				}
			}
		}
	}
    rootIt.clear();
	for (boneMapType::iterator it = this->begin(); it != this->end(); it++) {
		bool rootFlag = true;
		for (boneMapType::iterator it2 = this->begin(); it2 != this->end(); it2++) {
			for (int i = 0; i < it2->second.boneChildrenIt.size(); i++) {
				if (it2->second.boneChildrenIt[i] == it)
					rootFlag = false;
			}
		}
		if (rootFlag)
			rootIt.push_back( it );
	}
}
std::string MoMa::BoneList::getParentBoneName(int NodeId) {
	for (boneMapType::iterator it = this->begin(); it != this->end(); it++) {
		for (std::vector<int>::iterator it2 = it->second.jointChildren.begin(); it2 != it->second.jointChildren.end(); it2++)
			if ((*it2) == NodeId)
				return (it->first);
	}
	return ("");
}