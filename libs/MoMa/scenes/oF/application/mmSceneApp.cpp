#include "mmSceneApp.h"

using namespace std;
using namespace arma;

void MoMa::SceneApp::setup( ofEventArgs &args ) {

    ofSetFrameRate( 30 );
    ofSetVerticalSync( false );
    ofBackground( 255, 255, 255 );
    ofSetColor( 160, 160, 160 );

    ofEnableAlphaBlending();
    ofEnableAntiAliasing();
    ofEnableSmoothing();
    
    setNodeSize( DefaultNodeSize );
    setGridSize( DefaultGridSize );
    setViewDistance( DefaultViewDist );
    setPlotResolution( DefaultPlotRes );

    showGround( true );
    show3dScene( true );
    showNodeNames( false );
    showTimeTags( false );
    enableShortcuts();

    setActiveMode( SCENE2D );
    setPlaybackMode( PLAY );

    showAnnotation( true );
    showCurtain( false );
    showFigures( true );
    showTimeline( false );

    isPlayback = false;
    frameRate = 100.0f;
    isBegin = true;
    
    lowBound.index = 0;
    highBound.index = 1;
    appAtPos.index = 0;

    lowBound.time = 0.0f;
    highBound.time = 1.0f;
    appAtPos.time = 0.0f;
    
    zoomLowBound = -1;
    zoomHighBound = -1;
    isZoom = false;
    
    isReach3D = false;

    figureIdx = 0; // We start with 1
    _figure.resize( 1 ); // full-screen
    _figure[figureIdx].yTop = 0; // figure
    _figure[figureIdx].yBot = ofGetHeight();

    hasDragEventRegTrack = false;

    oscRcvPort = 7000; setup();
    receiver.setup( oscRcvPort );

    insertNewLabel = false;
    isLabelSelected = false;
    isEditing = false;
    tolerance = 5;
}

void MoMa::SceneApp::update( ofEventArgs &args ) {

    switch( activeMode ) {
            
        case SCENE3D:
            
            camera.enableMouseInput();
            
            break;
            
        case SCENE2D:
            
            camera.disableMouseInput();
            
            break;
            
        case ANNOTATE:
            
            camera.disableMouseInput();
            
            break;
            
        case CANVAS:
            
            camera.disableMouseInput();
            
            break;
    }

    switch( playbackMode ) {
            
        case SCRUB: {
            
            if( activeMode == SCENE2D || activeMode == ANNOTATE ) {
                
                appAtPos.index = ofMap( ofGetMouseX(), 0, ofGetWidth(), lowBound.index, highBound.index, true );
                appAtPos.time = ofMap( ofGetMouseX(), 0, ofGetWidth(), lowBound.time, highBound.time, true );
            }
            
            break;
        }
            
        case STREAM: {
            
            appAtPos.index = highBound.index;
            appAtPos.time = highBound.time;
            
            break;
        }
            
        case PLAY: {
            
            if( ofGetElapsedTimef() > 3.0f ) {
                
                double timeStep = 1.0f / ofGetFrameRate();
                
                if( isPlayback ) {
                    
                    appAtPos.time = appAtPos.time + timeStep;
                    appAtPos.index = getIndexFromTime( appAtPos.time );
                    
                    if( isBegin ) {
                        
                        onReachBegin();
                        isBegin = false;
                    }
                    
                    if( appAtPos.time > highBound.time ) {
                        
                        appAtPos.time = highBound.time; // Set to max first
                        appAtPos.index = getIndexFromTime( appAtPos.time );
                        
                        onReachEnd();
                        
                        appAtPos.time = lowBound.time; // Then set to min
                        appAtPos.index = getIndexFromTime( appAtPos.time );
                        
                        isBegin = true;
                    }
                }
            }
            
            break;
        }
    }
    
    // ---

    while( receiver.hasWaitingMessages() ) {
        
        ofxOscMessage message;
        receiver.getNextMessage( &message );

        for( int l=0; l<listener.size(); l++ ) {
            
            Frame oscFrame;
            bool hasReceivedFrame = false;

            if( message.getAddress() == listener[l].header+PositionHeader ) {
                
                oscFrame.setRotationFlag( false );
                listener[l].track->hasRotation = false;
                
                if( listener[l].mode == ROTATIONS ) {
                
                    listener[l].mode = POSITIONS;
                    listener[l].track->clear();
                }
                
                int nOfNodes = message.getNumArgs()/3;

                for( int n=0; n<nOfNodes; n++ ) {

                    float x = message.getArgAsFloat( 3*n );
                    float y = message.getArgAsFloat( 3*n+1 );
                    float z = message.getArgAsFloat( 3*n+2 );

                    Node nod( x, y, z );
                    oscFrame.push( nod );
                }
                
                hasReceivedFrame = true;
            }
            
            if( message.getAddress() == listener[l].header+RotationHeader ) {
                
                oscFrame.setRotationFlag( true );
                listener[l].track->hasRotation = true;
                
                if( listener[l].mode == POSITIONS ) {
                    
                    listener[l].mode = ROTATIONS;
                    listener[l].track->clear();
                }
                
                int nOfNodes = message.getNumArgs()/7;
                
                for( int n=0; n<nOfNodes; n++ ) {
                    
                    float x = message.getArgAsFloat( 3*n );
                    float y = message.getArgAsFloat( 3*n+1 );
                    float z = message.getArgAsFloat( 3*n+2 );
                    float rx = message.getArgAsFloat( 3*n+3 );
                    float ry = message.getArgAsFloat( 3*n+4 );
                    float rz = message.getArgAsFloat( 3*n+5 );
                    float rw = message.getArgAsFloat( 3*n+6 );
                    
                    Node nod( x, y, z, rx, ry, rz, rw );
                    oscFrame.push( nod );
                }
                
                hasReceivedFrame = true;
            }
            
            if( hasReceivedFrame ) {
            
                listener[l].track->push( oscFrame );
                setPlayerSize( listener[l].track->nOfFrames() );
                onOscReceived(); // Trigger custom code here
            }
        }
    }

    update();
}

