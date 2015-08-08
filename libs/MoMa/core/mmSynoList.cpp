/**
 *
 *  @file mmSynoList.cpp
 *  @brief MotionMachine source file for SynoList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmSynoList.h"

using namespace std;
using namespace MoMa;

SynoList::SynoList( string fileName ) {
    
    load( fileName );
}

void SynoList::load( string fileName ) {
    
    ifstream synFile( fileName.c_str() );
    
    if( !synFile.is_open() ) {
        
        cout << "SynoList: File could not be opened!" << endl;
        return; // We alert in stdout and quit if no/wrong file!
    }
    
    for( string synLine; getline(synFile, synLine); ) {
        
        stringstream synStrm;
       
        // NB: '\n' should not happen with getline.
        
        if( synLine != "" && synLine != " " &&
        synLine != "\t" && synLine != "\n" ) {
            
            string head;
            
            synStrm << synLine;
            
            synStrm >> head;
           
            // insert the main keyword as its own synonymous in map
            syno.insert( pair<string,string>(head,head) );

            string oneEquiv;

            while( synStrm >> oneEquiv ) {
                
                //insert each synonymous with its main keyword
                syno.insert( pair<string,string>(oneEquiv, head) );
            }
        }
    }
}

bool SynoList::search( std::string query, std::string &answer ) {
    
    bool isFound = false;
   
    map<string,string>::const_iterator it;

    it = syno.find( query );
    
    if (it != syno.end()) {
        
        answer = it->second;
        isFound = true;
    }
    
    // TODO 'else answer = "";' ???
    
    return( isFound );
}

void SynoList::print( void ) {
    
    map<string,string>::const_iterator it = syno.begin();

    while( it != syno.end() ) {
        
        cout << it->first << " -> ";
        cout << it->second << " ";
        cout << endl;

        ++it;
    }
}
