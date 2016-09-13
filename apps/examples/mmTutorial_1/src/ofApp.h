/**
 *
 *  @file ofApp.h
 *  @brief MotionMachine header file for tutorial example
 *  @copyright Numediart Institute, UMONS (c) 2015
 *  @author Mickaël Tits
 *
 */
#pragma once
#include "MoMa.h"
#include "MoMaUI.h"
#include "ofMain.h"
class ofApp : public MoMa::SceneApp {
  public:
    
    void setup( void );
    void update( void );
    
    void scene3d( void );
    void scene2d( void );
    void annotate( void );
    
    void keyPressed( int key );
    void keyReleased( int key );
    
    void mousePressed( int x, int y, int button );
    void mouseReleased( int x, int y, int button );
    void mouseDragged( int x, int y, int button );
    void mouseMoved( int x, int y );
    
    void windowResized( int w, int h );
    void dragEvent( ofDragInfo dragInfo );
    void gotMessage( ofMessage msg );

    MoMa::Track myTrack; //This is an instance (an object) of the class "MoMa::Track", in which I want to store a motion file, and that I want then to process and display
    MoMa::TimedVec myFeature; //This is an instance of the class "MoMa::TimedVec", in which I want to store a feature that I have extracted from my motion data
    
    //TimedVec is a class in which I can store a vector of (1D) data associated with a time scale (i.e. a discrete temporal signal; which is quite useful as in motion computing we are usually dealing with temporal features...)
    //For a multi-dimensional signal, you can use a TimedMat, or even a TimedCube.

    //For instance, if you want to store the speed of a joint or a marker (let's say the left hand) on each direction (x, y and z-axes), you can use a TimedMat of three rows (and the number of columns corresponds to the number of frames)
    //an example of TimedCube is simply the positions of every marker in a Track (myTrack.position is a TimedCube of dimensions (3,m,n), i.e. 3 rows ( for x, y and z-axes), m columns for the m markers in your motion data, and n slices for the n frames in your data).

    std::string featureName; //The name of my feature

    MoMa::Frame myFrame, myInitialFrame; //These are instances of the class "MoMa::Frame", in which I want to store a frame extracted from my motion data
};
