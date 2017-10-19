/**
 *
 *  @file ofApp.cpp
 *  @brief MotionMachine source file for empty example
 *  @copyright Numediart Institute, UMONS (c) 2015
 *
 *

TUTORIAL DESCRIPTION
This is a short example showing how to enable the dragging of motion data in the application and their display.
*/

#include "ofApp.h"
#include "mmGeometry.h"

using namespace std;
using namespace arma;
using namespace MoMa;

void ofApp::setup( void ) {    
    
    setActiveMode(SCENE3D); // Focus mode on the 3D scene (so that the mouse controls the 3D scene)
    addNewTrack("1"); // Add a new track called "1" (to put the data in it) in the internal track vector of the 
    //application
    registerDragEvent(track("1")); // Make the drag event load the dragged data in the track "1"
}

void ofApp::update( void ) {
    
    
}

void ofApp::scene3d( void ) {
    
    draw(track("1")[getAppTime()]); // Draw the current frame (at the current time of the application) of the track "1"
    // Same as :
    //draw(track("1").frame(getAppTime()));
}

void ofApp::scene2d( void ) {
    
    // Example to draw some timed data in the 2D scene
    //draw(track("1")(0).position, "First node position data"); // Draw the positions (x,y,z) of the first node (node 0) 
                                                                //of the track "1"

    draw(COM); //Draw the positions of the center of mass (extracted in dragEvent)
               //of the track "1"
}

void ofApp::annotate( void ) {
    
    
}

void ofApp::keyPressed( int key ) {
    
    
}

void ofApp::keyReleased( int key ) {
    
    
}

void ofApp::mousePressed( int x, int y, int button ) {
    
    
}

void ofApp::mouseReleased( int x, int y, int button ) {
    
    
}

void ofApp::mouseDragged( int x, int y, int button ) {
    
    
}

void ofApp::mouseMoved( int x, int y ) {
    
    
}

void ofApp::windowResized( int w, int h ) {
    
    
}

void ofApp::dragEvent( ofDragInfo dragInfo ) {
    
    //This event is still called when a track file is dragged in the application window.
    // Here we extract the center of mass of the track (to display its trace it in the 2D layer)
    COM = Geometry::centerOfMassTrace(track("1"));
}

void ofApp::gotMessage( ofMessage msg ) {
    
    
}