void MoMa::SceneApp::draw( ofEventArgs &args ) {

    is2D = false;
    camera.begin();

    ofEnableDepthTest();

    if( is3dScene && isGround ) {
        
        ofPushStyle();
        ofSetLineWidth( 1 );
        ofSetColor( 210, 210, 210 );
        ofPushMatrix(); ofRotate(90, 0, 1, 0);
        ofDrawGridPlane( 4000.0f, gridSize );
        ofPopMatrix();
        ofPopStyle();
    }

    ofEnableSmoothing();
    ofEnableLighting();
    light.enable();

    if( is3dScene ) {
        
        scene3d();
    }

    light.disable();
    ofDisableLighting();
    ofDisableSmoothing();

    ofDisableDepthTest();

    camera.end();
    is2D = true;

    if( isCurtain ) {

        ofPushStyle();
        ofSetColor( 255, 255, 255, 180 );
        ofRect( 0, 0, ofGetWidth(), ofGetHeight() );
        ofPopStyle();
    }

    if( isZoom ) {

        ofPushStyle();
        ofSetColor( Turquoise, 30 );
        ofRect( zoomLowBound, 0, zoomHighBound-zoomLowBound, ofGetHeight() );
        ofPopStyle();
    }

    if( isFigure ) {

        _figure[figureIdx].yMin = -1.0f;
        _figure[figureIdx].yMax = 1.0f;

        _figure[figureIdx].plot.clear();
        _figure[figureIdx].plotId = 0;

        scene2d(); // 2D figures
        render2d(); // Render
    }

    if( isAnnotation ) {

        annotate(); // Annotations
    }
    
    if( isTimeline ) {
        
        float selLowBound = 0;
        float selHighBound = 0;
        float absPlaybackPos = 0;
        
        if( maxBound.index > 0 ) {
        
            selLowBound = ofMap( lowBound.index, 0, maxBound.index, 1.5f, ofGetWidth()-0.5f );
            selHighBound = ofMap( highBound.index, 0, maxBound.index, 1.5f, ofGetWidth()-0.5f );
            absPlaybackPos = ofMap( appAtPos.index, 0, maxBound.index, 0, ofGetWidth() );
        }
        
        ofPushStyle();
        
        ofSetColor( 255, 128 ); ofFill();
        ofRect( -1, ofGetHeight()-80, ofGetWidth()+2, 70 );
        
        ofSetColor( 200, 150 ); ofNoFill();
        ofRect( -1, ofGetHeight()-80, ofGetWidth()+2, 70 );
        
        ofSetColor( Turquoise, 40 ); ofFill();
        ofRect( selLowBound, ofGetHeight()-82,
        selHighBound-selLowBound, 74 );
        
        ofSetColor( Turquoise, 128 ); ofNoFill();
        ofRect( selLowBound, ofGetHeight()-82,
        selHighBound-selLowBound, 74 );
        
        ofSetColor( Turquoise, 200 );
        ofLine( absPlaybackPos, ofGetHeight()-82,
        absPlaybackPos, ofGetHeight()-8 );
        
        ofPopStyle();
    }
    
    // TODO Remove this, this is temporary
    
    string drawActiveMode;
    string drawPlaybackMode;
    
    if( activeMode == SCENE3D ) drawActiveMode = "Active Mode = SCENE3D";
    else if( activeMode == SCENE2D ) drawActiveMode = "Active Mode = SCENE2D";
    else if( activeMode == ANNOTATE ) drawActiveMode = "Active Mode = ANNOTATE";
    else if( activeMode == CANVAS ) drawActiveMode = "Active Mode = CANVAS";
    
    ofPushStyle(); ofSetColor( 200 );
    ofDrawBitmapString( drawActiveMode, 10, 20 );
    ofPopStyle(); // Draw the current active mode
    
    if( playbackMode == PLAY ) drawPlaybackMode = "Playback = PLAY";
    else if( playbackMode == SCRUB ) drawPlaybackMode = "Playback = SCRUB";
    else if( playbackMode == STREAM ) drawPlaybackMode = "Playback = STREAM";
    
    ofPushStyle(); ofSetColor( 200 );
    ofDrawBitmapString( drawPlaybackMode, 10, 40 );
    ofPopStyle(); // Draw the current player mode

    draw();

    /*
    if( isRecording() ) {

        ofImage oneWindow;
        oneWindow.grabScreen( 0, 0, ofGetWidth(), ofGetHeight() );
        recorder.addFrame( oneWindow );
    }
    */
}

void MoMa::SceneApp::exit( ofEventArgs &args ) {

    for( int k=0; k<_track.size(); k++ ) {

        delete _track[k].track;
    }

    Canvas::deleteCanvas();

    exit();
}

