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

#ifndef MAX_PATH
#define MAX_PATH 260
#endif
/*#ifdef _WIN32
#include <windows.h>
#endif*/

namespace MoMa {
    
    //std::string getExeFilePath( void ); // get the total path and name of the executable file
    std::string getAppPath( void ); //get the path to the directory containing the executable file
    std::string getLibPath( void ); //get the path to the MoMa libraries
    std::string getDataPath( void ); //get the data path
    std::string getResPath( void ); // get resources path
}

#endif
