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

LabelList::LabelList( string fName ) {
    
    load( fName );
}

void LabelList::load( string fName ) {
    
    ifstream labFile( fName.c_str() );
    
    if( !labFile.is_open() ) {
        
        cout << "LabelList: File could not be opened!" << endl;
        return; // We alert in stdout and quit if no/wrong file!
    }
    
    fileName = fName;
    
    while( labFile.good() ) {
        
        string curLine;
        stringstream curStrm;
        
        string labName;
        int labIdx;
        
        getline( labFile, curLine ); // Grab the line
        
        if( curLine != "" && curLine != " " &&
        curLine != "\t" && curLine != "\n" ) {
            
            curStrm << curLine;
            
            curStrm >> labIdx; curStrm >> labName;
            if (curStrm.good()) {
                curStrm >> labName;
            }
            this->push_back( Label( labIdx, labName ) );
        }
    }
}

void LabelList::save( string fName, int type, int trackSize ){
    
    if( size()>0 ){
        
        std::ofstream ofs ( fName.c_str(), std::ofstream::out );
        
        switch( type ) {
                
            case EVENT:
                
                for( int i=0; i<size(); i++ ) {
                    
                    ofs << at( i ).idx << " " << at( i ).name << endl;
                }
                
                break;
                
            case SEGMENT:
                
                ofs << "0 " << at( 0 ).idx << " _" << endl;
                
                for( int i=0; i<size()-1; i++ ) {
                    
                    ofs << at( i ).idx << " " << at( i+1 ).idx << " " << at( i ).name << endl;
                }
                
                ofs << at( size()-1 ).idx << " " << trackSize << " " << at( size()-1 ).name << endl;
                
                break;
                
            default:
                
                break;
        }
        
        ofs.close();
    }
}

void LabelList::insert( int idx, std::string name ) {

    push_back( Label( idx, name ) );
}

void LabelList::remove( int idx ) {

    for( int l=0; l<size(); l++ ) {
    
        if( at(l).idx == idx ) {
        
            erase( begin()+idx );
        }
    }
}

void LabelList::print( void ) {
    
    for( int n=0; n<size(); n++ ) {
        
        cout << at( n ).idx << " " << at( n ).name << endl;
    }
}
