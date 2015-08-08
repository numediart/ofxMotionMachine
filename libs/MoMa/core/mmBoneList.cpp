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

void MoMa::BoneList::load( string fileName ) {
    
    ifstream bonFile( fileName.c_str() );
    
    if( !bonFile.is_open() ) {
        
        cout << "BoneList: File could not be opened!" << endl;
        return; // We alert in stdout and quit if no/wrong file!
    }
    
    while ( bonFile.good() ) {
        
        string curLine;
        stringstream curStrm;
        int begIdx, endIdx;
        
        getline( bonFile, curLine ); // Grab the line
        
        if( curLine != "" && curLine != " "
        && curLine != "\t" && curLine != "\n" ) {
            
            curStrm << curLine;
            
            curStrm >> begIdx; curStrm >> endIdx;
            this->push_back( make_pair( begIdx, endIdx ) );
        }
    }
}

void MoMa::BoneList::print( void ) {
    
    for( int n=0; n<size(); n++ ) {
        
        cout << at( n ).first << " " << at( n ).second << endl;
    }
}
