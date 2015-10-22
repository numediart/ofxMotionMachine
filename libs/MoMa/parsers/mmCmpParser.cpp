//
//  mmCmpParser.cpp
//  MotionMachine
//
//  Created by Thierry Ravet on 12/08/15.
//  Copyright (c) 2015 YCAMInterlab. All rights reserved.
//

#include "mmCmpParser.h"

namespace MoMa {
    bool IsVAXOrder(void)
    {
        short x, *px;
        unsigned char *pc;
        
        px = &x;
        pc = (unsigned char *) px;
        *pc = 1; *(pc+1) = 0;         /* store bytes 1 0 */
        return x==1;          /* does it read back as 1? */
    }
    
    void SwapInt32(int *p)
    {
        char temp,*q;
        
        q = (char*) p;
        temp = *q; *q = *(q+3); *(q+3) = temp;
        temp = *(q+1); *(q+1) = *(q+2); *(q+2) = temp;
    }
    void SwapShort(short *p)
    {
        char temp,*q;
        
        q = (char*) p;
        temp = *q; *q = *(q+1); *(q+1) = temp;
    }
    CmpParser::CmpParser(){
        
    }
    CmpParser::CmpParser(std::string filename){
        if (!this->openFile(filename)){
            std::cerr<<"impossible to open data file"<<std::endl;
        }
        
    }
    CmpParser::~CmpParser(){
        this->closeFile();
        
    }
    bool CmpParser::openFile(std::string filename){
        this->closeFile();
        file.open (filename.c_str(), std::fstream::in | std::fstream::binary);
        if (file.is_open()==false)
        return false;
        file.read((char*)&hdr,sizeof(HTKhdr));
        if (IsVAXOrder()){
            SwapShort(&hdr.sampSize);
            SwapShort(&hdr.sampKind);
            SwapInt32(&hdr.sampPeriod);
            SwapInt32(&hdr.nSamples);
        }
        return true;
        
    }
    
    
    bool CmpParser::readData(arma::Col<double> &data,short code){
        
        unsigned int dim=hdr.sampSize/4;
        data.resize(dim);
        if (code==0){
            float *p=new float;
            for (int i=0;i<dim;i++){
                file.read((char*)p,sizeof(float));
                //return false;
                SwapInt32((int*)p);
                data[i]=*p;
            }
            delete p;
        }
        return true;
    }
    //ReadHTKHeader();
    bool CmpParser::getNextFrame(arma::Col<double> &frame, int derivate){
        if (file.is_open()==false)
        return false;
        if (hdr.sampKind&1024){//compressed mode. TODO to be implemented
            std::cerr<<"HTK compressed mode is not yet supported"<<std::endl;
            return false;
        }
        else{
            unsigned int dim=hdr.sampSize/4;
            arma::Col<double> data;
            if (!this->readData(data)){
                return false;
            }
            if (prev1.n_rows!=data.n_rows){
                prev2=data;
                if (!this->readData(prev1)){
                    return false;
                }
                if (!this->readData(data)){
                    return false;
                }
            }
            if (derivate==2){
                frame.resize(3*dim);
                frame.subvec(0, dim-1)=prev1;
                frame.subvec(dim, 2*dim-1)=(data-prev2)/2;
                frame.subvec(2*dim, 3*dim-1)=prev2-2*prev1+data;
            }
            else if (derivate==1){
                frame.resize(2*dim);
                frame.subvec(0, dim-1)=prev1;
                frame.subvec(dim, 2*dim-1)=(data-prev2)/2;
            }
            else{
                frame.resize(dim);
                frame=data;
            }
            prev2=prev1;
            prev1=data;
            //std::cout<<"data:"<<std::endl;
            //std::cout<<frame<<std::endl;
            return true;
        }
    }
    
    bool CmpParser::load( std::string fileName, MoMa::Track *tr, bool hasRotation ){
        this->openFile(fileName);
        unsigned int dim=hdr.sampSize/4/3;
        arma::cube trackCont(3,dim,hdr.nSamples);
        arma::Col<double> frameTemp;
        unsigned int t=0;
        while (getNextFrame(frameTemp,0)){
            trackCont.slice(t)=arma::reshape(frameTemp,3,frameTemp.n_elem/3);
            t++;
            
            
        }
        tr->position.setData(1.0E7/hdr.sampPeriod, trackCont);
        tr->setFrameRate( 1.0E7/hdr.sampPeriod );
        
        return true;
    }
    bool CmpParser::closeFile(){
        if (file.is_open()==true)
        file.close();
        return true;
    }//ReadHTKHeader();
}