/** C3dParser : Parser of C3D files 
*
* Reads a C3D file and stores the 3D motion data in a MoMa::Track
* Handles all standard Machine formats : Intel-PC, DEC-VAX and MIPS/SUN; and Data formats : int, float
*
* Created by Mickaël Tits on 05/03/2015
* @file mmC3dParser.h
* @brief MotionMachine header file for C3dParser class.
* @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#ifndef __mmC3dParser__
#define __mmC3dParser__

#include <iostream>
#include <fstream>
#include <iterator>
#include <algorithm>
#include <vector>
#include <iomanip>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <cmath>
#include <ctype.h>
#include <cctype>
#include <stdint.h>
#include <cfloat>


#include "mmTrack.h"

namespace MoMa {

    typedef int8_t int8;
    typedef unsigned char uint8;
    typedef uint16_t int16;
    typedef int16_t sint16;

    struct strucEvent {
        
        float time;
        int8 value;
        char name[4];
        strucEvent();
    };

    struct structParameter {
        
        std::vector<std::string> data;
        std::string name;
        std::vector<uint8> dim;
        std::string description;
        std::string dataType;
        structParameter();
    };

    struct structParameterGroup {
        
        std::vector <structParameter> parameter;
        std::string name;
        std::string description;
        structParameterGroup();
    };

    class C3dParser {

      public:

        C3dParser( std::string const &fileName, Track *track );
        void load( std::string const &fileName, Track *track );
        
        
        

      private :

        static void decToFloat( float* value );
        static void swapbytes(void *object, size_t size);
        static size_t VAXread(void * ptr, size_t size, size_t count, FILE * stream);
        static size_t MIPSread(void * ptr, size_t size, size_t count, FILE * stream);
        float fix( float value );

        bool debug;
        FILE *fid;
        int8 NrecordFirstParameterblock;
        int8 key;
        int8 proctype;
        size_t (*reading) (void*, size_t, size_t, FILE *);

        /// Header ///
        int16 Nmarkers, NanalogSamplesPerVideoFrame, StartFrame, EndFrame, MaxInterpolationGap, NrecordDataBlock, NanalogFramesPerVideoFrame,NanalogChannels, NvideoFrames;
        float Scale, VideoFrameRate, AnalogFrameRate;

        /// Events ///
        int16 EventIndicator, Nevents;
        std::vector<strucEvent> Event;

        /// Parameter Blocks ///
        int8 dat1, key2, NparameterRecords, Ncharacters, deschars, type, dimnum, GroupNumber, Nparameters, testoffset;
        int16 offset;
        uint8 *dimension, wordlength;
        int mult, datalength;
        int nextrec, filepos, testpos;
        int ParameterNumber;
        char *GroupName, *GroupDescription, *ParameterName, *data, *description;
        std::vector<structParameterGroup> ParameterGroup;
        std::vector<int> ParameterNumberIndex;

        /// Data blocks ///

        float b,highbyte,lowbyte,tmp[3];
        sint16 a;

    };
}

#endif
