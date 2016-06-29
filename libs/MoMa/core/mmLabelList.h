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

#include "mmMoment.h"
#include <algorithm>

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
        
        Label( MoMa::Moment mom, std::string nam ) {
            
            moment = mom; name = nam;
            state = UNSELECTED;
        }
        
        MoMa::Moment moment;
        std::string name;
        // double time;
        int state;
    };
    
    class LabelList : public std::vector<Label> {
        
      public:
        
        LabelList( void ) : std::vector<Label>() {} // For compliance
        LabelList( std::string fName, double frameRate ); // From text file
        
        void load( std::string fName, double frameRate ); // Load/save labels
        void save( std::string fName, int type = EVENT, double maxTime = 0.0f );
        void sort();
        //bool compare(Label i, Label j);

        void insert( MoMa::Moment mom, std::string nam );
        void remove( std::string nam ); // Insert/remove
        void remove( int idx ); // here remove @ index
        
        void print( void ); // Print labels
        
      // protected:
        
        std::string _fileName;
        std::string _name;
    };
}

#endif
