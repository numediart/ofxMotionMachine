/** C3dParser : Parser of C3D files
*
* Reads a C3D file and stores the data in a MoMa::Track
* Handles all standard Machine formats : Intel-PC, DEC-VAX and MIPS/SUN; and Data formats : int, float
*
* Created by Mickaël Tits on 05/03/2015
* @file mmC3dParser.cpp
* @brief MotionMachine source file for C3dParser class.
* @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmC3dParser.h"

using namespace std;
using namespace MoMa;

strucEvent::strucEvent(void) {

    time = 0;
    value = 0;
}

structParameter::structParameter(void) {

    name = "";
    description = "";
    dataType = "";
}

structParameterGroup::structParameterGroup(void) {

    name = "";
    description = "";
}

C3dParser::C3dParser(std::string const &fileName, MoMa::Track *track) {

    /*#ifdef _DEBUG
        debug = true;
    #else
        debug = false;
    #endif*/
    debug = false;

    fid = NULL;
    reading = NULL;
    load(fileName, track);
}

void C3dParser::load(string const &fileName, Track *track) {

    bool printing = debug;
    size_t fres = 0;
    int ret = 0;
    string unit = "";

    /// Open file ///
    if (printing) {

        printf("\n\n----Opening file...----\n\n");
    }

    cout << "Trying to open : \"" << fileName << "\"" << endl;
    fid = fopen(fileName.c_str(), "rb"); // native format (PC-intel)

    if (fid == NULL) {

        cout << "File could not be opened.\n";
        return;
    }

    /// Clear everything except bones and create a new nodeList ///
    std::shared_ptr<NodeList> nodeList;
    nodeList = std::make_shared<NodeList>();
    track->clearData();
    if (track->hasNodeList) {
        track->nodeList = 0;
    }
    (track->rotationOffset).clear();
    track->nodeList = nodeList;

    fres = fread(&NrecordFirstParameterblock, sizeof(int8), 1, fid); // Reading record number of parameter section
    fres = fread(&key, sizeof(int8), 1, fid); // key = 80

    fseek(fid, 512 * (NrecordFirstParameterblock - 1) + 3, 0); // ump to processortype - field
    fres = fread(&proctype, sizeof(int8), 1, fid);
    proctype = proctype - 83;                       // proctype: 1(INTEL-PC); 2(DEC-VAX); 3(MIPS-SUN/SGI)

    switch (proctype) {

    case 1:

        if (printing) {
            cout << "Machine format : INTEL-PC" << endl;
        }
        reading = fread;
        break;

    case 2:

        if (printing) {
            cout << "Machine format : DEC-VAX" << endl;
        }
        reading = C3dParser::VAXread;
        break;

    case 3:

        if (printing) {
            cout << "Machine format : MIPS-SUN/SGI" << endl;
        }
        reading = C3dParser::MIPSread;
        break;
    }

    /// Read First Parameters ///
    if (printing) {
        printf("Parameters : \n");
        printf("NrecordFirstParameterblock : %d\n", NrecordFirstParameterblock);
        printf("key : %d\n", key);
        printf("proctype : %d\n", proctype);
    }

    /// Read Header ///
    if (printing) {
        cout << "\n\n----Reading Header...---- \n\n";
    }

    fseek(fid, 2, SEEK_SET);
    fres = reading(&Nmarkers, sizeof(int16), 1, fid);                         // number of markers
    fres = reading(&NanalogSamplesPerVideoFrame, sizeof(int16), 1, fid);      // number of analog channels x #analog frames per video frame
    fres = reading(&StartFrame, sizeof(int16), 1, fid);                       //# of first video frame
    fres = reading(&EndFrame, sizeof(int16), 1, fid);                             //# of last video frame
    fres = reading(&MaxInterpolationGap, sizeof(int16), 1, fid);              //maximum interpolation gap allowed (in frame)
    fres = reading(&Scale, sizeof(float), 1, fid);                            // floating-point scale factor to convert 3D-integers to ref system units                           
    fres = reading(&NrecordDataBlock, sizeof(int16), 1, fid);                 // starting record number for 3D point and analog data
    fres = reading(&NanalogFramesPerVideoFrame, sizeof(int16), 1, fid);

    if (NanalogFramesPerVideoFrame > 0)
        NanalogChannels = NanalogSamplesPerVideoFrame / NanalogFramesPerVideoFrame;
    else NanalogChannels = 0;

    fres = reading(&VideoFrameRate, sizeof(float), 1, fid);
    track->setFrameRate((float)VideoFrameRate);
    AnalogFrameRate = VideoFrameRate*NanalogFramesPerVideoFrame;

    if (printing) {
        ret = system("pause");
        printf("Nmarkers : %d\n", Nmarkers);
        printf("NanalogSamplesPerVideoFrame : %d\n", NanalogSamplesPerVideoFrame);
        printf("StartFrame : %d\n", StartFrame);
        printf("EndFrame : %d\n", EndFrame);
        printf("MaxInterpolationGap : %d\n", MaxInterpolationGap);
        printf("Scale : %f\n", Scale);
        printf("NrecordDataBlock : %d\n", NrecordDataBlock);
        printf("NanalogFramesPerVideoFrame : %d\n", NanalogFramesPerVideoFrame);
        printf("VideoFrameRate : %f\n", VideoFrameRate);
    }


    /// Read Events ///
    if (printing) {
        cout << "\n\n----Reading Events...----\n\n";
        ret = system("pause");
    }

    fseek(fid, 298, SEEK_SET);
    fres = reading(&EventIndicator, sizeof(int16), 1, fid);
    if (EventIndicator == 12345) {
        fres = reading(&Nevents, sizeof(int16), 1, fid);
        if (printing) {
            printf("Nevents : %d\n", Nevents);
        }
        fseek(fid, 2, SEEK_CUR); // skip one position/2 bytes
        if (Nevents > 0) {
            for (int i = 0; i < Nevents; i++) {
                Event.push_back(*new strucEvent());
                fres = reading(&(Event[i].time), sizeof(float), 1, fid);
            }
            fseek(fid, 188 * 2, SEEK_SET);
            for (int i = 0; i < Nevents; i++) {
                fres = fread(&(Event[i].value), sizeof(int8), 1, fid);
            }
            fseek(fid, 198 * 2, SEEK_SET);
            for (int i = 0; i < Nevents; i++) {
                fres = fread(Event[i].name, sizeof(char) * 4, 1, fid);  // not sure if this works, never tried events (ça marche pas, il est défini en local donc il est supprimé après le if. Faut plutôt définir un tableau dynamique avant, et définir sa taille dans le if avec un new)
            }
        }
        //cout << "Event 0 : time : " << Event[0].time << "\n value : " << Event[0].value << "\n name : " << string(Event[0].name,4) << endl;
    }



    /// Read 1st parameter block ///
    if (printing) {
        cout << "\n\n----Reading Parameters...----\n\n";
        ret = system("pause");
    }

    fseek(fid, 512 * (NrecordFirstParameterblock - 1), SEEK_SET);


    fres = fread(&dat1, sizeof(int8), 1, fid);
    fres = fread(&key2, sizeof(int8), 1, fid);                        // key = 80;
    fres = fread(&NparameterRecords, sizeof(int8), 1, fid);
    fres = fread(&proctype, sizeof(int8), 1, fid);
    //proctype=proctype-83;                                   // proctype: 1(INTEL-PC); 2(DEC-VAX); 3(MIPS-SUN/SGI)


    fres = fread(&Ncharacters, sizeof(int8), 1, fid);                 // characters in group/parameter name
    fres = fread(&GroupNumber, sizeof(int8), 1, fid);                 // id number -ve=group / +ve=parameter

    /// Read  allparameter blocks ///
    while (Ncharacters > 0) // The end of the parameter record is indicated by <0 characters for group/parameter name
    {
        //ret = system("pause");
        if (GroupNumber < 0) { /// Group data

            if (printing) {
                cout << "\n\n====Group data====\n\n";
                ret = system("pause");
            }
            structParameterGroup p;
            GroupNumber = abs(GroupNumber);
            GroupName = new char[Ncharacters];
            fres = fread(GroupName, sizeof(int8), Ncharacters, fid);
            p.name = string(GroupName, Ncharacters);   //group name
            fres = reading(&offset, sizeof(int16), 1, fid);                           //offset in bytes
            fres = fread(&deschars, sizeof(int8), 1, fid);
            GroupDescription = new char[deschars];     //description characters
            fres = fread(GroupDescription, sizeof(char), deschars, fid);
            p.description = string(GroupDescription, deschars); //group description
            if (ParameterGroup.size() < GroupNumber) {

                ParameterGroup.resize(GroupNumber);
                ParameterNumberIndex.resize(GroupNumber);
            }
            //ParameterGroup.push_back(p);
            ParameterGroup[GroupNumber - 1] = p;
            //ParameterNumberIndex.push_back(0);
            ParameterNumberIndex[GroupNumber - 1] = 0;
            fseek(fid, offset - 3 - deschars, SEEK_CUR);
            if (printing) {
                ret = system("pause");
                printf("Group %d : \n \t- Name : %s\n \t- Ncharacters : %d\n \t- GroupDescription : %s\n ", GroupNumber, GroupName, Ncharacters, GroupDescription);
                cout << p.name << "\t" << p.description << endl;
                ret = system("pause");
            }
            delete GroupDescription;
            delete GroupName;

        }

        else { /// Parameter data

            if (printing) {
                printf("\n\n====Parameters data====\n\n");
                ret = system("pause");
            }
            structParameter param;
            if (ParameterNumberIndex.size() < GroupNumber) {

                ParameterNumberIndex.resize(GroupNumber);
                ParameterNumberIndex[GroupNumber - 1] = 1;
            }
            else {

                ParameterNumberIndex[GroupNumber - 1] = ParameterNumberIndex[GroupNumber - 1] + 1;
            }
            ParameterNumber = ParameterNumberIndex[GroupNumber - 1];              // index all parameters within a group
            ParameterName = new char[Ncharacters];

            /// read parameter name
            fres = fread(ParameterName, sizeof(int8), Ncharacters, fid);                  // name of parameter
            param.name = string(ParameterName, Ncharacters);   //save parameter name

            /// read offset
            fres = reading(&offset, sizeof(int16), 1, fid);                           //offset of parameters in bytes
            filepos = ftell(fid);
            nextrec = filepos + offset - 2;                           //position of beginning of next record

            /// read type
            fres = fread(&type, sizeof(int8), 1, fid);     // type of data: -1=char/1=byte/2=integer*2/4=real*4

            /// read number of dimensions
            fres = fread(&dimnum, sizeof(int8), 1, fid);
            dimension = new uint8[dimnum];
            if (dimnum == 0)
                datalength = abs(type);                               //length of data record
            else {
                mult = 1;
                for (int j = 0; j < dimnum; j++) {
                    fres = fread(&(dimension[j]), sizeof(uint8), 1, fid);
                    mult = mult*dimension[j];
                    (param.dim).push_back(dimension[j]);
                }
                datalength = abs(type)*mult;                          //length of data record for multi-dimensional array
            }


            if (type == -1) { /// datatype=='char'

                param.dataType = "char";
                wordlength = dimension[0];    //length of character word
                data = new char[wordlength];
                if (dimnum == 2 && datalength > 0) { //& parameter(idnumber,index,2).dim>0
                    for (int j = 0; j < dimension[1]; j++) {
                        fres = fread(data, sizeof(char), wordlength, fid);    //character word data record for 2-D array
                        (param.data).push_back(string(data, wordlength));
                        if (ParameterGroup[GroupNumber - 1].name == "POINT" && param.name == "LABELS" && dimension[1] == Nmarkers) {
                            if (printing) {
                                cout << "param.data[" << j << "] : " << param.data[j] << endl;
                                ret = system("pause");
                            }
                            //ret = system("pause");

                            // This bit of code removes the space characters that
                            // sometimes happen after the extracted node names. It's
                            // a hack because I have no idea why they get added.

                            // TODO We need to review the parser for errors.

                            bool isGoing = true;

                            while (isGoing) {

                                size_t found = param.data[j].find(' ');

                                if (found != string::npos) {

                                    param.data[j].erase(found, 1);

                                }
                                else {

                                    isGoing = false;
                                }
                            }

                            //nodeList->push_back(param.data[j]);
                            nodeList->insert(make_pair(param.data[j], j));
                            track->hasNodeList = true;
                        }
                    }
                }
                else if (dimnum == 1 && datalength > 0) {
                    fres = fread(data, sizeof(char), wordlength, fid);        //numerical data record of 1-D array
                    (param.data).push_back(string(data, wordlength));
                    if (ParameterGroup[GroupNumber - 1].name == "POINT" && param.name == "UNITS") {

                        if (printing) {
                            cout << "param.data[0] (unit) : " << param.data[0] << endl;
                            ret = system("pause");
                        }

                        // This bit of code removes the space characters that
                        // sometimes happen after the extracted node names. It's
                        // a hack because I have no idea why they get added.

                        // TODO We need to review the parser for errors.

                        bool isGoing = true;

                        while (isGoing) {

                            size_t found = param.data[0].find(' ');

                            if (found != string::npos) {

                                param.data[0].erase(found, 1);

                            }
                            else {

                                isGoing = false;
                            }
                        }

                        unit = param.data[0];
                    }
                }
                delete data;

            }

            else if (type == 1) {    /// 1-byte for boolean

                param.dataType = "bool";
                Nparameters = datalength / abs(type);
                data = new char[Nparameters];
                fres = fread(data, sizeof(int8), Nparameters, fid);
                (param.data).push_back(string(data, Nparameters));
                delete data;

            }

            else if (type == 2 && datalength > 0) {       /// integer

                param.dataType = "int";
                Nparameters = datalength / abs(type);
                data = new char[2 * Nparameters];
                //fres = fread(data,sizeof(int16),Nparameters,fid); //if it doesn't work, replace with next line (in comment)
                fres = fread(data, sizeof(int8), 2 * Nparameters, fid);
                (param.data).push_back(string(data, 2 * Nparameters));
                delete data;

            }

            else if (type == 4 && datalength > 0) {
                param.dataType = "float";
                Nparameters = datalength / abs(type);
                data = new char[4 * Nparameters];
                //fres = fread(data,sizeof(float),Nparameters,fid); //if it doesn't work, replace with next line (in comment)
                fres = fread(data, sizeof(int8), 4 * Nparameters, fid);
                (param.data).push_back(string(data, 4 * Nparameters));
                delete data;

            }

            else {

                // error

            }

            fres = fread(&deschars, sizeof(int8), 1, fid);                            //description characters

            if (deschars > 0) {
                description = new char[deschars];
                fres = fread(description, sizeof(char), deschars, fid);
                param.description = string(description, deschars);
                delete description;
            }

            //moving ahead to next record
            ParameterGroup[GroupNumber - 1].parameter.push_back(param);
            if (printing) {
                cout << "\nParameter " << ParameterNumber << " of group " << abs(GroupNumber) << ":\n \t- Name : " << param.name << "\n \t- Description : " << param.description << "\n \t- Type : " << param.dataType << endl << endl;
                ret = system("pause");
            }

            fseek(fid, nextrec, SEEK_SET);
            delete dimension;
            delete ParameterName;

        }

        fres = fread(&Ncharacters, sizeof(int8), 1, fid);             // characters in next group/parameter name
        fres = fread(&GroupNumber, sizeof(int8), 1, fid);             // id number -ve=group / +ve=parameter

    }

    /// Summary of parameter blocks///
    if (printing) {
        cout << "Summary of parameters :\n";
        //ret = system("pause");
        for (int i = 0; i < ParameterGroup.size(); i++) {
            cout << "\n\nGroup " << i + 1 << ": \n\tName : " << ParameterGroup[i].name << "\n\tDescription : " << ParameterGroup[i].description << endl;
            structParameter param;
            for (int j = 0; j < ParameterGroup[i].parameter.size(); j++) {
                param = ParameterGroup[i].parameter[j];
                cout << "\n\tParameter " << j + 1 << ": \n\t\tName : " << param.name << "\n\t\tDescription : " << param.description << "\n\t\tType : " << param.dataType << endl;
            }
        }

        printf("\nNumber of markers : %d\n", Nmarkers);
        printf("Number of frames : %d\n", EndFrame - StartFrame + 1);
        printf("MaxInterpolationGap : %d\n", MaxInterpolationGap);
        printf("VideoFrameRate : %f\n", VideoFrameRate);
        printf("NrecordDataBlock : %d\n", NrecordDataBlock);
    }


    /*
    /// Store point labels in NodeList ///


    // Find the location of the markers' labels in the parameters
    int PointGroup, LabelsParameter;
    for(int i=0;i<ParameterGroup.size();i++){
    if(ParameterGroup[i].name=="POINT"){
    PointGroup=i;
    for(int j=0;j<ParameterGroup[i].parameter.size();j++){
    if(ParameterGroup[i].parameter[j].name=="LABELS"){
    LabelsParameter=j;
    break;
    }
    }
    break;
    }
    }

    // Store the labels in NodeList
    structParameter param = ParameterGroup[PointGroup].parameter[LabelsParameter];
    */

    /// Read data blocks ///
    if (printing) {
        printf("\n\n====Reading data blocks====\n\n");
        ret = system("pause");
    }

    fseek(fid, (NrecordDataBlock - 1) * 512, SEEK_SET);

    NvideoFrames = EndFrame - StartFrame + 1;
    if (printing) {
        cout << "Scale : " << Scale << endl;
    }

    if (Scale < 0) {
        vector<vector<vector<float> > > Markers((int)NvideoFrames, vector<vector<float> >((int)Nmarkers, vector<float>(3, FLT_MAX)));
        vector<vector<float> > CameraInfo((int)NvideoFrames, vector<float>((int)Nmarkers));
        vector<vector<float> > ResidualError((int)NvideoFrames, vector<float>((int)Nmarkers));
        vector<vector<float> > AnalogSignals((int)NvideoFrames*(int)NanalogFramesPerVideoFrame, vector<float>((int)NanalogChannels));
        float* analogtmp;
        analogtmp = new float[NanalogChannels];
        if (printing) {
            cout << "Markers data format : float\n";
        }
        arma::cube positionData = arma::ones(3, Nmarkers, NvideoFrames)*arma::datum::nan;
        for (int i = 0; i < NvideoFrames; i++) {
            //Frame tempFrame;
            for (int j = 0; j < Nmarkers; j++) {
                //Node tempNode;
                //tempNode.setPosition( arma::datum::nan, arma::datum::nan, arma::datum::nan );
                for (int k = 0; k < 3; k++) {
                    fres = reading(&(Markers[i][j][k]), sizeof(float), 1, fid);
                    //printf("Marker %d %d %d : %f \t", i,j,k,Markers[i][j][k]);
                }
                //cout << endl;
                //ret = system("pause");

                fres = reading(&b, sizeof(float), 1, fid);
                a = (sint16)b;
                if (a != -1) {
                    b = fix(b);
                    highbyte = fix(b / 256);
                    lowbyte = b - highbyte * 256;
                    CameraInfo[i][j] = highbyte;
                    ResidualError[i][j] = lowbyte*abs(Scale);
                    positionData(0, j, i) = Markers[i][j][0];
                    positionData(1, j, i) = Markers[i][j][1];
                    positionData(2, j, i) = Markers[i][j][2];
                    //tempNode.setPosition(Markers[i][j][0],Markers[i][j][1],Markers[i][j][2]);
                }
                else {
                    //cout << "Invalid 3D Point (the marker is missing).\nPoint : " << Markers[i][j][0] << "," << Markers[i][j][1] << "," << Markers[i][j][2] << endl;
                }
                //tempFrame.push(tempNode);
            }
            //            track->push(tempFrame);

            for (int j = 0; j < NanalogFramesPerVideoFrame; j++) {

                fres = reading(analogtmp, sizeof(float), NanalogChannels, fid);
                //copy((AnalogSignals[j+NanalogFramesPerVideoFrame*(i-1)]).begin(),(AnalogSignals[j+NanalogFramesPerVideoFrame*(i-1)]).end(),analogtmp);    //TODO correctly!
            }

        }
        if (unit == "m") {

            positionData *= 1000; //Default unit in motionmachine is mm
        }
        else if (unit == "dm") {

            positionData *= 100; //Default unit in motionmachine is mm
        }
        else if (unit == "cm") {

            positionData *= 10; //Default unit in motionmachine is mm
        }
        else if (unit == "mm") {

            //Do nothing //Default unit in motionmachine is mm
        }

        track->position.setData(VideoFrameRate, positionData);

        delete analogtmp;
    }

    else {
        vector<vector<vector<sint16> > > Markers((int)NvideoFrames, vector<vector<sint16> >((int)Nmarkers, vector<sint16>(3, INT16_MAX)));
        vector<vector<int8> > CameraInfo((int)NvideoFrames, vector<int8>((int)Nmarkers));
        vector<vector<int8> > ResidualError((int)NvideoFrames, vector<int8>((int)Nmarkers));
        vector<vector<int16> > AnalogSignals((int)NvideoFrames*(int)NanalogFramesPerVideoFrame, vector<int16>((int)NanalogChannels));
        int16* analogtmp;
        analogtmp = new int16[NanalogChannels];

        if (printing) {
            cout << "Markers data format : int\n";
        }

        for (int i = 0; i < NvideoFrames; i++) {
            Frame tempFrame;
            for (int j = 0; j < Nmarkers; j++) {
                Node tempNode;
                tempNode.setPosition(arma::datum::nan, arma::datum::nan, arma::datum::nan);
                for (int k = 0; k < 3; k++) {
                    fres = reading(&(Markers[i][j][k]), sizeof(int16), 1, fid);
                }

                fres = fread(&(ResidualError[i][j]), sizeof(int8), 1, fid);
                fres = fread(&(CameraInfo[i][j]), sizeof(int8), 1, fid);
                if (ResidualError[i][j] < 0) {
                    //cout << "Invalid 3D Point (the marker is missing).\nPoint : " << Markers[i][j][0] << "," << Markers[i][j][1] << "," << Markers[i][j][2] << endl;
                }
                else {

                    tempNode.setPosition((float)Markers[i][j][0] * Scale, (float)Markers[i][j][1] * Scale, (float)Markers[i][j][2] * Scale);
                }

                tempFrame.push(tempNode);

            }
            track->push(tempFrame);
            for (int j = 0; j < NanalogFramesPerVideoFrame; j++) {

                fres = reading(analogtmp, sizeof(int16), NanalogChannels, fid);
                //copy((AnalogSignals[j+NanalogFramesPerVideoFrame*(i-1)]).begin(),(AnalogSignals[j+NanalogFramesPerVideoFrame*(i-1)]).end(),analogtmp);    //TODO correctly!
            }
        }
        delete analogtmp;
    }

    fclose(fid);

    if (printing) {
        cout << "Finished reading\n";
    }
}


