/**
 *
 *  @file mmSynoList.h
 *  @brief MotionMachine header file for SynoList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmSynoList__
#define __mmSynoList__

#include <map>
#include <fstream>
#include <sstream>
#include <iostream>

namespace MoMa {
    
    class SynoList {
    
      public:
        
        SynoList( void ) {} // Constructor
        SynoList( std::string fileName ); // Create object from text file
        void load( std::string fileName ); // Load synonyms from text file
        
        bool search( std::string query, std::string &answer ); // Search
        
        void print( void ); // Print the list of synonyms

      private:

        std::map< std::string, std::string> syno;
    };
}

#endif
