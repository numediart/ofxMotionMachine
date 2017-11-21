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

    /**
    * MoMa::XmlArchiver is a intern data exporter/importer for MoMa under XML format to archive data and intermediate results
    * 3 fields exist : tracks features and labelCollection
    */

    class XmlArchiver {
    public:
        XmlArchiver();
        ~XmlArchiver();
        /**
        *
        * addTrack add the information contained in a track to the archive.
        * @param pTrack ref to the aimed MoMa::Track.
        *
        */
        void addTrack( const MoMa::Track& pTrack );
        /**
        *
        * addTrack add the information contained in a one dimensionnal feature to the archive.
        * The information will be store under a unique list of value in the xml file.
        * @param feat ref to the aimed MoMa::TimedVec feature.
        * @param name name of the feature
        * @param trackName if this feature is linked to a track, you can use this field to store this information.
        *
        */
        void addFeature( const MoMa::TimedVec feat, std::string name, std::string trackName=std::string("") );
        /**
        *
        * addTrack add the information contained in a two dimensionnal feature to the archive.
        * Each feature frame will be stored separately
        * @param feat ref to the aimed MoMa::TimedMat feature.
        * @param name name of the feature
        * @param trackName if this feature is linked to a track, you can use this field to store this information.
        *
        */
        void addFeature( const MoMa::TimedMat feat, std::string name, std::string trackName = std::string( "" ) );
        /**
        *
        * addTrack add the information contained in a three dimensionnal feature to the archive.
        * Each feature frame will be stored separately
        * @param feat ref to the aimed MoMa::TimedCube feature.
        * @param name name of the feature
        * @param trackName if this feature is linked to a track, you can use this field to store this information.
        *
        */
        void addFeature( const MoMa::TimedCube feat, std::string name, std::string trackName = std::string( "" ) );
        /**
        *
        * addLabels add the information contained in a label list to the archive.
        * Each feature frame will be stored separately
        * @param label ref to the aimed MoMa::LabelList feature.
        * @param name name of the label collection
        * @param trackName if this label collection is linked to a track, you can use this field to store this information.
        *
        */
        void addLabels( const LabelList label, std::string name, std::string trackName = std::string( "" ) );

        /**
        *
        * save open a file and save the added information inside.
        * @param archiveFileName complete path and name of the file to archive the data.
        */
        void save( std::string archiveFileName );
        /**
        *
        * load open and parse a xml file. It counts the number of track stored in this file
        * @param archiveFileName complete path and name of the file.
        */
        void load( std::string archiveFileName );
        /**
        *
        * getTrack fill a MoMa::track with the information from one of the archived track
        * @param pTrack ref to trakc object to import data in the xml.
        * @param index index of the targeted track.
        */
        void getTrack( Track& pTrack, int index = 0 );
        /**
        *
        * getNumberOfTrack give the number of track that you can find in the archive
        *
        */
        inline int getNumberOfTrack() { return trackNum; };
        /**
        *
        * getFeature fill a MoMa::TimedContainer with the information from one of the archived feature
        * @param featureName name of the targeted feature in the xml.
        * @param pTrack ref to timedContainer (with the appropriate dimension) object to store the data.
        *
        */
        bool getFeature( std::string featureName, MoMa::TimedCube &feat );
        bool getFeature( std::string featureName, MoMa::TimedMat &feat );
        bool getFeature( std::string featureName, MoMa::TimedVec &feat );
        bool getFeature( std::string featureName, MoMa::TimedCube &feat, std::string &trackName );
        bool getFeature( std::string featureName, MoMa::TimedMat &feat, std::string &trackName );
        bool getFeature( std::string featureName, MoMa::TimedVec &feat, std::string &trackName );
        /**
        *
        * getLabel fill a MoMa::TimedContaine with the information from one of the archived feature
        * @param name name of the targeted label in the xml.
        * @param pLabelList ref to labelList object to store the data.
        *
        */
        bool getLabel( std::string name, LabelList &pLabelList );
        bool getLabel( std::string name, LabelList &pLabelList, std::string &trackName );
        

        /**
        *
        * getTrackNames return the names of tracks stored in the xml archive
        * @return a vector with the names of the tracks
        */
        std::vector<std::string> getTrackNames();
        /**
        *
        * getFeatureNames return the names of features stored in the xml archive
        * @return a vector with the names of the tracks and the dimension of this feature
        */
        std::vector< std::pair<std::string, int > > getFeatureNames();
        /**
        *
        * getLabelNames return the names of label collections stored in the xml archive
        * @return a vector with the names of the tracks
        */
        std::vector<std::string> getLabelNames();
        /**
        *
        * contentPrint display in a console the content of the xml archive
        *
        */
        void contentPrint();
        /**
        *
        * clear clear the tinyXml object 
        *
        */

        void clear();
    protected:

        std::shared_ptr<NodeList> loadNodeList( TiXmlElement * nodeRoot );
        std::shared_ptr<BoneList> loadBoneList( TiXmlElement * boneRoot );
        void loadData( TiXmlElement * frameRoot, MoMa::Track &pTrack );
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
