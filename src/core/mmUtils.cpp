/**
*
*  @file mmUtils.cpp
*  @brief MotionMachine source file for Utils.
*  @author Mickael Tits
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmUtils.h"

#ifndef _WIN32
#include "unistd.h"
#endif

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

string MoMa::getAbsoluteAppPath( void ) {

#ifdef _WIN32
    wchar_t* cwd;
    wchar_t buff[MAX_PATH + 1];

    cwd = _wgetcwd( buff, MAX_PATH + 1 );
    wstring ws(cwd);
    //wcout << "wide string : " << ws << endl;
    string str(ws.begin(), ws.end());
    //cout << "string : " << str << endl;
    str = str + "/";

    //cout << "appPath : " << str << endl;
    //Uncomment if you have bugs with special ASCII characters
    //checkFilePath(str);

    //cout << "Corrected appPath : " << str << endl;

    return str;
#else
    char* cwd;
    char buff[PATH_MAX + 1];

    cwd = getcwd( (char*)buff, (size_t)(PATH_MAX + 1) );
    
    if( cwd != NULL ) {
        
        // printf( "My working directory is %s.\n", cwd );
        
        string str( cwd );
#ifdef  __APPLE__
        str = str + "/../../../";
#else // __unix__ or __linux__
		str = str + "/";
#endif        
        return str;
        
    } else {
    
        string str = "";
        return str;
    }
#endif


    /*string filePath = getExeFilePath();
    size_t sep = filePath.find_last_of("\\/");
    string appDir = filePath.substr( 0, sep+1 );
    return appDir;*/
}

string MoMa::getAppPath( void ) {

#ifdef _WIN32
    return "";
#else
#ifdef  __APPLE__
    return"../../../";
#else

    return "";
#endif
#endif
}

string MoMa::getLibPath( void ) {

    string appDir = getAppPath();
#ifdef NDEBUG
    cout << "Warning : you should not use getLibPath nor getAbsoluteLibPath in Release mode. If the bin folder is moved, the relative path to the libs will be incorrect." << endl;
#endif
#ifdef  __APPLE__
    return( appDir +  "../../../../../../../libs/MoMa/" );
#else
    return( appDir + "../../../../libs/MoMa/" );
#endif
}

string MoMa::getAbsoluteLibPath( void ) {

    string appDir = getAbsoluteAppPath();
#ifdef NDEBUG
    cout << "Warning : you should not use getLibPath nor getAbsoluteLibPath in Release mode. If the bin folder is moved, the relative path to the libs will be incorrect." << endl;
#endif
#ifdef  __APPLE__
    return( appDir +  "../../../../libs/MoMa/" );
#else
    return( appDir + "../../../../libs/MoMa/" );
#endif
}

string MoMa::getDataPath( void ) {

    string appDir = getAppPath();
    return( appDir + "data/" );
}

string MoMa::getExampleDataPath(void) {

	string appDir = getAbsoluteAppPath();
	return(appDir + "../../../../addons/ofxMotionMachine/mmExampleData/");
}

string MoMa::getAbsoluteDataPath( void ) {

    string appDir = getAbsoluteAppPath();
    return( appDir + "data/" );
}

//unsafe to use
/*string MoMa::getResPath( void ) {

#ifdef NDEBUG
    string appDir = getAppPath();
    return( appDir + "data/resources/" );
#else
    string libDir = getLibPath();
    return( libDir + "resources/" );
#endif
}*/

string MoMa::getAbsoluteResPath( void ) {

/*#ifdef NDEBUG*/
    string appDir = getAbsoluteAppPath();
    return( appDir + "data/resources/" );
/*#else
    string libDir = getAbsoluteLibPath();
    return( libDir + "resources/" );
#endif*/
}

void MoMa::correctPath( string &path, bool invert ) {

    string tmp;

    // Manage special characters (accents
    // for example) in file path (extended ASCII table)
    if(!invert) {

        for( int i=0; i<path.size(); ++i ) {

            if( path[i] != -61 ) {

                // Basic ASCII table
                tmp.push_back( path[i] );

            } else {

                // Manage buggy special characters (extended ASCII character) the
                // special character is divided in two characters : -61 and X (between
                // -1 and -127); to get a valid character the conversion is c = X+64

                ++i; // tmp[i] was -61
                tmp.push_back( path[i]+64 );
            }
        }
    }
    else {

        for( int i=0; i<path.size(); ++i ) {

            if( path[i] >= 0 ) {

                // Basic ASCII table
                tmp.push_back( path[i] );

            } else {

                // Negative ASCII character

                tmp.push_back(-61);
                tmp.push_back( path[i]-64 );
            }
        }
    }

    path.clear();
    path = tmp;
}

void MoMa::checkFilePath( string &path ) {

    ifstream fPath( path.c_str() );

    if(!fPath) {

        correctPath( path );
        fPath = ifstream( path.c_str() );

        if(!fPath) {

            correctPath( path, true );
            fPath = ifstream( path.c_str() );

            if(!fPath) {

                cout << "The following path does not exists or is not accessible : " << path << endl;
            }
        }
    }
}

string MoMa::getExtension(string filePath) {

    size_t dot = filePath.find_last_of(".");
    string extension = filePath.substr(dot + 1);
    transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    return extension;
}

string MoMa::getName(string filePath) {

    size_t sep = filePath.find_last_of("\\/");
    size_t dot = filePath.find_last_of(".");
    return filePath.substr(sep + 1, dot - sep - 1); //length of dot - sep - 1
}

string MoMa::getFolder(string filePath) {

    size_t sep = filePath.find_last_of("\\/");
    return filePath.substr(0,sep+1); //length of sep+1
}
