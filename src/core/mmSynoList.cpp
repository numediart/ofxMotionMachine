/**
*
*  @file mmSynoList.cpp
*  @brief MotionMachine source file for SynoList class.
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmSynoList.h"

using namespace std;
using namespace MoMa;

const std::string SynoList::DefaultPath = getAbsoluteResPath() + "SynoList.txt";


SynoList::SynoList( string fileName ) {

    load( fileName );
}

bool SynoList::load( string fileName ) {

    ifstream synFile( fileName.c_str() );

    if( !synFile.is_open() ) {

        cout << "SynoList: File could not be opened!" << endl;
        return false; // Stdout alert & quit if no/wrong file!
    }

    syno.clear();

    for( string synLine; getline(synFile, synLine); ) {

        stringstream synStrm;

        if( synLine != "" && synLine != " " &&
            synLine != "\t" && synLine != "\n" ) {

                string head;
                string oneEquiv;

                // Get stream head from text line
                synStrm << synLine;
                synStrm >> head;

                // Insert the main keyword as its own synonymous
                syno.insert( pair<string,string>( head, head ) );

                while( synStrm >> oneEquiv ) {

                    // Insert each synonymous with its main keyword
                    syno.insert( pair<string,string>( oneEquiv, head ) );
                }
        }
    }

    return true;
}

bool SynoList::search( std::string query, std::string &answer ) {

    bool isFound = false;

    map<string,string>::const_iterator it;

    it = syno.find( query );

    if( it != syno.end() ) {

        answer = it->second;
        isFound = true;

    } else {

        answer = NameNotFound;
        isFound = false;
    }

    return( isFound );
}

void SynoList::print( void ) {

    map<string,string>::const_iterator it = syno.begin();

    while( it != syno.end() ) {

        cout << it->first << " -> ";
        cout << it->second << " ";
        cout << endl;

        ++it;
    }
}
