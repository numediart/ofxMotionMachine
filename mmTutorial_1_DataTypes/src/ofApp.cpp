/**
*
*  @file ofApp.cpp
*  @brief MotionMachine source file for tutorial example
*  @copyright Numediart Institute, UMONS (c) 2015
*  @author Mickaël Tits
*

This tutorial gives a first overview of what you can do with the MotionMachine framework. More detailed and advanced
tutorials are (or will be soon ;-) ) available to show more advanced use of the framework. 
Note: do not fear the length of this file, the same example could have been done in a few lines. Remember, this is a tutorial!


1) TUTORIAL DESCRIPTION

This tutorial is intended to show the main data types in the MotionMachine, designed for mocap data manipulation. Please read 
Section 3 for more details on data types. 
As an example of the use of these data types, this code executes the following processing steps:
- we load MoCap data (recorded from a MS Kinect One) into an object of the class MoMa::Track (myTrack, as declared in ofApp.h)
- we filter myTrack because the data are noisy (we use a mean filter with a window of 5 frames)
- we extract from myTrack a MoMa::Trace containing the pelvis trajectory in time (position and orientation)
- from the Trace, we extract a MoMa::TimedMat containing the positions in time of the pelvis
- we extract some time information from this TimedMat
- we extract an arma::mat containing row data of the pelvis position in time (from which we can use armadillo linear 
  algebra library!)
- we compute the Euclidean distance between the pelvis and the right hand and store the result in a MoMa::TimedVec (myFeature, 
  as declared in ofApp.h)
- we draw the results, i.e. the MoMa::Frame (myFrame, as declared in ofApp.h)) extracted from myTrack at the running time of the 
application, we draw a line linking the pelvis and the right hand for that frame, and we display the evolution in time of myFeature.
- as an example of additional interaction, when you press SHIFT+S, you can save myFeature in a standard ".csv" file. When you
  press SHIFT+X, you can save the processed track and feature (i.e. myTrack and myFeature) in an xml archive.
Note: for an enhanced use of the application, compile it in Release mode! 


2) BASIC INTERACTION FUNCTIONALITIES DESCRIPTION

In the application window, graphical commands allow control and interaction with the application.  Try them to see all their 
functionalities (double-click on the menu to develop it). Additionally, shortcuts are internally available to interact with
the application:
- "3": focus on the 3D scene layer. Whilst focused on the 3D scene, you can use the mouse to rotate, zoom and translate the
       scene (resp. left click, right click, and scroll click). If you do not have a scroll (on a laptop for instance), 
       "m" pressed + left click works too.
- "2": focus on the 2D scene layer. You can then use "s" to be in scrub mode, so that time follows the move of your mouse.
Press "p" to be back in playback mode. In playback mode, you can press the space bar to play and pause, "r" to stop.
- "a": focus on the annotation layer. (a separated tutorial will be released for the use of the annotation layer).
- "h": hide/show graphical commands.
- "z": zoom on time axis (while "z" is pressed, use your mouse to select the time zone on which you want to zoom.
- "l": show/hide timeline (useful if you are zooming on the time axis)
- "u": unzoom (reset) on time axis
Most these action can be done by using the graphical commands instead of these shortcuts (except "h", "z" and "u", for now!).


3) DATA TYPES DESCRIPTION

+ Basic motion data containers:

    - MoMa::Track: contains the MoCap data, including positions, orientations and the skeleton structure (nodes and bones).
    - MoMa::Frame: contains MoCap data (positions, orientations and skeleton structure) at a given moment.
    - MoMa::Trace: contains data of a node (or marker, or joint according to terminology) in time (positions and orientations).
    - MoMa::Node: contains data of a node/marker/joint at a given moment.

+ Timed data containers (containers for MoCap data and features):

    - MoMa::TimedVec: contains a column vector (it is important to remember that it is a column and not a row vector) as well
      as information about time, such as an initial time and a framerate, or a vector of timestamps if the framerate is
      not constant). A TimedVec can be used to store a 1-dimensional feature.
    - MoMa::TimedMat: contains a matrix of d rows (generally 3 to store position data and 4 for orientation data, but it can store
      any multidimensional feature) and n columns (n frames), endowed with additional information about time, such as an
      initial time and a framerate, or a vector of timestamps if the framerate is variable). It has generally 3 rows (x,y,z)
      for position data, and 4 rows for orientations (quaternions). A TimedMat can be used to store a multi-dimensional feature
      (of course, you could also use a std::vector<MoMa::TimedVec> which can be handy in some cases).
    - MoMa::TimedCube: contains a cube of d rows, m columns, and n slices. For instance, the positions in a MoMa::Track are
      stored in a TimedCube containing 3 rows (x,y,z), m columns (m markers/nodes), and n frames. A TimedCube can be used to store
      high-dimensional features, such as all positions, speeds, accelerations, ... of all nodes in each direction.

    Note: inside motion data containers, data are generally stored in these timed containers.

+ General data containers (using armadillo library, documentation available here : http://arma.sourceforge.net/docs.html ) :

    - arma::vec: column vector of data
    - arma::mat: matrix of data
    - arma::cube: cube of data
    - and much other data types (see documentation)

    Note: inside timed data containers, data are stored in these general containers. The use of armadillo data types directly
    enables the use of linear algebra methods as defined in armadillo documentation.


4) BRIEF NOTE ON OPENFRAMEWORKS (oF) AND MOTIONMACHINE LOGIC ( see http://openframeworks.cc/ for details):

oF allows the easy and fast development of interactive C++ applications. To that end, it incorporates the power of many different
well-known C++ libraries for image, audio, video processing and visualization, graphic interfacing, etc.
The development of an application is mainly done by defining the methods of the class "ofApp" (inheriting from the class "ofBaseApp"),
which are then automatically managed by openFrameworks:
- setup(void) is called at the beginning of the application, and can be used to initialize and process stuffs ones before
  the real-time/interactive use of the application.
- update(void) is called in loop just after the setup phase and can be used for online (i.e. real-time and interactive) processing.
- draw(void) is called in loop always just after update(void), and is used to draw stuffs on the window of the application
- other methods are used to manage events such as mouse clicks, keyboards inputs, dragged data, etc. to interact with the application.
For details, please refer to openFrameworks website.

In the case of MotionMachine, the design is similar. The difference lies in the fact that the class "ofApp" that you must define inherits
from the class "MoMa::SceneApp", which is en extended version of "ofBaseApp"). The main methods automatically managed by SceneApp are:
- setup(void)
- update(void)
- scene3d(void) which is used for drawing 3D stuffs (such as a Frame, a Node, or geometrical forms) in the 3D scene of the application
- scene2d(void) which is used for drawing 2D stuffs (such as the evolution of a feature in time) in the 2D scene of the application
- annotate(void) which is used for drawing labels and other stuffs in the annotation scene of the application
- other methods to manage events for interaction with the application

In the application window, a layered view of the 3D scene, the 2D scene and the annotation scene is automatically managed.

In brief, the calling order of main methods is:
setup, update, scene3d, scene2d, annotate, update, scene3d, scene2d, annotate, update, ...
This order can be interrupted at any time by event methods (keypressed, mousemoved, etc.).

As a final word, we can summarize by telling that the power of the MotionMachine framework arises in the combination of openFrameworks 
and armadillo functionalities, extended with new data types, and new interaction possibilities, dedicated to motion data visualization
and processing.
*/