void MoMa::SceneApp::keyPressed( ofKeyEventArgs &key ) {

    if( isShortcut ) { 

        if( !isEditing ) {
            
            if( key.key == 'p' ) playbackMode = PLAY; // 'p' like 'play'
            if( key.key == 's' ) playbackMode = SCRUB; // 's' like 'scrub'
            if( key.key == 'o' ) playbackMode = STREAM; // 'o' like 'osc'
            
            if( key.key == 't' && isReach3D == false ) { // 't' like 'through'
                
                beforeReachMode = activeMode;
                activeMode = SCENE3D;
                isReach3D = true;
            }
            
            if( key.key == 'h' ) {
                
                // TODO In this case, it doesn't restore the previous view
                // but get us back to the main view, i.e. new children.
                
                if( Canvas::canvasOpened() ) Canvas::closeAllCanvas();
                else Canvas::openMainCanvas(); // Show/hide the canvas
            }
            
            if( key.key == '3' ) setActiveMode( SCENE3D ); // '3' like '3D scene'
            if( key.key == '2' ) setActiveMode( SCENE2D ); // '2' like '2D scene'
            if( key.key == 'a' ) setActiveMode( ANNOTATE ); // 'a' like 'annotate'
            
            if( key.key == 'l' ) isTimeline = !isTimeline; // Show/hide timeline
        }
        
        switch( activeMode ) {
                
            case SCENE3D:
                
                break;
                
            case SCENE2D:
                
                if( key.key == 'z' ) isZoom = true; // 'z' like 'zoom'
                if( key.key == 'u' ) showAll(); // 'u' like 'unzoom'
                
                break;
                
            case ANNOTATE:
                
                if( !isEditing ) {
                
                    if( key.key == 'z' ) isZoom = true; // 'z' like 'zoom'
                    if( key.key == 'u' ) showAll(); // 'u' like 'unzoom'
                }
                
                if( hasMouseEventRegLabelList && isLabelSelected ) {
                    
                    if( key.key == OF_KEY_BACKSPACE ) {
                        
                        if( isEditing ) {
                            
                            if( (*mouseEventRegLabelList)[selectedLabelIdx].name.size() > 0 ) {
                            
                                (*mouseEventRegLabelList)[selectedLabelIdx].name.resize(
                                (*mouseEventRegLabelList)[selectedLabelIdx].name.size()-1 );
                            }
                        
                        } else {
                            
                            (*mouseEventRegLabelList).erase( (*mouseEventRegLabelList).begin()+selectedLabelIdx );
                            isLabelSelected = false;
                        }
                        
                    } else if( key.key == OF_KEY_RETURN ) {
                        
                        if( isEditing ) {
                            
                            (*mouseEventRegLabelList)[selectedLabelIdx].state = UNSELECTED;
                            isLabelSelected = false;
                            isEditing = false;
                            
                        } else {
                            
                            // TODO Rewire the logic to take system text box into account
                            
                            (*mouseEventRegLabelList)[selectedLabelIdx].name
                            = ofSystemTextBoxDialog( "Enter label name:" );
                            // isEditing = true;
                        }
                        
                    } else if( isEditing ) {
                        
                        (*mouseEventRegLabelList)[selectedLabelIdx].name.push_back( key.key );
                    }
                    
                } else if ( key.key == 'n' ) { // 'n' like 'new label'
                    
                    insertNewLabel = true;
                }
                
                break;
        }
        
        if( playbackMode == PLAY ) {
            
            if( key.key == ' ' ) { // space like most play/pause
                
                if( isPlaying() ) pause();
                else play(); // Switch it
            }
            
            if( key.key == 'r' ) { // 'r' like 'reset'
                
                stop();
            }
            
            if ( !isPlaying() ) {
                
                if ( key.key == OF_KEY_LEFT ) {
                    
                    appAtPos.index--; // Decrement and update time
                    appAtPos.time = getTimeFromIndex( appAtPos.index );
                }
                
                if ( key.key == OF_KEY_RIGHT ) {
                    
                    appAtPos.index++; // Increment and update time
                    appAtPos.time = getTimeFromIndex( appAtPos.index );
                }
            }
        }
        
        keyPressed( key.key );
    }
}

void MoMa::SceneApp::keyReleased( ofKeyEventArgs &key ) {

    if( isShortcut ) {
        
        if( !isEditing ) {
            
            if( key.key == 't' && isReach3D == true ) { // 't' like 'through'
                
                activeMode = beforeReachMode;
                isReach3D = false;
            }
        }

        switch( activeMode ) {
                
            case SCENE3D:
                
                break;
                
            case SCENE2D:
                
                if( key.key == 'z' ) isZoom = false;
                
                break;
                
            case ANNOTATE:
                
                if( !isEditing ) {
                    
                    if( key.key == 'z' ) isZoom = false;
                    if( key.key == 'n' ) insertNewLabel = false;
                }
                
                break;
        }

        keyReleased( key.key );
    }
}

