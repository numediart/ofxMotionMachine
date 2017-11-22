/**
 *
 *  @file main.cpp
 *  @brief MotionMachine main file for empty example
 *  @copyright Numediart Institute, UMONS (c) 2015
 *
 */

#include "ofMain.h"
#include "ofApp.h"

int main( void ) {
    
    /*#if defined(TARGET_LINUX) || defined(TARGET_ANDROID)
		char buff[FILENAME_MAX];
		ssize_t size = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
		if (size == -1){
			cout << "getAppPath(): readlink failed with error " << errno;
		}
		else{
			buff[size] = '\0';

			//return buff;
		}
	#elif defined(TARGET_OSX)
		char path[FILENAME_MAX];
		uint32_t size = sizeof(path);
		if(_NSGetExecutablePath(path, &size) != 0){
			cout << "getAppPath(): path buffer too small, need size " <<  size;
		}
		//return path;
	#elif defined(TARGET_WIN32)
		vector<char> executablePath(MAX_PATH);
		DWORD result = ::GetModuleFileNameA(NULL, &executablePath[0], static_cast<DWORD>(executablePath.size()));
		if(result == 0) {
			cout << "getAppPath(): couldn't get path, GetModuleFileNameA failed";
		}else{
            string filePath = string(executablePath.begin(), executablePath.begin() + result);
            cout << filePath;
            size_t sep = filePath.find_last_of("\\/");
            string dirPath = filePath.substr( 0, sep+1 );
            cout << dirPath << endl;

			//return string(executablePath.begin(), executablePath.begin() + result);
		}
	#endif
	//return "";
    system("pause");*/
	ofSetupOpenGL( 1200, 800, OF_WINDOW );
	ofRunApp( new ofApp() );
}