float C3dParser::fix(float value) {
    if (value < 0.0)
        return ceil(value);
    else
        return floor(value);
}

size_t C3dParser::VAXread(void * ptr, size_t size, size_t count, FILE * stream) {

    size_t ret = fread(ptr, size, count, stream);

    if (size == sizeof(float))
        decToFloat((float*)ptr);

    return ret;
}

size_t C3dParser::MIPSread(void * ptr, size_t size, size_t count, FILE * stream) {

    size_t ret = fread(ptr, size, count, stream);
    swapbytes(ptr, size);

    return ret;
}

/*size_t C3dParser::floatread(void * ptr, size_t count, FILE * stream){

    size_t ret = fread(ptr,sizeof(float),count,stream);

    if(proctype == 2){
        decToFloat( (float*)ptr );
    }
    else if(proctype == 3){
        //mipsToFloat( (float*)ptr );
        swapbytes( ptr, sizeof(float) );
    }
    return ret;
}*/

/*size_t C3dParser::int16read(void * ptr, size_t count, FILE * stream){

    size_t ret = fread(ptr,sizeof(int16),count,stream);

    if(proctype == 3){
        swapbytes( ptr, sizeof(int16) );
    }
    return ret;
}*/


void C3dParser::decToFloat(float *val)
{
    float tmp;

    int8 *dst = (int8 *)&tmp;
    int8 *src = (int8 *)val;

    dst[0] = src[2];
    dst[1] = src[3];
    dst[2] = src[0];
    dst[3] = src[1];

    if (*val != 0) --dst[3];

    *val = tmp;
}

/*void C3dParser::mipsToFloat(float *val)
{
float newval;

int8 *dst = ( int8 *)&newval;
int8 *src = ( int8 *)val;

dst[0] = src[3];
dst[1] = src[2];
dst[2] = src[1];
dst[3] = src[0];

if(debug){
cout << "input : " << *val << endl;
printf("src : %d, %d, %d, %d\n",src[0],src[1],src[2],src[3]);
printf("dst : %d, %d, %d, %d\n",dst[0],dst[1],dst[2],dst[3]);
cout << "output : " << newval << endl;
}

*val = newval;
}*/

void C3dParser::swapbytes(void *object, size_t size)
{
    int8 *start, *end;
    for (start = (int8*)object, end = start + size - 1; start < end; ++start, --end)
    {
        int8 swap = *start;
        *start = *end;
        *end = swap;
    }
}
