/**
*
*  @file ofApp.cpp
*  @brief MotionMachine source file for empty example
*  @copyright Numediart Institute, UMONS (c) 2015
*
*

TUTORIAL DESCRIPTION
This is an example showing how we can easily annotate data in the MotionMachine.
The example is explained for annotation of the example data "LShoulderFlexExt.c3d".
Drag and drop this file from the folder ofxMotionMachine\mmExampleData\ to follow the example.
This code could be used with any other data file, though it is better if the files encodes the framerate,
as it is needed for correct labeling. If no framerate is provided, the default framerate of the application
will be used (100fps). You can also add manually the framerate of the track after loading it, and the framerate
of the label list at load function call.

Once you dragged the track file (LShoulderFlexExt.c3d), you can use the annotation mode.
Just place your mouse where you want to add a label, then hold the "n" key and click to create a label.
You can than select or deselect a label by clicking on it.
If a label is selected:
- you can edit its name by pressing the "enter" key, entering the name of the label in 
the GUI window, and pressing "enter" again (or clicking on the "OK" button).
- you can move it on the timeline by dragging
- you can remove it with the key "bakcspace"

You can interactively visualize the movement display in the 3D scene, or the corresponding Center of Mass feature 
extracted in this example and displayed in the 2D scene, for spotting important keyframes of your data, and add 
new labels at these moments.

When you're finished, you can save your labels in a .lab file (ascii file) by pressing "Maj+S". The label list 
is stored in bin/data directory of this tutorial project, and has the same name as the track file.

Finally, you can load an existing label list by pressing "Maj+L", and edit it (then press "Maj+S" again to save it
in bin/data with the name of the track file). Do not forgat that you should load a track before labels, otherwise 
the framerate will be defined to 100fps by default for the labels, and thus might differ from the track framerate afterwards.

You can see a simple example of label list in the file "LShoulderFlexExt.lab" in the example data folder (ofxMotionMachine\mmExampleData\).

*/

#include "ofApp.h"
#include "mmGeometry.h"

using namespace std;
using namespace arma;
using namespace MoMa;

void ofApp::setup(void) {

    setActiveMode(ANNOTATE); // Focus mode on the annotation scene (so that we can add, edit, or move, or remove labels)
    // Note than you can also press "a" to activate this mode
    setPlaybackMode(SCRUB); // It is interesting to use the scrub mode while annotating as we know all the time the correspondance with
                            //the time controlled by the mouse (for 2D and annotation scene) and with the 3D movement
                            //Note that you could also press "s" to activate this mode (working either in 2D Scene mode or Annotation mode)
    addNewTrack("1"); // Add a new track called "1" (to put the data in it) in the internal track vector of the 
                      //application
    registerDragEvent(track("1")); // Make the drag event load the dragged data in the track "1"
    registerDragEvent(myLabels); // Make the drag event load the dragged labels (.lab files) in myLabels
    registerMouseEvent(myLabels); // Activate this label list for annotation

    //setNumOfFigures(3); // Can be used to draw separately each coordinate of the center of mass
}

void ofApp::update(void) {


}

void ofApp::scene3d(void) {

    draw(track("1")[getAppTime()]); // Draw the current frame (at the current time of the application) of the track "1"
                                    // Same as :
                                    //draw(track("1").frame(getAppTime()));
}

void ofApp::scene2d(void) {

    // Example to draw some timed data in the 2D scene
    //draw(track("1")(0).position, "First node position data"); // Draw the positions (x,y,z) of the first node (node 0) 
    //of the track "1"

    draw(COM); //Draw the positions of the center of mass (extracted in dragEvent)
               //of the track "1"

    //Or draw separately each coordinate of the center of mass
    /*if (COM.nOfFrames() > 0) {
        for (int axis = 0; axis < 3; axis++) {
            figure(axis);
            draw(COM.position.elem(axis), "axis " + ofToString(axis));
        }
    }*/

}

void ofApp::annotate(void) {

    draw(myLabels);

}

void ofApp::keyPressed(int key) {


}

void ofApp::keyReleased(int key) {

    //Save a label list
    if (key == 'S' || key == '9') {

        myLabels.sort(); //By default, labels are sorted in the list by their creation time. Use sort to sort them according
                            // to their position.
        myLabels.save(getDataPath() + track("1").fileName + ".lab", EVENT); //Save the label list
    }

    //Load a label list
    if (key == 'L') {

        if (track("1").nOfFrames() <= 0) {

            std::cout << "You should load a track file before labels, as the framerate is not defined.\n";
        }

        ofFileDialogResult res;

        res = ofSystemLoadDialog("Loading Labels from File", false, getDataPath());


        if (res.bSuccess)
        {
            myLabels.clear();
            myLabels.load(res.filePath, track("1").frameRate());
        }
    }

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



    //This event is still called when a track file is dragged in the application window.
    // Here we extract the center of mass of the track (to display its trace it in the 2D layer)
    COM = Geometry::centerOfMassTrace(track("1"));
    // It is interesting to display a global movement feature such the center of mass trajectory, 
    // as it can be a good indicator for spotting important keyframes (such as zeros or local minima of any coordinate)
}

void ofApp::gotMessage(ofMessage msg) {


}
