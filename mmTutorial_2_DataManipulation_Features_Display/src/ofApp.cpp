/**
 *
 *  @file ofApp.cpp
 *  @brief MotionMachine source file for empty example
 *  @copyright Numediart Institute, UMONS (c) 2015
 *  @author Mickaël Tits
 *

 TUTORIAL DESCRIPTION
 This tutorial aims to show some example of manipulation and feature extraction on MoCap data.
 This code executes the following steps:
 - Load some noisy MoCap data in a Track "myTrack" (load a nodelist, a bonelist and the raw data).
 - Extract a part of the track "myTrack" in time and store it into "myCuttedTrack" (we keep frames 500 to 1000).
 - Filter "myCuttedTrack" and store the result in "myFilteredTrack" (mean filter with a window of 5 frames).
 - Translate "myCuttedTrack" and "myFilteredTrack" to put them side by side, onto the front of the 3D-scene.
 - Compute some features (sotred in TimedVec) : "mySpeed", the normal speed of the right hand in "myCuttedTrack", "myFilteredSpeed", 
   the normal speed of the right hand in "myFilteredTrack", and "myDoubleFilteredSpeed", the filtered version of the normal speed of 
   the right hand in "myFilteredTrack".
 - In the 3D-scene, we draw the current Frame of each Track ("myCuttedTrack" and "myFilteredTrack")
 - In the 2D-scene, we draw the three features in separated figures.
 Note: for enhanced use of the application, compile it in Release mode.
 */
#include "ofApp.h"
#include "mmSignal.h" //enables some signal processing techniques (such as filtering)
#include "mmGeometry.h" //enables some geometry techniques (such as translation)
#include "mmKinematics.h" //enables some kinematics techniques (such as speed computation)

using namespace std;
using namespace arma;
using namespace MoMa;
void ofApp::setup(void) {

    //-------------------- Load some motion data in your track  ---------------------------

    //Some noisy example (flat text file from kinect data)    
    myTrack.load(getExampleDataPath() + "kinect2.nodes"); //load corresponding nodelist (it must have the same number of
	//markers (or nodes) as in the raw data text file)
	//Same as: 
	//myTrack.load(getAppPath() + "../../ExampleData/kinect2.nodes");

    myTrack.load(getExampleDataPath() + "kinect2.bones"); //load corresponding bonelist, describing the links between nodes,
    // i.e. the skeleton structure, used for drawing, and for local coordinate representation

    myTrack.load(getExampleDataPath() + "kinect2.txt"); //load raw data from a flat text file (one line = a frame; one 
    //line has 3 x m values, 3 for x,y,z directions, and m for m markers (or nodes) )

    myTrack.setFrameRate(30); //This file is recorded from a kinect has thus a framerate around 30fps

    //-----------------------------------------------------------------------------------
       

    //------------------- Cut a part of the track in time -----------------------------------
    myTrack.subTrack(myCuttedTrack, 500, 1000); // Extract frames [500-1000] of myTrack and store them in myCuttedTrack

    // Same as:
    myCuttedTrack = myTrack; // Copy myTrack in myCuttedTrack
    myCuttedTrack.cut(500, 1000); // Cut the track to keep  frammes [500-1000]

    //Note that the initial time of myCuttedTrack is not zero, as it is delayed of 100 frames
    cout << "Initial time of myTrack: " << myTrack.position.initialTime() << endl;
    cout << "Initial time of myCuttedTrack: " << myCuttedTrack.position.initialTime() << endl; 

    //-----------------------------------------------------------------------------------


    //-------------------- Filter the noisy data  ---------------------------
    
    myFilteredTrack = meanfilter(myCuttedTrack, 5); //a mean filter with a window of 5 frames (signal processing
    //functions are available if you include "mmSignal.h", see beginning oe the file)
    
    //-----------------------------------------------------------------------------------


    //------------------ Translate the tracks ------------------------------
    
    Geometry::translate(myCuttedTrack, 3000, -500, 700); //Translate the track of 3000mm on x-axis, -500mm on y-axis, 
    // and 700mm on z-axis(supposing the unit of the track data is mm)
    
    Geometry::translate(myFilteredTrack, 3000, 500, 700); //Translate the track of 3000mm on x-axis, 500mm on y-axis, 
    // and 700mm on z-axis(supposing the unit of the track data is mm)

    //-----------------------------------------------------------------------------------


    //-------------------------- Compute some features --------------------------------------

    mySpeed = normSpeed(myCuttedTrack("RightHand"));
    myFilteredSpeed = normSpeed(myFilteredTrack("RightHand"));
    myDoubleFilteredSpeed = meanfilter(normSpeed(myFilteredTrack("RightHand")), 5);
    //Note: by default, a five-point derivative is used for the speed computation

    //-----------------------------------------------------------------------------------


    //------------------------- Initialize 2D scene figures -----------------------------

    //We want to display three features separately, so we need three figures
    setNumOfFigures(3);

    //-------------------------------------------------------------------------------------


    //------------------------ Manually initialize the player ------------------------------

    //You should always initialize the player after loading a track
    setFrameRate(myFilteredTrack.frameRate()); // Set the framerate of the application to the framerate of the track,
    //so that it is played at the real speed

    setPlayerSize(myFilteredTrack.minTime(), myFilteredTrack.maxTime()); // Set the size of the player to the size of
    //the track, so we can play the whole track

    //-----------------------------------------------------------------------------------
}
void ofApp::update(void) {


}
void ofApp::scene3d(void) {

    //Display the processed tracks
    ofSetColor(Green);
    draw(myCuttedTrack[getAppTime()]);
    ofSetColor(Blue);
    draw(myFilteredTrack[getAppTime()]);
}
void ofApp::scene2d(void) {

    //Display the features
    figure(0); //Activate the first figure for drawings
    draw(mySpeed, "Speed of unfiltered right hand");
    figure(1); //Activate the second figure for drawings
    draw(myFilteredSpeed, "Speed of filtered right hand");
    figure(2); //Activate the third figure for drawings
    draw(myDoubleFilteredSpeed, "Filtered speed of filtered right hand");
    //You can check the noticable difference between these three features...
}
void ofApp::annotate(void) {


}
void ofApp::keyPressed(int key) {


}
void ofApp::keyReleased(int key) {


}
void ofApp::mousePressed(int x, int y, int button) {


}
void ofApp::mouseReleased(int x, int y, int button) {


}
void ofApp::mouseDragged(int x, int y, int button) {


}
void ofApp::mouseMoved(int x, int y) {


}
void ofApp::windowResized(int w, int h) {


}
void ofApp::dragEvent(ofDragInfo dragInfo) {


}
void ofApp::gotMessage(ofMessage msg) {


}
