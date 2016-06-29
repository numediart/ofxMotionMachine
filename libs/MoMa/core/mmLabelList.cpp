/**
 *
 *  @file mmLabelList.cpp
 *  @brief MotionMachine source file for LabelList class.
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#include "mmLabelList.h"

using namespace std;
using namespace MoMa;

LabelList::LabelList( string fName, double frameRate ) {
    
    load( fName, frameRate );
}

void LabelList::load( string fName, double frameRate ) {
    
    ifstream labFile( fName.c_str() );
    
    if( !labFile.is_open() ) {
        
        cout << "LabelList: File could not be opened!" << endl;
        return; // We alert in stdout and quit if no/wrong file!
    }
    
    _fileName = fName;
    
    while( labFile.good() ) {
        
        string curLine;
        stringstream curStrm;
        
        string labName;
        double labTime;
        
        getline( labFile, curLine ); // Grab the line
        
        if( curLine != "" && curLine != " " &&
        curLine != "\t" && curLine != "\n" ) {
            
            curStrm << curLine;
            
            curStrm >> labTime; curStrm >> labName;
            if( curStrm.good() ) curStrm >> labName;
            
            this->push_back( Label( Moment( labTime, frameRate ), labName ) );
        }
    }
}

void LabelList::save( string fName, int type, double maxTime ) {
    
    if( size() > 0 ) {
        
        std::ofstream ofs ( fName.c_str(), std::ofstream::out );
        
        switch( type ) {
                
            case EVENT:
                
                for( int i=0; i<size(); i++ ) {
                    
                    ofs << at( i ).moment.time() << " " << at( i ).name << endl;
                }
                
                break;
                
            case SEGMENT:
                
                ofs << "0 " << at( 0 ).moment.time() << " _" << endl;
                
                for( int i=0; i<size()-1; i++ ) {
                    
                    ofs << at( i ).moment.time() << " " << at( i+1 ).moment.time() << " " << at( i ).name << endl;
                }
                
                ofs << at( size()-1 ).moment.time() << " " << maxTime << " " << at( size()-1 ).name << endl;
                
                break;
                
            default:
                
                break;
        }
        
        ofs.close();
    }
}

bool comparelabels(Label i, Label j) { 

    return (i.moment.time()<j.moment.time()); 
}

void LabelList::sort() {

    std::sort(this->begin(), this->end(), comparelabels);
}

void LabelList::insert( MoMa::Moment mom, std::string nam ) {

    push_back( Label( mom, nam ) );
}

void LabelList::remove( std::string nam ) {

    for( int l=0; l<size(); l++ ) {
    
        if( at(l).name == nam ) {
        
            erase( begin()+l );
        }
    }
}

void LabelList::remove( int idx ) {
    
    erase( begin()+idx );
}

void LabelList::print( void ) {
    
    for( int n=0; n<size(); n++ ) {
        
        cout << at( n ).moment.time() << " " << at( n ).name << endl;
    }
}
