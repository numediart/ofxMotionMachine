#ifndef __mmSceneApp__
#define __mmSceneApp__

#include <armadillo>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxUI.h"

#include "MoMa.h"
#include "mmCam.h"
#include "mmOfBone.h"
#include "mmOfUtils.h"
#include "mmCanvas.h"

namespace MoMa {

    class Canvas;
    
    // -- Data structres used in SceneApp --
    
    enum InteractionMode {
        
        // User interaction on SceneApp can be different:
        // - SCENE3D: clicks and keys affect the 3D scene
        // - SCENE2D: clicks and keys affect the 2D figures
        // - ANNOTATE: clicks and keys affect the annotation
        // - CANVAS: clicks affect UI canvas (automatic)
    
        SCENE3D,
        SCENE2D,
        ANNOTATE,
        CANVAS
    };
    
    enum PlaybackMode {
        
        // The SceneApp playback head can behave differently:
        // - PLAY: like a player, start and stop on space bar
        // - SCRUB: position of the mouse = position in the file
        // - STREAM: just displays latest frame (for ringbuffer)
    
        PLAY,
        SCRUB,
        STREAM
    };
    
    struct Plot { // Plot = one 2D curve in oF
        
        arma::vec data; // Raw data to synchronize with
        ofPolyline line; // Actual polyline to draw in oF
        std::string name; // Name of the curve ( if any )
        ofColor color; // Color of the curve in oF
    };
    
    struct Figure { // Figure = one 2D timeline in oF
        
        Figure( void ) { yMin = -1.0f; yMax = 1.0f; hue = 150; }
    
        vector<Plot> plot; // Collection of plotted graphs
        
        float yMin, yMax; // Data min/max for 1 collection
        float yTop, yBot; // Top/bottom positions on screen
        
        int hue; // Local hue for color circle
        int plotId; // Current ID to be drawn
    };
    
    struct _Track { // Container for built-in track
        
        _Track( void ) { isShown = true; } // Show by def
        
        Track *track; // Pointer to dynamically-created track
        bool isShown; // Is that track shown in 3D scene?
    };
    
    struct Listener { // Container for OSC-exposed track
    
        std::string header; // OSC header associated with track
        Track *track; // Pointer to track to be filled with OSC
    };
    
    struct _LabelList { // Container for built-in label list
    
        _LabelList( void ) { isShown = false; } // Unshown by def
        
        LabelList *labelList; // Pointer to dynamically-created label list
        bool isShown; // Is that label list shown in annotation scene?
    };
    
    // -- Shortcuts to SceneApp skeleton colors --
    
    static const ofColor Turquoise = ofColor( 79, 178, 248 );
    static const ofColor DarkTurquoise = ofColor( 50, 120, 200 );    
    static const ofColor Green = ofColor( 0, 255, 0 );
    static const ofColor Red = ofColor( 255, 0, 0 );
    
    // -- SceneApp --

    class SceneApp : public ofBaseApp {

      public:
        
        // - Exposed oF methods -
        
        virtual void setup( void ) {}
        virtual void update( void ) {}
        virtual void draw( void ) {}
        virtual void exit( void ) {}
        
        virtual void keyPressed( int key ) {}
        virtual void keyReleased( int key ) {}
        
        virtual void mousePressed( int x, int y, int button ) {}
        virtual void mouseReleased( int x, int y, int button ) {}
        virtual void mouseDragged( int x, int y, int button ) {}
        virtual void mouseMoved( int x, int y, int button ) {}
        
        virtual void dragEvent( ofDragInfo dragInfo) {}
        
        // - Specific MoMa events -
        
        virtual void onStart( void ) {}
        virtual void onPause( void ) {}
        virtual void onStop( void ) {}
        
        virtual void onReachBegin( void ) {}
        virtual void onReachEnd( void ) {}
        
        // - New 3D/2D MoMa methods -
        
        virtual void scene3d( void ) {}
        virtual void scene2d( void ) {}
        virtual void annotate( void ) {}
        
        // - Drawing methods for the 3D & 2D -
        
        void draw( Node node ); // Draw a node
        void draw( Frame frame ); // Draw a frame
        
        void draw( arma::vec data, int hue, std::string name="" );
        void draw( arma::vec data, std::string name="" ); // Vec
        
        void draw( arma::mat data, string name="" ); // Mat and
        void draw( Trace trace, std::string name="" ); // Trace
        
        void draw( LabelList labelList ); // Label list
        
        // - Built-in track related methods -
        
        int nOfTracks( void ) { return( _track.size() ); }
        void setNumOfTracks( int nOfTracks ); // # of Tracks
        void addNewTrack( std::string name="", bool isShown=true );
        Track &track( std::string name ); // Choose the track by name
        Track &track( int index ); // Choose the track by index
        void showTrack( int trackId, bool show ); // Change status
        bool isTrackShown( int trackId ); // Check if track shown
        void registerDragEvent( Track &trck ); // Drag event
        
        // - Built-in label list related methods -
        
        int nOfLabelLists( void ) { return( _labelList.size() ); }
        void setNumOfLabelLists( int nOfLabelLists ); // # of Label lists
        void addNewLabelList( std::string name="", bool isShown=true );
        LabelList &labelList( std::string name ); // Choose the label list by name
        LabelList &labelList( int index ); // Choose the label list by index
        void showLabelList( int labelListId, bool show ); // Change status
        bool isLabelListShown( int labelListId ); // Check if label list shown
        void registerMouseEvent( LabelList &labList ); // Register mouse event
        void registerDragEvent( LabelList &labList ); // Register drag event
        
        // - Figure-related methods -
        
        int nOfFigures( void ) { return( _figure.size() ); }
        void setNumOfFigures( int nOfFigures ); // # of Figs
        void figure( int figId ); // Choose the target figure
        
