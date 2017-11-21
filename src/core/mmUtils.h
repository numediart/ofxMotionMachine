/**
 *
 *  @file mmUtils.h
 *  @brief MotionMachine header file for Utils.
 *  @author Mickael Tits
 *  @copyright Numediart Institute, UMONS (c) 2014-2015
 *
 */

#ifndef __mmUtils__
#define __mmUtils__

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <armadillo>
#include <algorithm>

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

#ifdef _WIN32
 typedef unsigned int uint;
#endif

/*#ifdef _WIN32
#include <windows.h>
#endif*/

namespace MoMa {
    
    //std::string getExeFilePath( void ); // get the total path and name of the executable file
    std::string getAppPath( void ); //get the path to the directory containing the executable file
    std::string getAbsoluteAppPath( void ); //get the absolute path to the directory containing the executable file
    std::string getLibPath( void ); //get the path to the MoMa libraries
    std::string getAbsoluteLibPath( void ); //get the absolute path to the MoMa libraries
    std::string getDataPath( void ); //get the data path
	std::string getExampleDataPath(void); //get the example data path
    std::string getAbsoluteDataPath( void ); //get the absolute path to the directory containing the data
    //std::string getResPath( void ); // get resources path
    std::string getAbsoluteResPath( void ); // get absolute resources path
    std::string getName( std::string filePath ); // returns filename of the filepath
    std::string getExtension(std::string filePath); // returns filename of the filepath
    std::string getFolder(std::string filePath); // returns folder of the filepath (path without filename)

    void correctPath( std::string &path, bool invert = false ); // manage special characters bugs (extended ASCII characters). The conversion can be done in both way by use of "invert".
    void checkFilePath( std::string &path ); //check if the path exists; if not, tries to correct the path with correctPath(...)
    //std::string checkFilePath( std::string const &path ); //check if the file path exists; if not, tries to correct the path with correctPath(...)
    //std::string checkDirectoryPath( std::string const &path ); //check if the directory path exists; if not, tries to correct the path with correctPath(...)
    
    
}

#endif
