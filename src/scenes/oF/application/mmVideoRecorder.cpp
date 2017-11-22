#ifdef __decprecated__
#include "mmVideoRecorder.h"

#ifndef TARGET_LINUX

using namespace std;

MoMa::VideoRecorder::VideoRecorder( void ) {
    
    setup();
}

void MoMa::VideoRecorder::setup( void ) {
    
    videoFileName = "Untitled.mp4";
    isVideoRecord = false;
    
    #ifdef _WIN32
    
    #else
    recorder.setVideoCodec( "mpeg4" );
    recorder.setVideoBitrate( "8000k" );
    #endif
}

void MoMa::VideoRecorder::setVideoFileName( string name ) {
    
    videoFileName = name;
}

void MoMa::VideoRecorder::startVideoRecord( string name ) {
    
    setVideoFileName( name );
    startVideoRecord();
}

void MoMa::VideoRecorder::startVideoRecord( void ) {
    
    #ifdef _WIN32
    recorder.setup(ofGetWidth(), ofGetHeight(), videoFileName);
    recorder.setCodecQualityLevel(OF_QT_SAVER_CODEC_QUALITY_LOSSLESS);
    //recorder.listCodecs();    
    recorder.setCodecType(0); //0 = plan RVB, 4 = H.264, 15 = Video mpeg-4, 19 = Apple Video; defalut : 11 = photo - jpeg
    #else
    recorder.setup( videoFileName, ofGetWidth(), ofGetHeight(), 30 );
    #endif
    
    isVideoRecord = true;
}

void MoMa::VideoRecorder::stopVideoRecord( void ) {
    
    isVideoRecord = false;
    
    #ifdef _WIN32
    recorder.finishMovie(); 
    #else
    recorder.close();
    #endif
}

void MoMa::VideoRecorder::addFrame( ofImage &frame ) {
 
    #ifdef _WIN32
    recorder.addFrame( frame.getPixels(), (1/ofGetFrameRate()) );
    #else
    recorder.addFrame( frame.getPixelsRef() );
    #endif
}

bool MoMa::VideoRecorder::isRecording( void ) {
    
    return( isVideoRecord );
}

#endif
#endif