        // - Timeline-related methods -
        
        void setPlaybackMode( int mode ); // Set playback mode
        void setFrameRate( float rate ); // Set playback rate
        
        // TODO replace setTrackSize() by setPlayerSize()
        
        void setPlayerSize( int size ); // Define track size
        int getAppIndex( void ); // Query app index
        
        void zoom( int iMin, int iMax ); // Zoom
        void showAll( void ); // Back to fullsize
        
        void play( void ); // Play the sequence
        void pause( void ); // Pause the sequence
        void stop( void ); // Stop the sequence
        bool isPlaying( void ); // Is playing?
        
        // - Screencasting-related methods -
        
        /*
        void setVideoFileName( string name ); // Set video name
        void startVideoRecord( string name ); // Start recording
        void startVideoRecord( void ); // Start video recording
        void stopVideoRecord( void ); // Stop video recording
        bool isRecording( void ); // Are we recording?
        */
        
        // - OSC-related methods -
        
        void addOscListener( std::string header, Track &track );
        void setOscListenerPort( int listenerPort );
        
        // - Viewing preferences methods -
        
        void setNodeSize( float size ); // Set node size
        void setGridSize( float size ); // Set grid size
        void setViewDistance( float dist ); // Set distance
        
        void setActiveMode( int mode ); // Set active mode
        
        void show3dScene( bool scene ); // Show 3D scene
        void showGround( bool ground ); // Show ground grid
        void showNodeNames( bool names ); // Show node names
        void showTimeTags( bool times ); // Show time tags
        
        void showAnnotation( bool annot ); // Show annotations
        void showCurtain( bool curtain ); // Show 3D curtain
        void showFigures( bool figures ); // Show 2D figures
        void showTimeline( bool time ); // Show timeline

        // - Shortcuts control methods -

        void enableShortcuts( void ); // Enable shortcuts
        void disableShortcuts( void ); // Disable shortcuts
        
        // - Misc methods -
        
        std::string getAppPath( void ); // App path
        std::string getLibPath( void ); // Lib path
    
      //protected:
        
        // TODO make the protected protected again
        
        // - Overridden event-based methods -
    
        virtual void setup( ofEventArgs &args );
        virtual void update( ofEventArgs &args );
        virtual void draw( ofEventArgs &args );
        virtual void exit( ofEventArgs &args );
        
        virtual void keyPressed( ofKeyEventArgs & key );
        virtual void keyReleased( ofKeyEventArgs & key );
        
        virtual void mousePressed( ofMouseEventArgs &mouse );
        virtual void mouseReleased( ofMouseEventArgs &mouse );
        virtual void mouseDragged( ofMouseEventArgs &mouse );
        virtual void mouseMoved( ofMouseEventArgs &mouse );
        
        virtual void dragged( ofDragInfo &drag );
        
        // - 2D rendering method -
        
        void render2d( void );
        
        // - Visualisation types -
        
        int activeMode; // Active UI mode
        int backupMode; // Backup UI mode
        
        bool is3dScene; // Show 3D scene
        bool isGround; // Do we draw ground?
        bool isNodeNames; // Show node names
        bool isTimeTags; // Show time stamps
        
        bool isAnnotation; // Show annotations
        bool isCurtain; // Show 3D curtain
        bool isFigure; // Show 2D figures
        bool isTimeline; // Show timeline
        bool isZoom; // Are we in zoom?
        
        bool isReach3D; // Are we reaching 3D?
        int beforeReachMode; // = Mode before it
        
        // - 3D drawing types -
        
        mmCam camera; // Default camera
        ofLight light; // Default light point
        float nodeSize; // Skeleton node size
        float gridSize; // Skeleton node size
        
        vector<_Track> _track; // List of tracks
        bool hasDragEventRegTrack; // Has registered?
        Track *dragEventRegTrack; // Registered track
        
        // - 2D drawing types -
        
        bool is2D; // Check drawing context
        vector<Figure> _figure; // List of figures
        int figureIdx; // Figure to be drawn in
        
        // - X axis mapping -
        
        int idxMin, idxMax, playerSize; // x values
        int zoomLowBound, zoomHighBound; // Zoom
        
        // - Playback types -
        
        int playbackMode; // Playback mode flag
        float frameRate; // Frame rate from playback
        bool isPlayback; // Do we keep play it back?
        float fAppIndex; // Float index
        int appIndex; // App index
        bool isBegin; // Is begin?
        
        // - Video recorder -
        
        // VideoRecorder recorder; // Video recorder
        
        // - OSC communication -
        
        vector<Listener> listener; // Registered pairs
        ofxOscReceiver receiver; // OSC listener
        int oscRcvPort; // Listening port
        
        // - Annotation types -
        
        vector<_LabelList> _labelList; // List of annotations
        bool hasDragEventRegLabelList; // Has drag-event registered?
        LabelList *dragEventRegLabelList; // Registered label list
        bool hasMouseEventRegLabelList; // Has mouse-event registered?
        LabelList *mouseEventRegLabelList; // Registered label list
        
        //bool hasRegisteredAnnotationDragEvent; // Has registered?
        //LabelList *registeredLabelList; // Registered label list
        
        //LabelList labelList; // TMP: current label list
        
        bool insertNewLabel; // Are we inserting a new label?
        bool isLabelSelected; // Is a label currently selected?
        int selectedLabelIdx; // If yes, this is its index
        bool isEditing; // Are we editing (typing text)
        int tolerance; // Tolerence for seletion

        // - Shortcuts control -
        
        bool isShortcut; // Shortcuts enabled?
        
        // - Built-in canvas -
        
        MoMa::Canvas *menuView;
    };
}

#endif
