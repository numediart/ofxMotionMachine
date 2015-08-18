//
//  mmCmpParser.h
//  MotionMachine
//
//  Created by Thierry Ravet on 12/08/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#ifndef __MotionMachine__mmCmpParser__
#define __MotionMachine__mmCmpParser__

#include <stdio.h>

#include <armadillo>
#include <iostream>

#include "mmTrack.h"
namespace MoMa {
class CmpParser
{
    typedef struct {              /* HTK File Header */
        int nSamples;
        int sampPeriod;
        short sampSize;
        short sampKind;
    } HTKhdr;
public:
    CmpParser();
    CmpParser(std::string filename);
    bool load( std::string fileName, MoMa::Track *tr, bool hasRotation =false );
    ~CmpParser();
    bool openFile(std::string filename);//ReadHTKHeader();
    bool readData(arma::Col<double> &data,short code=0);
    bool getNextFrame(arma::Col<double> &frame, int derivate=0);
    bool closeFile();//ReadHTKHeader();
    bool isOpen(){return file.is_open();}
    unsigned int getNumberOfFrames(){return (this->isOpen()?hdr.nSamples:0);}
    
private:
    
    std::fstream file;
    HTKhdr hdr;
    arma::colvec prev1;
    arma::colvec prev2;
    
    bool bSwap;
};
}
#endif /* defined(__MotionMachine__mmCmpParser__) */