void MoMa::SceneApp::mousePressed( ofMouseEventArgs &mouse ){

    switch( activeMode ) {
            
        case SCENE3D:
            
            break;
            
        case SCENE2D:
            
            if( isZoom ) {
                
                zoomLowBound = mouse.x;
                zoomHighBound = mouse.x;
            }
            
            break;
            
        case ANNOTATE:
            
            if( isZoom ) {
                
                zoomLowBound = mouse.x;
                zoomHighBound = mouse.x;
            }
            
            if( hasMouseEventRegLabelList ) {
                
                bool newLabel = true;
                
                for( int l=0; l<(*mouseEventRegLabelList).size(); l++ ) {
                    
                    if( (*mouseEventRegLabelList)[l].state == HOVERED ) {
                        
                        (*mouseEventRegLabelList)[l].state = SELECTED;
                        isLabelSelected = true;
                        selectedLabelIdx = l;
                        newLabel = false;
                        
                    } else if( (*mouseEventRegLabelList)[l].state == SELECTED ) {
                        
                        (*mouseEventRegLabelList)[l].state = UNSELECTED;
                        isLabelSelected = false;
                        newLabel = false;
                    }
                }
                
                if( insertNewLabel == true && newLabel == true ) {
                    
                    // TODO Label times could be saved as seconds
                    // (*mouseEventRegLabelList).push_back( Label( ofMap( mouse.x, 0, ofGetWidth(), lowBound.time, highBound.time ), "new" ) );
                    (*mouseEventRegLabelList).push_back( Label( ofMap( mouse.x, 0, ofGetWidth(), lowBound.index, highBound.index ), "new" ) );
                    
                    selectedLabelIdx = (*mouseEventRegLabelList).size()-1;
                    (*mouseEventRegLabelList).back().state = SELECTED;
                    isLabelSelected = true;
                }
            }
            
            break;
    }
    
    mouseX = mouse.x; mouseY = mouse.y;
    mousePressed( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::mouseReleased( ofMouseEventArgs &mouse ) {

    switch( activeMode ) {
            
        case SCENE3D:
            
            break;
            
        case SCENE2D:
            
            if( isZoom ) {
                
                zoomHighBound = mouse.x;
                
                int newMin = ofMap( zoomLowBound, 0, ofGetWidth(), lowBound.index, highBound.index );
                int newMax = ofMap( zoomHighBound, 0, ofGetWidth(), lowBound.index, highBound.index );
                
                zoomLowBound = zoomHighBound = -1;
                zoom( newMin, newMax );
            }
            
            break;
            
        case ANNOTATE:
            
            if( isZoom ) {
                
                zoomHighBound = mouse.x;
                
                int newMin = ofMap( zoomLowBound, 0, ofGetWidth(), lowBound.index, highBound.index );
                int newMax = ofMap( zoomHighBound, 0, ofGetWidth(), lowBound.index, highBound.index );
                
                zoomLowBound = zoomHighBound = -1;
                zoom( newMin, newMax );
            }
            
            break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mouseReleased( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::mouseDragged( ofMouseEventArgs &mouse ) {

    switch( activeMode ) {
            
        case SCENE3D:
            
            break;
            
        case SCENE2D:
            
            if( isZoom ) {
                
                zoomHighBound = mouse.x;
            }
            
            break;
            
        case ANNOTATE:
            
            if( isZoom ) {
                
                zoomHighBound = mouse.x;
            }
            
            if( hasMouseEventRegLabelList && isLabelSelected ) {
                
                // TODO Handle time here too
                // (*mouseEventRegLabelList)[selectedLabelIdx].time = ofMap(
                // mouse.x, 0, ofGetWidth(), lowBound.time, highBound.time );
                (*mouseEventRegLabelList)[selectedLabelIdx].idx = ofMap(
                mouse.x, 0, ofGetWidth(), lowBound.index, highBound.index );
            }
            
            break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mouseDragged( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::mouseMoved( ofMouseEventArgs &mouse ) {

    switch( activeMode ) {
            
        case SCENE3D:
            
            break;
            
        case SCENE2D:
            
            break;
            
        case ANNOTATE:
            
            if( hasMouseEventRegLabelList ) {
                
                for( int l=0; l<(*mouseEventRegLabelList).size(); l++ ) {
                    
                    int pixLabel = ofMap( (*mouseEventRegLabelList)[l].idx,
                    lowBound.index, highBound.index, 0, ofGetWidth() );
                    
                    if( (*mouseEventRegLabelList)[l].state != SELECTED ) {
                        
                        if( mouse.x > pixLabel-tolerance
                        && mouse.x < pixLabel+tolerance ) {
                            
                            (*mouseEventRegLabelList)[l].state = HOVERED;
                            
                        } else {
                            
                            (*mouseEventRegLabelList)[l].state = UNSELECTED;
                        }
                    }
                }
            }
            
            break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mouseMoved( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::dragged( ofDragInfo &drag ) {

    if( hasDragEventRegTrack ) {

        deque<string> trackFileNames;

        for( int k=0; k<drag.files.size(); k++ ) {

            string trackFileName = drag.files[k];
            string ext = trackFileName.substr( trackFileName.find_last_of(".") + 1 );
            
            // Load data files first to avoid invalidity of nodes/bones
            if( ext == "txt" || ext == "v3d" || ext == "c3d" || ext == "bvh" ) {

                trackFileNames.push_front( trackFileName );

            } else if( ext == "nodes" || ext == "bones" ) {

                trackFileNames.push_back( trackFileName );
            }
        }

        for( int k=0; k<trackFileNames.size(); k++ ) {

            dragEventRegTrack->load( trackFileNames[k] );
        }

        setPlayerSize( dragEventRegTrack->nOfFrames() );
        setFrameRate( dragEventRegTrack->frameRate() );
    }
    
    if( hasDragEventRegLabelList ) {
        
        deque<string> labFileNames;
        
        for( int k=0; k<drag.files.size(); k++ ) {
            
            string labFileName = drag.files[k];
            string ext = labFileName.substr( labFileName.find_last_of(".") + 1 );
            
            if( ext == "lab" ) {
                
                dragEventRegLabelList->load( labFileName );
                
            }
        }
    }

    dragEvent( drag );
}

unsigned int MoMa::SceneApp::getIndexFromTime( double time ) {
    
    return( (unsigned int)( time * frameRate ) );
}

double MoMa::SceneApp::getTimeFromIndex( unsigned int idx ) {

    return( (double)idx / frameRate );
}

void MoMa::SceneApp::render2d( void ) {

    for( int fIdx=0; fIdx<_figure.size(); fIdx++ ) {

        float shift; // Values
        bool lowTextLoc = false;
        float nIndex = 23; // Names
        
        // -- Display plot names --

        for( int f=0; f<_figure[fIdx].plot.size(); f++ ) {

            ofPushStyle();

            ofSetColor( _figure[fIdx].plot[f].color ); // Grab name from plot name field
            ofDrawBitmapString( _figure[fIdx].plot[f].name, 13, _figure[fIdx].yTop+nIndex );

            ofPopStyle();

            nIndex += 14;
        }
        
        // -- Display zero line --
        
        ofPushStyle();
        
        ofSetLineWidth( 1.5f ); ofSetColor( 160, 160, 160, 128 );
        float zLn = ofMap( 0, _figure[fIdx].yMin, _figure[fIdx].yMax, _figure[fIdx].yBot-5, _figure[fIdx].yTop+5 );
        ofLine( 0, zLn, ofGetWidth(), zLn );

        ofPopStyle();
        
        // -- Display figures --

        for( int f=0; f<_figure[fIdx].plot.size(); f++ ) {

            _figure[fIdx].plot[f].line.clear(); // Clear first
            
            for( unsigned int n=0; n<_figure[fIdx].plot[f].data.nOfFrames(); n++ ) {
                
                float x = ofMap( _figure[fIdx].plot[f].data.time( n ), lowBound.time, highBound.time, 0, ofGetWidth() ); // Apply screen mapping here
                float y = ofMap( _figure[fIdx].plot[f].data.at( n ), _figure[fIdx].yMin, _figure[fIdx].yMax, _figure[fIdx].yBot-5, _figure[fIdx].yTop+5 );

                _figure[fIdx].plot[f].line.addVertex( ofVec2f( x, y ) ); // Add vertex
            }

            ofPushStyle();

            ofSetLineWidth( 1.5f );
            ofSetColor( _figure[fIdx].plot[f].color ); _figure[fIdx].plot[f].line.draw();

            ofPopStyle();
        }
        
        // -- Display playback head --
        
        ofPushStyle();
        
        double appAtTime = getAppTime(); // Sync fig time reading with application time
        float appTimeX = ofMap( appAtTime, lowBound.time, highBound.time, 0, ofGetWidth(), true );
        
        ofSetLineWidth( 1.5f ); ofSetColor( 160, 160, 160, 128 ); // Draw PB time line
        ofLine( appTimeX, 0, appTimeX, ofGetHeight() ); // horizontal line & value @ bottom
        ofDrawBitmapString( ofToString( appAtTime ), appTimeX+6, _figure[fIdx].yBot-5 );

        ofPopStyle();

        for( int f=0; f<_figure[fIdx].plot.size(); f++ ) {
            
            // Compute plot-specific index at which we can grab plot value
            float appAtIdx = appAtTime * _figure[fIdx].plot[f].data.frameRate(); // TODO : put that in timed container
            
            ofPoint pnt = _figure[fIdx].plot[f].line.getPointAtIndexInterpolated( appAtIdx ); // Apply screen mapping here
            float value = ofMap( pnt.y, _figure[fIdx].yBot-5, _figure[fIdx].yTop+5, _figure[fIdx].yMin, _figure[fIdx].yMax );

            if( lowTextLoc == false ) { shift = -3.0f; lowTextLoc = true; }
            else { shift = +11.0f; lowTextLoc = false; } // Alternating

            ofPushStyle();

            ofSetColor( _figure[fIdx].plot[f].color ); ofSetLineWidth( 1.5f );
            ofDrawBitmapString( ofToString( value ), pnt.x+6, pnt.y+shift );
            ofNoFill(); ofCircle( pnt, 5 ); // Bubble and text

            ofPopStyle();
        }
    }
}

void MoMa::SceneApp::draw( Node node ) {

    if( node.hasRotation() ) {

        ofBoxPrimitive box; box.set( nodeSize );

        box.setPosition( toVec3f( node.position ) );
        box.setOrientation( toVec3f( node.rotation ) );

        box.draw();

    } else {

        ofSpherePrimitive sphere; sphere.setRadius( nodeSize/2 );
        sphere.setPosition( toVec3f( node.position ) );

        sphere.draw();
    }

    string tag = "";
    if( isNodeNames ) tag  = node.name();
    if( node.hasTime() && isTimeTags ) tag
    += ( "(" + ofToString( node.time() ) + ")" );

    ofPushStyle();
    ofSetColor( ofGetStyle().color, 120 ); // Keep color but add alpha
    ofDrawBitmapString( tag, toVec3f( node.position ) + nodeSize/1.5f );
    ofPopStyle();
}

void MoMa::SceneApp::draw( TimedVec tvec, int hue, std::string name ) {
    
    if( is2D ) {
        
        Plot plot;
        
        plot.data = tvec; plot.name = name;
        plot.color.setHsb( hue, 255, 255, 128 );
        
        if( tvec.nOfFrames() > 1 ) {
            
            if( _figure[figureIdx].yMin > (float)tvec.getData().min() ) _figure[figureIdx].yMin = (float)tvec.getData().min();
            if( _figure[figureIdx].yMax < (float)tvec.getData().max() ) _figure[figureIdx].yMax = (float)tvec.getData().max();
            
            _figure[figureIdx].plot.push_back( plot );
            _figure[figureIdx].plotId++;
        }
    }
}

void MoMa::SceneApp::draw( TimedVec tvec, std::string name ) {
    
    if( is2D ) {
    
        _figure[figureIdx].hue = ( 150 + 40*_figure[figureIdx].plotId )%255;
        draw( tvec, _figure[figureIdx].hue, name ); // Automatic hue
    }
}

void MoMa::SceneApp::draw( TimedMat tmat, string name ) {
    
    if( is2D ) {
    
        for( int r=0; r<tmat.nOfElems(); r++ ) {
        
            TimedVec tvec = tmat.elem( r );
            draw( tvec, name + " ( " + ofToString( r ) + " )" );
        }
    }
}

void MoMa::SceneApp::draw( Trace trace, string name ) {
    
    if( is2D ) {
    
        draw( trace.position, trace.name() + " ( position )" );
        
        if( trace.hasRotation() ) {
        
            draw( trace.rotation, trace.name() + " ( rotation )" );
        }
    }
    
    /*
     if( is2D ) {
     
     // TODO: to manage the case with timestamped value for X-Axis.
     
     mat traceMtx = trace.getPosition();
     if( trace.hasRotation() ) traceMtx = join_rows(traceMtx, trace.getRotation());
     
     draw( traceMtx, trace.name() );
     
     } else {
     
     int nMin = idxMin+1; if( nMin < 1 ) nMin = 1; // We adjust trace to zoom range
     int nMax = idxMax+1; if( nMax > trace.nOfFrames() ) nMax = trace.nOfFrames();
     
     for( unsigned int n=nMin; n<nMax; n++ ) {
     
     ofPushStyle();
     ofSetColor( ofGetStyle().color, 128 ); ofSetLineWidth( 1 );
     ofLine( toVec3f( trace.getFramePosition(n-1) ), toVec3f( trace.getFramePosition(n) ) );
     ofPopStyle();
     }
     }
     */
}

void MoMa::SceneApp::draw( vec data, int hue, string name ) {
    
    TimedVec tvec;
    
    tvec.setData( frameRate, data );
    draw( tvec, hue, name );
    
    /*
     Plot plot;
     
     plot.data = data; plot.name = name;
     plot.color.setHsb( hue, 255, 255, 128 );
     
     if( data.n_rows > 1 ) {
     
     // It doesn't make sense to adjust min/max if there is
     // not at least two points in the vector -> div by zero!
     
     if( _figure[figureIdx].yMin > (float)data.min() ) _figure[figureIdx].yMin = (float)data.min();
     if( _figure[figureIdx].yMax < (float)data.max() ) _figure[figureIdx].yMax = (float)data.max();
     }
     
     _figure[figureIdx].plot.push_back( plot );
     _figure[figureIdx].plotId++;
     */
}

void MoMa::SceneApp::draw( vec data, string name ) {
    
    TimedVec tvec;
    
    tvec.setData( frameRate, data );
    draw( tvec, name );
    
    /*
     _figure[figureIdx].hue = ( 150 + 40*_figure[figureIdx].plotId )%255;
     draw( data, _figure[figureIdx].hue, name );
     */
}

void MoMa::SceneApp::draw( mat data, string name ) {
    
    TimedMat tmat;
    
    tmat.setData( frameRate, data );
    draw( tmat, name );
    
    /*
     if( is2D ) {
     
     for( int r=0; r<data.n_rows; r++ ) {
     
     vec row = data.row( r ).t(); // Slice mat into vecs
     draw( row, name+ " ( " + ofToString( r ) + " )" );
     }
     }
     */
}

void MoMa::SceneApp::draw( LabelList labelList ) {
/*
    for( int l=0; l<labelList.size(); l++ ) {

        ofPushStyle();

        float labelHue = ofGetStyle().color.getHue();

        if( labelList[l].state == UNSELECTED ) {

            ofColor unselected; unselected.setHsb( labelHue, 128, 255 );
            ofSetColor( unselected, 180 ); ofSetLineWidth( 1.2 );

        } else if ( labelList[l].state == HOVERED ) {

            ofColor hovered; hovered.setHsb( labelHue, 180, 255 );
            ofSetColor( hovered, 180 ); ofSetLineWidth( 3 );

        } else if (labelList[l].state == SELECTED) {

            ofColor selected; selected.setHsb( labelHue, 240, 255 );
            ofSetColor( selected, 200 ); ofSetLineWidth( 3 );
        }
        
        float labPos = ofMap( labelList[l].idx, lowBound.index, highBound.index, 0, ofGetWidth() );
        ofLine( labPos, 0, labPos, ofGetHeight() ); // We draw labels line and label names
        ofDrawBitmapString( labelList[l].name, labPos+6, 14 );
        
        ofPopStyle();
    }
*/
}

void MoMa::SceneApp::draw( Frame frame ) {

    if( frame.hasBoneList ) {

        for( int b=0; b<frame.boneList->size(); b++ ) {

            ofVec3f beg = toVec3f( frame.node( frame.boneList->at( b ).first ).position );
            ofVec3f end = toVec3f( frame.node( frame.boneList->at( b ).second ).position );

            ofSetLineWidth( 2 );
            ofLine( beg, end );
        }
    }

    if( frame.hasRotation() ) {

        vector<ofBoxPrimitive> box;
        box.resize( frame.nOfNodes() );

        for( int b=0; b<box.size(); b++ ) {

            box[b].setPosition( toVec3f( frame.node(b).position ) );
            box[b].setOrientation( toQuaternion( frame.node(b).rotation ) );
            box[b].set( nodeSize ); // Set position, rotation and radius

            box[b].draw();
        }
        if( frame.hasBoneList ){
            vector<ofBone> bones;
            bones.resize( frame.boneList->size() );

            for( int b=0; b<bones.size(); b++ ) {
                float s=arma::norm(frame.node( frame.boneList->at( b ).first ).position -frame.node( frame.boneList->at( b ).second ).position );
                bones[b].setScale(s, 1, 1);
                bones[b].setPosition( toVec3f( frame.node( frame.boneList->at( b ).first ).position ) );
                bones[b].setOrientation( toQuaternion( frame.node( frame.boneList->at( b ).second ).rotationOffset  ) *toQuaternion( frame.node( frame.boneList->at( b ).first ).rotation  ));//
                bones[b].draw();
            }
        }

    } else {

        vector<ofSpherePrimitive> sphere;
        sphere.resize( frame.nOfNodes() );

        for( int s=0; s<sphere.size(); s++ ) {

            sphere[s].setPosition( toVec3f( frame.node(s).position ) );
            sphere[s].setRadius( nodeSize/2 ); // Set position and radius

            sphere[s].draw();
        }
    }

    for( int n=0; n<frame.nOfNodes(); n++ ) {

        string tag  = ""; if( frame.hasNodeList && isNodeNames ) tag = frame.nodeList->at( n );
        if( frame.hasTime() && isTimeTags && n==0 ) tag += ( "(" + ofToString( frame.time() ) + ")" );

        ofPushStyle();
        ofSetColor( ofGetStyle().color, 120 ); // We keep the color but make it transparent
        ofDrawBitmapString( tag, toVec3f( frame.node(n).position ) + nodeSize/1.5f );
        ofPopStyle();
    }
}

void MoMa::SceneApp::setNumOfTracks( int nOfTracks ) {

    for( int k=0; k<_track.size(); k++ ) {

        delete _track[k].track;
    }

    _track.clear(); // New vec
    _track.resize( nOfTracks );

    for( int k=0; k<_track.size(); k++ ) {

        _track[k].track = new Track();
    }
}

void MoMa::SceneApp::addNewTrack( std::string name, bool isShown ) {

    _Track _tr;
    
    _tr.isShown = isShown;
    _tr.track = new Track();
    _tr.track->setName( name );
    
    _track.push_back( _tr );
}

MoMa::Track &MoMa::SceneApp::track( std::string name ) {
    
    int kFound = -1;
    bool isFound = false;
    
    for( int k=0; k<_track.size(); k++ ) {
        
        if( _track[ k ].track->easyName.compare( name ) == 0 ) {
            
            isFound = true;
            kFound = k;
        }
    }
    
    // TODO Very bad way to do this: memory leak!!!
    
    if( isFound ) return( *(_track[ kFound ].track) );
    else return( *( new Track() ) ); // Make decision
}

MoMa::Track &MoMa::SceneApp::track( int index ) {

    if( index > 0 && index < _track.size() ) return( *(_track[ index ].track) );
    else return( *( new Track() ) ); // TODO Very bad way to do this: memory leak!!!
}

void MoMa::SceneApp::registerDragEvent( MoMa::Track &trck ) {

    hasDragEventRegTrack = true;
    dragEventRegTrack = &trck;
}

void MoMa::SceneApp::showTrack( int trackId, bool show ) {

    _track[ trackId ].isShown = show;
}

bool MoMa::SceneApp::isTrackShown( int trackId ) {

    return( _track[ trackId ].isShown );
}

void MoMa::SceneApp::setNumOfLabelLists( int nOfLabelLists ) {
    
    _labelList.clear(); // New vec
    _labelList.resize( nOfLabelLists );
    
    for( int k=0; k<_labelList.size(); k++ ) {
        
        _labelList[k].labelList = new LabelList();
    }
}

void MoMa::SceneApp::addNewLabelList( std::string name, bool isShown ) {
    
    _LabelList _ll;
    
    _ll.isShown = isShown;
    _ll.labelList = new LabelList();
    _ll.labelList->name = name;
    
    _labelList.push_back( _ll );
    
    registerMouseEvent( *(_labelList.back().labelList) );
}

MoMa::LabelList &MoMa::SceneApp::labelList( std::string name ) {
    
    int kFound = -1;
    bool isFound = false;
    
    for( int k=0; k<_labelList.size(); k++ ) {
        
        if( _labelList[ k ].labelList->name.compare( name ) == 0 ) {
            
            isFound = true;
            kFound = k;
        }
    }
    
    // TODO Very bad way to do this: memory leak!!!
    
    if( isFound ) return( *(_labelList[ kFound ].labelList) );
    else return( *( new LabelList() ) ); // Make decision
}

MoMa::LabelList &MoMa::SceneApp::labelList( int index ) {
    
    if( index > 0 && index < _labelList.size() ) return( *(_labelList[ index ].labelList) );
    else return( *( new LabelList() ) ); // TODO Very bad way to do this:  leak!!!
}

void MoMa::SceneApp::registerMouseEvent( LabelList &labList ) {
    
    hasMouseEventRegLabelList = true;
    mouseEventRegLabelList = &labList;
}

void MoMa::SceneApp::registerDragEvent( LabelList &labList ) {
    
    hasDragEventRegLabelList = true;
    dragEventRegLabelList = &labList;
}

void MoMa::SceneApp::showLabelList( int labelListId, bool show ) {
    
    _labelList[ labelListId ].isShown = show;
}

bool MoMa::SceneApp::isLabelListShown( int labelListId ) {
    
    return( _labelList[ labelListId ].isShown );
}

void MoMa::SceneApp::setNumOfFigures( int nOfFigures ) {

    _figure.clear(); // New vector
    _figure.resize( nOfFigures );

    float figWidth = (float)ofGetHeight() / (float)nOfFigures;

    for( int k=0; k<_figure.size(); k++ ) {

        _figure[k].yTop = (float)k * figWidth;
        _figure[k].yBot = (float)(k+1) * figWidth;
    }
}

void MoMa::SceneApp::figure( int figId ) {

    figureIdx = figId;

    _figure[figureIdx].yMin = -1.0f;
    _figure[figureIdx].yMax = 1.0f;

    _figure[figureIdx].plot.clear();
    _figure[figureIdx].plotId = 0;
}

void MoMa::SceneApp::setPlaybackMode( int mode ) {

    playbackMode = mode;
}

/*
void MoMa::SceneApp::setVideoFileName( string name ) {

    recorder.setVideoFileName( name );
}

void MoMa::SceneApp::startVideoRecord( string name ) {

    recorder.startVideoRecord( name );
}

void MoMa::SceneApp::startVideoRecord( void ) {

    recorder.startVideoRecord();
}

void MoMa::SceneApp::stopVideoRecord( void ) {

    recorder.stopVideoRecord();
}

bool MoMa::SceneApp::isRecording( void ) {

    return( recorder.isRecording() );
}
*/

void MoMa::SceneApp::setFrameRate( float rate ) {

    frameRate = rate;
}

void MoMa::SceneApp::setPlayerSize( unsigned int nOfFrames ) {

    maxBound.index = nOfFrames;
    highBound.index = nOfFrames-1;
    
    if( highBound.index < 1 ) highBound.index = 1;
    
    highBound.time = getTimeFromIndex( highBound.index );
    
    if( appAtPos.index > highBound.index ) appAtPos.index = highBound.index;
    appAtPos.time = getTimeFromIndex( appAtPos.index ); // Check app time
}

void MoMa::SceneApp::setPlayerSize( double time ) {
    
    setPlayerSize( getIndexFromTime( time ) );
}

unsigned int MoMa::SceneApp::getAppIndex( void ) {

    return( appAtPos.index );
}

double MoMa::SceneApp::getAppTime( void ) {
    
    return( appAtPos.time );
}

void MoMa::SceneApp::play( void ) {

    isPlayback = true;
    onStart();
}

void MoMa::SceneApp::pause( void ) {

    isPlayback = false;
    onPause();
}

void MoMa::SceneApp::stop( void ) {

    appAtPos.index = lowBound.index;
    appAtPos.time = getTimeFromIndex( appAtPos.index );
    
    isPlayback = false;
    isBegin = true;
    onStop();
}

bool MoMa::SceneApp::isPlaying( void ) {

    return( isPlayback );
}

void MoMa::SceneApp::zoom( int iMin, int iMax ) {

    lowBound.index = iMin;
    highBound.index = iMax;
    
    if( lowBound.index <= 0 ) lowBound.index = 0;
    if( highBound.index < 1 ) highBound.index = 1;
    
    lowBound.time = getTimeFromIndex( lowBound.index );
    highBound.time = getTimeFromIndex( highBound.index );
    
    appAtPos.index = lowBound.index;
    appAtPos.time = lowBound.time;
}

void MoMa::SceneApp::showAll( void ) {

    highBound.index = maxBound.index-1;
    lowBound.index = 0;
    
    highBound.time = getTimeFromIndex( highBound.index );
    lowBound.time = getTimeFromIndex( lowBound.index );
}

void MoMa::SceneApp::addOscListener( std::string header, MoMa::Track &track ) {

    MoMa::Listener list;

    list.header = header; list.track = &track;

    listener.push_back( list );
}

void MoMa::SceneApp::setOscListenerPort( int listenerPort ) {

    oscRcvPort = listenerPort;
}

void MoMa::SceneApp::setNodeSize( float size ) {

    nodeSize = size;
}

void MoMa::SceneApp::setGridSize( float size ) {

    gridSize = size;
}

void MoMa::SceneApp::setViewDistance( float distance ) {

    camera.setPosition( ofVec3f( 3.5f, 0.0f, 2.0f )*distance );
    camera.setTarget( ofVec3f( 0, 0, 0 ) ); // oF commands to move cam
    camera.setFarClip( 10000 + 10.0f*distance ); camera.setNearClip( 0 );
    light.setPosition( ofVec3f( 3.5f, 2.0f, 2.0f )*distance );
}

void MoMa::SceneApp::setPlotResolution( int reso ) {
    
    plotResolution = reso;
}

void MoMa::SceneApp::setActiveMode( int mode ) {

    activeMode = mode;
}

void MoMa::SceneApp::show3dScene( bool scene ) {

    is3dScene = scene;
}

void MoMa::SceneApp::showGround( bool ground ) {

    isGround = ground;
}

void MoMa::SceneApp::showNodeNames( bool names ) {

    isNodeNames = names;
}

void MoMa::SceneApp::showTimeTags( bool times ) {

    isTimeTags = times;
}

void MoMa::SceneApp::showAnnotation( bool annot ) {

    isAnnotation = annot;
}

void MoMa::SceneApp::showCurtain( bool curtain ) {

    isCurtain = curtain;
}

void MoMa::SceneApp::showFigures( bool figures ) {

    isFigure = figures;
}

void MoMa::SceneApp::showTimeline( bool time ) {
    
    isTimeline = time;
}

string MoMa::SceneApp::getAppPath( void ) {

#ifdef _WIN32
    return( "" );
#else
    return( "../../../" );
#endif
}

string MoMa::SceneApp::getLibPath( void ) {

#ifdef _WIN32
    return( "../../../../libs/MoMa/" );
#else
    return( "../../../../../../../libs/MoMa/" );
#endif
}

void MoMa::SceneApp::enableShortcuts( void ) {

    isShortcut = true;
}

void MoMa::SceneApp::disableShortcuts( void ) {

    isShortcut = false;
}
