#ifndef __mmSceneApp__
#define __mmSceneApp__

namespace MoMa {class SceneApp;}

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
    
    const int DefaultNodeSize = 40;
    const int DefaultGridSize = 200;
    const int DefaultViewDist = 1000;
    const int DefaultPlotRes = 1000;
    
    const string PositionHeader = "/position";
    const string RotationHeader = "/rotation";
    const string NoFeature = "No feature";
    
    class MenuView;
    class PlayBar;
    class Options;
    
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
    
    enum OscParseMode {
    
        POSITIONS,
        ROTATIONS
    };
    
    struct Plot { // Plot = one 2D curve in oF
        
        // arma::vec data; // Raw data to synchronize with
        MoMa::TimedVec data; // Raw timed data to synchronise
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
    
        Listener( void ) { mode = POSITIONS; }
        
        bool mode; // Receiving mode: positions or rotations
        std::string header; // OSC header associated with track
        Track *track; // Pointer to track to be filled with OSC
    };
    
    struct _LabelList { // Container for built-in label list
    
        _LabelList( void ) { isShown = false; } // Unshown by def
        
        LabelList *labelList; // Pointer to dynamically-created label list
        bool isShown; // Is that label list shown in annotation scene?
    };
    
    enum FeatureDim {
    
        VECTOR,
        MATRIX,
        CUBE
    };
    
    union TimedUnion {
    
        TimedVec *tvec;
        TimedMat *tmat;
        TimedCube *tcube;
    };
    
    struct _Feature {
        
        FeatureDim type;
        TimedUnion feature;
        
        std::string oscHeader;
        std::string name;
        std::string desc;
        
        bool isFeasible;
        bool isShown;
        bool isSent;
        bool isWek;
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
        virtual void windowResized(int w, int h){}
        
        virtual void dragEvent( ofDragInfo dragInfo) {}
        
        // - Specific MoMa events -
        
        virtual void onStart( void ) {}
        virtual void onPause( void ) {}
        virtual void onStop( void ) {}
        
        virtual void onReachBegin( void ) {}
        virtual void onReachEnd( void ) {}
        
        virtual void onOscReceived( void ) {}
        
        // - New 3D/2D MoMa methods -
        
        virtual void scene3d( void ) {}
        virtual void scene2d( void ) {}
        virtual void annotate( void ) {}
        
        // - Drawing methods for the 3D & 2D -
        
        void draw( const Node &node ); // Draw a node
        void draw( const Frame &frame ); // Draw a frame
        
        void draw( const TimedVec &tvec, int hue, std::string name="" ); // TimedVec
        void draw( const TimedVec &tvec, std::string name="" ); // TimedVec (no hue)
        void draw( const TimedMat &trace, std::string name="" ); // TimedMat
        
        void draw(const Trace &trace, std::string name="" ); // Trace
        
        void draw( const arma::vec &data, int hue, std::string name="" );
        void draw( const arma::vec &data, std::string name="" ); // Vec
        void draw( const arma::mat &data, string name="" ); // and Mat
        
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
        void setAutoDrawLabelLists( bool ad ) { autoDrawLabelLists = ad; }
        
        // - Built-in feature related methods -
        
        int nOfFeatures( void ) { return( feature.size() ); } // # feat
        void setAutoDrawFeatures( bool ad ) { autoDrawFeatures = ad; }
        void addNewFeature( MoMa::TimedVec &feature, std::string name="",
        std::string osc="/feat", bool isShown=false, bool isSent=false );
        void addNewFeature( MoMa::TimedMat &feature, std::string name="",
        std::string osc="/feat", bool isShown=false, bool isSent=false );
        void addNewFeature( MoMa::TimedCube &feature, std::string name="",
        std::string osc="/feat", bool isShown=false, bool isSent=false );
        
        // - Figure-related methods -
        
        int nOfFigures( void ) { return( _figure.size() ); }
        void setNumOfFigures( int nOfFigures ); // # of Figs
        void figure( int figId ); // Choose the target figure
        
        // - Timeline-related methods -
        
        void setPlaybackMode( int mode ); // Set playback mode
        void setFrameRate( float rate ); // Set playback rate
        
        void setPlayerSize( unsigned int nOfFrames ); // Define size
        void setPlayerSize( double minTime,double maxTime); // Define size in sec
        unsigned int getAppIndex( void ); // Query app index
        double getAppTime( void ); // Query app time
        
        void zoom( double tMin, double tMax ); // Zoom
        void showAll( void ); // Back to fullsize
        
        void play( void ); // Play the sequence
        void pause( void ); // Pause the sequence
        void stop( void ); // Stop the sequence
        void previousIndex( void ); // Move backward of one index
        void nextIndex( void ); // Move Forward of one index
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
        void setOscSenderPort( int senderPort );
        void sendFeaturesAsOsc( bool send );
        
        // - Viewing preferences methods -
        
        void setNodeSize( float size ); // Set node size
        void setGridSize( float size ); // Set grid size
        void setViewDistance( float dist ); // Set distance
        
        void setPlotResolution( int reso ); // Set resolution
        
        void setActiveMode( int mode ); // Set active mode
        
        void show3dScene( bool scene ); // Show 3D scene
        void showGround( bool ground ); // Show ground grid
        void showNodeNames( bool names ); // Show node names
        void showTimeTags( bool times ); // Show time tags
        void showCaptions( bool caps ); // Show time tags
        
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

        // - Built-in canvas methods -

        void addMenuView( void ); // Add menu view to the canvas
        void removeMenuView( void ); // Remove menu view to the canvas
        void addPlayerBar( void ); // Add player view to the canvas
        void removePlayerBar( void ); // Remove player view to the canvas
    
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

        virtual void windowResized(ofResizeEventArgs &resize);
        
        virtual void dragged( ofDragInfo &drag );
        
        // - Protected methods -
        
        void render2d( void );
        
        // - Visualisation types -
        
        int activeMode; // Active UI mode
        int backupMode; // Backup UI mode
        
        bool is3dScene; // Show 3D scene
        bool isGround; // Do we draw ground?
        bool isNodeNames; // Show node names
        bool isTimeTags; // Show time stamps
        bool isCaptions; // Show captions
        
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
        int plotResolution; // Plot resolution
        
        // - X axis mapping -
        
        MoMa::Moment lowBound, highBound, maxBound,minBound;
        int zoomLowBound, zoomHighBound; // Zoom
        
        // - Playback types -
        
        int playbackMode; // Playback mode flag
        float frameRate; // Frame rate from playback
        bool isPlayback; // Do we keep play it back?
        MoMa::Moment appMoment; // Current app moment
        float fAppMomentIndex; // Float moment index
        bool isBegin; // Is begin?
        
        // - Video recorder -
        
        // VideoRecorder recorder; // Video recorder
        
        // - OSC communication -
        
        vector<Listener> listener; // Registered pairs
        ofxOscReceiver _receiver; // OSC listener
        int oscRcvPort; // Listening port
        
        ofxOscSender _sender; // OSC sender
        int oscSndPort; // Sender port
        
        // - Annotation types -
        
        vector<_LabelList> _labelList; // List of annotations
        bool hasDragEventRegLabelList; // Has drag-event registered?
        LabelList *dragEventRegLabelList; // Registered label list
        bool hasMouseEventRegLabelList; // Has mouse-event registered?
        LabelList *mouseEventRegLabelList; // Registered label list
        bool autoDrawLabelLists; // Auto-draw label lists
        
        bool insertNewLabel; // Are we inserting a new label?
        bool isLabelSelected; // Is a label currently selected?
        int selectedLabelIdx; // If yes, this is its index
        bool isEditing; // Are we editing (typing text)
        int tolerance; // Tolerence for seletion
        
        // - Feature types -
        
        vector<_Feature> feature; // List of features
        bool autoDrawFeatures; // Auto-draw features
        bool sendOscFeatures; // Send features @ OSC

        // - Shortcuts control -
        
        bool isShortcut; // Shortcuts enabled?
        
        // - Built-in canvas -        
        
        MoMa::MenuView *menuView;
        MoMa::PlayBar *playBar;
        MoMa::Options *optionsView;
    };
}

#endif
