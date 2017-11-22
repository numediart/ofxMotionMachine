#ifdef __decprecated__
#ifndef __mmVideoRecorder__
#define __mmVideoRecorder__

#include <armadillo>
#include "ofMain.h"

#ifndef TARGET_LINUX

#ifdef _WIN32
#include "ofxQtVideoSaver.h"
#else
#include "ofxVideoRecorder.h"
#endif

#include "MoMa.h"

namespace MoMa {
    
    class VideoRecorder {
        
    public:
        
        VideoRecorder( void ); void setup( void );
        void setVideoFileName( string name ); // Set video name
        void startVideoRecord( string name ); // Start video recording
        void startVideoRecord( void ); // Start video recording
        void stopVideoRecord( void ); // Stop video recording
        void addFrame( ofImage &frame ); // Record this frame
        bool isRecording( void ); // Are we recording?
        
    protected:
        
        bool isVideoRecord; // Record?
        string videoFileName;
        
#ifdef _WIN32
        ofxQtVideoSaver recorder;
#else
        ofxVideoRecorder recorder;
#endif
    };
}

#endif

#endif
#endif
