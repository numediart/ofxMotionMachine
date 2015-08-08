/**
 *
 *  @file mmLabelList.h
 *  @brief MotionMachine header file for LabelList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmLabelList__
#define __mmLabelList__

#include <vector>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>

namespace MoMa {
    
    enum SegmentType {
        
        EVENT,
        SEGMENT
    };
    
    enum LabelState {
        
        UNSELECTED,
        HOVERED,
        SELECTED
    };

    class Label {
    
      public:
        
        Label( int i, std::string n) {
            
            idx = i; name = n;
            state = UNSELECTED;
        }
        
        int idx;
        std::string name;
        int state;
    };
    
    class LabelList : public std::vector<Label> {
        
    public:
        
        LabelList( void ) : std::vector<Label>() {}
        LabelList( std::string fName ); // Create object from text file
        
        void load( std::string fName ); // Load/save labels from text file
        void save( std::string fName, int type = EVENT, int trackSize = 0 );
        
        void insert( int idx, std::string name );
        void remove( int idx ); // Insert & remove
        
        void print( void ); // Print labels
        
        std::string name;
        std::string fileName;
    };
}

#endif
