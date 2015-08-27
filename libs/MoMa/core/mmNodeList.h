/**
 *
 *  @file mmNodeList.h
 *  @brief MotionMachine header file for NodeList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmNodeList__
#define __mmNodeList__

#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstring>

#include "mmSynoList.h"

namespace MoMa {
    
    class NodeList : public std::map<std::string, int> {
        
      public:
        
        NodeList( void ) : std::map<std::string, int>() { synoList.load( synoList.DefaultPath ); }
        NodeList( std::string fileName ); // Constructor with path of the *.nodes text file
        void load( std::string fileName ); // Load names from the *.nodes text file
        
        std::string name( unsigned int idx ); // Get name string from index
        int index( std::string name ); // Get index from name string
        
        void print( void ); // Print the list of nodes
        
      protected:
        
        SynoList synoList;
    };
}

#endif
