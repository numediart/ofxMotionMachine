/**
 *
 *  @file mmBoneList.cpp
 *  @brief MotionMachine source file for BoneList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmMutatedBoneList.h"

using namespace std;

MoMa::MutatedBoneList::MutatedBoneList( string fileName ) {
    
    load( fileName );
}

bool MoMa::MutatedBoneList::load( string fileName ) {
    
    ifstream bonFile( fileName.c_str() );
    
    if( !bonFile.is_open() ) {
        
        cout << "mutatedBoneList: File could not be opened!" << endl;
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
			MutatedBoneData lBone(boneId, begIdx, lVec);
            (*this)[boneName]= lBone;
			boneId++;
        }
    }

    return true;
}

void MoMa::MutatedBoneList::print( void ) {
	for (MutatedBoneMapType::iterator it = this->begin(); it != this->end(); it++) {
		cout << it->first << " ";
		cout << it->second.jointParent << " ";
		for (int m = 0; m <it->second.jointChildren.size(); m++)
			cout << it->second.jointChildren[m] << " ";
		std::cout << endl;

	}
}

void MoMa::MutatedBoneList::updateBoneChildrenName() {
	for (MutatedBoneMapType::iterator it = this->begin(); it != this->end(); it++) {
		it->second.boneChildrenIt.clear();
		for (int i = 0; i < it->second.jointChildren.size(); i++) {
			int nodeId = it->second.jointChildren[i];
			for (MutatedBoneMapType::iterator it2 = this->begin(); it2 != this->end(); it2++) {
				if (it2->second.jointParent == nodeId) {
					it->second.boneChildrenIt.push_back(it2);
				}
			}
		}
	}
    rootIt.clear();
	for (MutatedBoneMapType::iterator it = this->begin(); it != this->end(); it++) {
		bool rootFlag = true;
		for (MutatedBoneMapType::iterator it2 = this->begin(); it2 != this->end(); it2++) {
			for (int i = 0; i < it2->second.boneChildrenIt.size(); i++) {
				if (it2->second.boneChildrenIt[i] == it)
					rootFlag = false;
			}
		}
		if (rootFlag)
			rootIt.push_back( it );
	}
}
std::string MoMa::MutatedBoneList::getParentBoneName(int NodeId) {
	for (MutatedBoneMapType::iterator it = this->begin(); it != this->end(); it++) {
		for (std::vector<int>::iterator it2 = it->second.jointChildren.begin(); it2 != it->second.jointChildren.end(); it2++)
			if ((*it2) == NodeId)
				return (it->first);
	}
	return ("");
}