/**
 *
 *  @file mmNodeList.h
 *  @brief MotionMachine header file for NodeList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmNodeList__
#define __mmNodeList__

#include <vector>
#include <iostream>
#include <cstring>
#include <string>
#include <fstream>

namespace MoMa {
    
    class NodeList : public std::vector<std::string> {
        
      public:
        
        NodeList( void ) : std::vector<std::string>() {} // Constructor
        NodeList( std::string fileName ); // Create object from text file
        void load( std::string fileName ); // Load names from text file
        void print( void ); // Print the list of nodes
        
        // TODO add a push function
    };
}

#endif
