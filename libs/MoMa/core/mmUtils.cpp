/**
*
*  @file mmUtils.cpp
*  @brief MotionMachine source file for Utils.
*  @author Mickael Tits
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmUtils.h"

using namespace std;

//string MoMa::getExeFilePath( void ) {
//
//    return "";
//
//    // taken from ofFileUtils
//    /*#if defined(_WIN32)
//    vector<char> executablePath(MAX_PATH);
//    DWORD result = ::GetModuleFileNameA(NULL, &executablePath[0], static_cast<DWORD>(executablePath.size()));
//    if(result == 0) {
//    cout << "getAppPath(): couldn't get path, GetModuleFileNameA failed";
//    }else{
//    return string(executablePath.begin(), executablePath.begin() + result);
//    }
//    #elif defined(__APPLE_CC__)
//    char path[FILENAME_MAX];
//    uint32_t size = sizeof(path);
//    if(_NSGetExecutablePath(path, &size) != 0){
//    cout << "getAppPath(): path buffer too small, need size " <<  size;
//    }
//    return path;
//    #else //Linux
//    char buff[FILENAME_MAX];
//    ssize_t size = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
//    if (size == -1){
//    cout << "getAppPath(): readlink failed with error " << errno;
//    }
//    else{
//    buff[size] = '\0';
//    return buff;
//    }
//
//    #endif
//    return "";*/
//}

string MoMa::getAppPath( void ) {

#ifdef _WIN32
    wchar_t* cwd;
    wchar_t buff[MAX_PATH + 1];

    cwd = _wgetcwd( buff, MAX_PATH + 1 );
    wstring ws(cwd);
    string str(ws.begin(), ws.end());
    str = str + "/";
    return str;
#else
    char* cwd;
    char buff[PATH_MAX + 1];

    cwd = getcwd( buff, PATH_MAX + 1 );
    if( cwd != NULL ) {
        printf( "My working directory is %s.\n", cwd );
    }
    return cwd;
#endif

    /*#ifdef _WIN32
    return "";
    #else
    return"../../../";
    #endif*/

    /*string filePath = getExeFilePath();
    size_t sep = filePath.find_last_of("\\/");
    string appDir = filePath.substr( 0, sep+1 );
    return appDir;*/
}

string MoMa::getLibPath( void ) {

    string appDir = getAppPath();
#ifdef NDEBUG
    cout << "Warning : you should not use getLibPath in Release mode. If the bin folder is moved, the relative path to the libs will be incorrect." << endl;
#endif
#ifdef _WIN32
    return( appDir + "../../../../libs/MoMa/" );
#else
    return( appDir +  "../../../../../../../libs/MoMa/" );
#endif
}

string MoMa::getDataPath( void ) {

    string appDir = getAppPath();
    return( appDir + "data/" );
}

string MoMa::getResPath( void ) {

#ifdef NDEBUG
    string appDir = getAppPath();
    return( appDir + "data/resources/" );
#else
    string libDir = getLibPath();
    return( libDir + "resources/" );
#endif
}