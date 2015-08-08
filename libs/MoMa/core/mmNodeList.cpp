/**
 *
 *  @file mmNodeList.cpp
 *  @brief MotionMachine source file for NodeList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmNodeList.h"

using namespace std;

MoMa::NodeList::NodeList( string fileName ) {
    
    load( fileName );
}

void MoMa::NodeList::load( string fileName ) {
    
    ifstream nodFile( fileName.c_str() );
    
    if( !nodFile.is_open() ) {
        
        cout << "NameList: File could not be opened!" << endl;
        return; // We alert in stdout and quit if no/wrong file!
    }
    
    while ( nodFile.good() ) {
        
        string nodeName;
        
        getline( nodFile, nodeName ); // Grab the name from line
        
        if( nodeName != "" && nodeName != " "
        && nodeName != "\t" && nodeName != "\n" ) {
            
            this->push_back( nodeName );
        }
    }
}

void MoMa::NodeList::print( void ) {
    
    for( int n=0; n<size(); n++ ) {
        
        cout << at( n ) << endl;
    }
}
