/**
*
*  @file mmXmlArchive.h
*  @brief MotionMachine header file for XmlArchiver class.
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#ifndef __mmXmlArchiver__
#define __mmXmlArchiver__
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "tinyxml.h"
#include "mmTrack.h"
#include "mmLabelList.h"


namespace MoMa {
    class XmlArchiver {
    public:
        XmlArchiver();
        ~XmlArchiver();
        void addTrack( const MoMa::Track& pTrack );
        void addFeature( const MoMa::TimedVec feat, std::string name, std::string trackName=std::string("") );
        void addFeature( const MoMa::TimedMat feat, std::string name, std::string trackName = std::string( "" ) );
        void addFeature( const MoMa::TimedCube feat, std::string name, std::string trackName = std::string( "" ) );
        void addLabels( const LabelList label, std::string name, std::string trackName );
        void save( std::string archiveFileName );
        void load( std::string archiveFileName );
        NodeList *loadNodeList( TiXmlElement * nodeRoot );
        BoneList *loadBoneList( TiXmlElement * boneRoot );
        void loadData( TiXmlElement * frameRoot, MoMa::Track &pTrack );
        void getTrack( Track& pTrack, int index = 0 );
        inline int getNumberOfTrack() { return trackNum; };


        bool getFeature( std::string featureName, MoMa::TimedCube &feat );
        bool getFeature( std::string featureName, MoMa::TimedMat &feat );
        bool getFeature( std::string featureName, MoMa::TimedVec &feat );
        void clear();
    protected:

        TiXmlDocument mArchiver; 
        TiXmlElement* mRoot;
        bool rootPositionOnlyFlag;
        int trackNum;
        int featureNum;
        int labelNum;
        bool binaryData;
    };
};
#endif //__mmXmlArchiver__