#include "ofApp.h"
#include "mmGeometry.h" //enable to use some geometry functions
#include "mmSignal.h" //enable to use some signal processing functions
#include "mmXmlArchiver.h" //enable to save processed data in an xml archive

using namespace std;
using namespace arma;
using namespace MoMa;
void ofApp::setup(void) { //setup() is called at the beginning of the application and can be used to initialize
    //and process stuffs ones before running

    //-------------------- Load some motion data in your track  ---------------------------

    //Some noisy example (flat text file from kinect data)

    myTrack.load(getExampleDataPath() + "kinect2.nodes"); //load corresponding nodelist (it must have the same number 
    //of markers (or nodes) as in the raw data text file)
	//Same as: 
	//myTrack.load(getAppPath() + "../../ExampleData/kinect2.nodes");

    myTrack.load(getExampleDataPath() + "kinect2.bones"); //load corresponding bonelist, describing the links between 
    //nodes, for drawing, and for local coordinate representation

    myTrack.load(getExampleDataPath() + "kinect2.txt"); //load raw data from a flat text file (one line = a frame; 
    //one line has 3 x m values, 3 for x,y,z directions, and m for m markers (or nodes) )

    myTrack.setFrameRate(30); //This file is recorded from a kinect has thus a framerate around 30fps
    //Filter these noisy data:

    myTrack = meanfilter(myTrack, 5); //a mean filter with a window of 5 frames (signal processing functions
    //are available if you include "mmSignal.h", see beginning of the file)

    //You can comment the line above (the filtering) to use and display original unfiltered data


    //Some better example (bvh)
    /*myTrack.load(getDataPath() + "SolomDance.bvh"); //getDataPath() leads to the folder /data/ of your project
    myTrack.localToGlobal(); //Ensure data in the track are in global coordinates for the sake of the example
    //below (the "SolomDance.bvh" example is actually in local, so we convert data to global coordinates)
    */


    //Some other example (v3d)
    /*myTrack.load(getDataPath() + "taichi.v3d");*/


    //-----------------------------------------------------------------------------------


    //-------------------- Extract some data from the track --------------------------------

    Trace myPelvisTrace = myTrack("Pelvis"); //extract the trace of the marker "Pelvis" from mytrack
    TimedMat myPelvisPosition = myPelvisTrace.position; //extract the timed matrix containing positions of the pelvis

    // ---- Information about time is contained in the timed matrix and can be accessed this way:

    vec myTimestamps;
    float myFps;
    float myInitialTime;
    if (myPelvisPosition.isTimed()) { //If the timed matrix is timestamped (generally when your data have 
        //a variable framerate)

        myTimestamps = myPelvisPosition.getTimeVec(); //the timestamps of the timed matrix
        //Which is the same as
        myTimestamps = myTrack.position.getTimeVec(); //the timestamps of the timed cube containing the position
        //data of the track
    }
    else { //If it is assumed that your data have a fixed framerate

        myFps = myPelvisPosition.frameRate();
        myInitialTime = myPelvisPosition.initialTime();

        //Which are the same as
        myFps = myTrack.frameRate();
        //or
        myFps = myTrack.position.frameRate();
        myInitialTime = myTrack.position.initialTime();
    }

    // ---- The position data are themselves stored in the timed matrix as an arma::mat, which is a type of matrix
    //from the armadillo library (http://arma.sourceforge.net/docs.html)

    mat myPelvisMat = myPelvisPosition.getData();

    //If you want to do algebra on your data (see armadillo documentation here : http://arma.sourceforge.net/docs.html)
    //, it is useful to extract the armadillo matrix (arma::mat) from the the timed matrix (MoMa::TimedMat) object.
    //In this object, you still have the data corresponding to the position of the pelvis, but you have no information
    //about time. 
    //I suggest you to check armadillo documentation to see how to use it, and what you can do with it. FYI, there is
    //a table of correspondance between Matlab/Octave syntax and armadillo syntax here:
    //(http://arma.sourceforge.net/docs.html#syntax)

    //In the same way, a timed vector (MoMa::TimedVec) contains an arma::vec which is a type of (column!) vector from
    //armadillo library, and a timed cube (MoMa::TimedCube) contains an arma::cube which is a type of cube from 
    //armadillo library.

    //-----------------------------------------------------------------------------------


    // ------------- Process example: let's compute the distance between the right hand and the pelvis --------------------------------

    //Firstly, we extract the right hand data from the track
    mat myLHandMat = myTrack("RightHand").position.getData();

    //Let's then compute the distance: sqrt( (x_lhand - x_pelvis)^2 + (y_lhand - y_pelvis)^2 + (z_lhand - z_pelvis)^2 )

    //For information, the position matrices have 3 rows (x,y,z) and n columns (n frames)
    rowvec tmp = sqrt(pow(myLHandMat.row(0) - myPelvisMat.row(0), 2) +
        pow(myLHandMat.row(1) - myPelvisMat.row(1), 2) + pow(myLHandMat.row(2) - myPelvisMat.row(2), 2));
    //or
    tmp = sqrt(sum(pow(myLHandMat - myPelvisMat, 2)));

    //We have here a row, but we want a column vector, so we transpose it:
    vec myDistance = tmp.t();

    //More simply, if you include the file "mmGeometry.h" (see beginning of this file), you can use a direct function
    //to compute the distance between pelvis and right hand:
    myDistance = Geometry::distance(myLHandMat, myPelvisMat);

    //We can now store this vector in our object myFeature (which is a MoMa::TimedVec):
    if (myTrack.position.isTimed())
        myFeature.setData(myTimestamps, myDistance); //if variable framerate
    else
        myFeature.setData(myFps, myDistance, myInitialTime); //if fixed framerate

    //by the way, there is also a geometry function for timed matrices, returning a timed vector; so you could actually do
    //all the above in one line!:
    myFeature = Geometry::distance(myTrack("Pelvis").position, myTrack("RightHand").position);


    //name of the feature
    featureName = "Distance RightHand-Pelvis";

    //-----------------------------------------------------------------------------------


    //------------------------ Manually initialize the player ------------------------------

    //You should always initialize the player after loading a track
    setFrameRate(myTrack.frameRate()); // Set the framerate of the application to the framerate of the track,
    //so that it is played at the real speed

    setPlayerSize(myTrack.nOfFrames()); // Set the size of the player to the size of the track, so we can play
    //the whole track

    //-----------------------------------------------------------------------------------
}
void ofApp::update(void) { //this function is called in loop, and allows to process stuffs online (in real-time)

    //---- We can extract any frame of our track in the 3D scene. For instance, let's extract the first frame 
    //(frame(0)) to display it then in scene3d()

    myInitialFrame = myTrack.frame(0.0f); //frame at time 0 second (supposing that the initial time of the track is 0
    //or
    myInitialFrame = myTrack.frame(0u); //frame at index 0
    //or
    myInitialFrame = myTrack[0.0f]; //Note: Brackets allow to extract a frame from a track, parentheses allow to 
    //extract a trace

    //---- We can also extract the current frame (at the current time of the application) to display it in the
    //3D scene (which is generally more useful...)

    myFrame = myTrack[getAppTime()];


}
void ofApp::scene3d(void) { //this function is called in loop (just after update()), and allows to display stuffs
    //in the 3D scene of the application

    //---- We can display our frames in the 3D scene
    ofSetColor(0, 0, 255, 50);//Set drawing color to blue, with transparency of 50
    draw(myInitialFrame); //This function allows to actually draw the frame

    //---- We translate the coordinate system of oF of 3m on x-axis
    ofPushMatrix();
    ofTranslate(3000, 0, 0);
    //"ofPushMatrix()" and "ofPopMatrix()" are beacons between with we can locally modify the coordinate system 
    //with "ofTranslate(x,y,z)". This translation have no more effect on the code following "ofPopMatrix()".
        
    ofSetColor(0, 255, 0);//Set drawing color to green
    draw(myFrame); //Draw myFrame, i.e. the frame at the running time of the application 

    //---- We can also draw any openframeworks-like stuff. For instance, let's draw a line linking the pelvis and
    //the right hand

    Node LHand = myTrack("RightHand")[getAppTime()]; //position data of the right hand at the current time of the
    //application

    Node Pelvis = myTrack("Pelvis")[getAppTime()]; //position data of the pelvis at the current time of the 
    //application

    vec LHandPositionVector = LHand.position;
    vec PelvisPositionVector = Pelvis.position;
    ofSetColor(255, 0, 0); //Set drawing color to red
    ofLine(toVec3f(LHandPositionVector), toVec3f(PelvisPositionVector)); //draw a line between two points

    ofPopMatrix(); //End of the local coordinate system effect
}
void ofApp::scene2d(void) { //this function is called in loop (just after scene3d()), and allows to display stuffs 
    //in the 2D scene of the application

    //---- We can display our feature in the 2D scene
    draw(myFeature, featureName);
}
void ofApp::annotate(void) {


}
void ofApp::keyPressed(int key) {

    if (key == 'S') { //press Shift + S to save the feature in a csv file

        vec myFeatureVec = myFeature.getRefData();
        myFeatureVec.save(getDataPath() + "myFeature.csv", csv_ascii);//You can read this with any text editor, 
        //speadsheet editor, matlab, ...
    }
    else if (key == 'X') { //press Shift + X to save the processed track and the feature in an xml archive

        XmlArchiver myArchive; //Available if you include "mmXmlArchiver.h" (see beginning of the file)
        myArchive.addTrack(myTrack);
        myArchive.addFeature(myFeature, featureName, myTrack.fileName);
        myArchive.save(getDataPath() + "myArchive.xml"); //You can then load this archive in any other MoMa application
    }
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
