/**
 *
 *  @file mmNodeList.cpp
 *  @brief MotionMachine source file for NodeList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

/*
int MoMa::Frame::index( std::string name ) {
    
    // TODO make more modular
    
    int nIdx = -1; // Initialise
    bool isFound = false; // Flag
    
    if( hasNodeList ) {
        
        for( int n=0; n<nodeList->size(); n++ ) {
            
            // If we find a matching name, we save index
            if( nodeList->at(n).compare( name ) == 0 ) {
                
                isFound = true; nIdx = n;
            }
        }
        
        if( hasSynoList && !isFound ) {
            
            for( int n=0; n<nodeList->size(); n++ ) {
                
                string nNameQuery = "nq"; // Synonym of name query
                string nFromList = "fl"; // Synonym of nth in list
                
                synoList->search( name, nNameQuery ); // Search name
                synoList->search( nodeList->at(n), nFromList ); // & list
                
                if( nFromList.compare( nNameQuery ) == 0 ) {
                    
                    nIdx = n; // :)
                    isFound = true;
                }
            }
        }
        
        if( !isFound ) {
            
            cout << "Frame::index: Node not found" << endl;
        }
        
    } else {
        
        cout << "Frame::index: No node name list" << endl;
    }
    
    return( nIdx );
}
*/

#include "mmNodeList.h"

using namespace std;
using namespace MoMa;

NodeList::NodeList( string fileName ) {
    
    load( fileName ); // load NodeList file content
    synolist( synoList.DefaultPath ); // load SynoList
}

void NodeList::load( string fileName ) {
    
    unsigned int idx = 0u;
    ifstream nodFile( fileName.c_str() );
    
    if( !nodFile.is_open() ) {
        
        cout << "NameList: File could not be opened!" << endl;
        return; // We alert in stdout and quit if no/wrong file!
    }
    
    while ( nodFile.good() ) {
        
        string nodeName; // Grab name
        getline( nodFile, nodeName );
        
        if( nodeName != "" && nodeName != " " &&
        nodeName != "\t" && nodeName != "\n" ) {
            
            this->insert( make_pair( nodeName, idx ) );
            idx++; // Insert new pair and increment idx
        }
    }
}

bool NodeList::synolist( string fileName ) {

    return this->synoList.load(fileName);
}

std::string NodeList::name( unsigned int idx ) {
    
    std::string foundName = NameNotFound;
    
    map<string, int>::const_iterator it = this->begin();
    
    while( it != this->end() ) {
        
        if( it->second == idx ) {
            foundName = it->first;
            break; // once you find it, exit the loop
        }
        ++it; // Check if index exists then increment
    }
    
    return( foundName );
}

int NodeList::index( std::string name ) {
    
    int nIdx = -1; // Initialise
    bool isFound = false; // Is found?
    map<string, int>::const_iterator it;
    
    it = this->find( name ); // Find index
    
    if( it != this->end() ) {
        
        nIdx = it->second;
        isFound = true;
    
    } else {
        
        string nNameQuery = "nq"; // Synonym of name query
        synoList.search( name, nNameQuery ); // Search name

        // Browse nodeList
        it = this->begin();
        
        while( it != this->end() ) {
            
            string nFromList = "fl"; // Synonym of nth in list
            
            synoList.search( it->first, nFromList ); // and list
            
            if( nFromList.compare( nNameQuery ) == 0 ) {
                
                nIdx = it->second;
                isFound = true;
                break; // once you find it, exit the loop
            }
            
            ++it;
        }
        
        if( !isFound ) {
            
            cout << "NodeList: Node not found" << endl;
        }
    }
    
    return( nIdx );
}

void NodeList::rename(std::string oldname, std::string newname) {

	this->operator[](newname) = this->index(oldname);
	this->erase(oldname);
}

void NodeList::print( void ) {
    
    map<string,int>::const_iterator it = this->begin();
    
    while( it != this->end() ) {
        
        cout << it->first << " -> ";
        cout << it->second << endl;
        
        ++it;
    }
}
