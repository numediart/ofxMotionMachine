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
        
        Label( double t, std::string n ) {
            
            time = t; name = n;
            state = UNSELECTED;
        }
        
        std::string name;
        double time;
        int state;
    };
    
    class LabelList : public std::vector<Label> {
        
      public:
        
        LabelList( void ) : std::vector<Label>() {}
        LabelList( std::string fName ); // Create object from text file
        
        void load( std::string fName ); // Load/save labels from a text file
        void save( std::string fName, int type = EVENT, double maxTime = 0.0f );
        
        void insert( double time, std::string name );
        void remove( double time ); // Insert & remove
        void remove( int idx ); // here remove @ index
        
        void print( void ); // Print labels
        
        std::string fileName;
        std::string name;
    };
}

#endif
