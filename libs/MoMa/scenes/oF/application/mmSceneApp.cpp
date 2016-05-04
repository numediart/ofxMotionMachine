#include "mmSceneApp.h"
#include "mmMenuView.h"
#include "mmPlayBar.h"
#include "mmOptionsView.h"

using namespace std;
using namespace arma;

void MoMa::SceneApp::setup( ofEventArgs &args ) {

    verbose = false;
    keyEnabled = true;
    mouseEnabled = true;
    resizeEnabled = true;
    isExit = false;
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
    showCaptions( true );
    enableShortcuts();

    setActiveMode( SCENE2D );
    setPlaybackMode( PLAY );

    setAutoDrawLabelLists( true );
    setAutoDrawFeatures( false );

    showAnnotation( true );
    showCurtain( false );
    showFigures( true );
    showTimeline( false );

    isPlayback = false;
    frameRate = 100.0f;
    isBegin = true;

    lowBound.setIndex( 0, frameRate );
    highBound.setIndex( 1, frameRate );
    maxBound.setIndex( 1, frameRate );
    appMoment.setIndex( 0, frameRate );

    zoomLowBound = -1;
    zoomHighBound = -1;
    isZoom = false;

    isReach3D = false;

    figureIdx = 0; // We start with 1
    _figure.resize( 1 ); // full-screen
    _figure[figureIdx].yTop = 0; // figure
    _figure[figureIdx].yBot = ofGetHeight();

    menuView = NULL;
    playBar = NULL;
    optionsView = NULL;
    addMenuView();

    hasDragEventRegTrack = false;
    hasMouseEventRegLabelList = false;  

    oscRcvPort = 7000;
    oscSndPort = 8000;

    setup();

    _sender.setup( "127.0.0.1", oscSndPort );
    _receiver.setup( oscRcvPort );

    insertNewLabel = false;
    isLabelSelected = false;
    isEditing = false;
    tolerance = 5;
}

void MoMa::SceneApp::update( ofEventArgs &args ) {

    if (mouseEnabled) {

        switch (activeMode) {

        case SCENE3D:

            camera.enableMouseInput();

            break;

        case SCENE2D:

            camera.disableMouseInput();

            break;

        case ANNOTATE:

            camera.disableMouseInput();

            break;

        /*case CANVAS: //deprecated

            camera.disableMouseInput();

            break;*/
        }
    }

    switch( playbackMode ) {

    case SCRUB: {

        if( activeMode == SCENE2D || activeMode == ANNOTATE ) {

            appMoment.setTime( ofMap( ofGetMouseX(), 0, ofGetWidth(),
                lowBound.time(), highBound.time(), true ), frameRate );

            // appAtPos.index = ofMap( ofGetMouseX(), 0, ofGetWidth(), lowBound.index, highBound.index, true );
            // appAtPos.time = ofMap( ofGetMouseX(), 0, ofGetWidth(), lowBound.time, highBound.time, true );
        }

        break;
                }

    case STREAM: {

        // appAtPos.index = highBound.index;
        // appAtPos.time = highBound.time;

        appMoment.setTime( highBound.time(), frameRate );

        break;
                 }

    case PLAY: {

        if( ofGetElapsedTimef() > 3.0f ) {

            double timeStep = 1.0f / ofGetFrameRate();

            if( isPlayback ) {

                // appAtPos.time = appAtPos.time + timeStep;
                // appAtPos.index = getIndexFromTime( appAtPos.time );

                appMoment.setTime( appMoment.time() + timeStep, frameRate );

                if( isBegin ) {

                    onReachBegin();
                    isBegin = false;
                }

                if( appMoment.time() > highBound.time() ) {

                    // appAtPos.time = highBound.time; // Set to max first
                    // appAtPos.index = getIndexFromTime( appAtPos.time );

                    appMoment.setTime( highBound.time(), frameRate );

                    onReachEnd();

                    // appAtPos.time = lowBound.time; // Then set to min
                    // appAtPos.index = getIndexFromTime( appAtPos.time );

                    appMoment.setTime( lowBound.time(), frameRate );

                    isBegin = true;
                }
            }
        }

        break;
               }
    }

    // ---

    while( _receiver.hasWaitingMessages() ) {

        ofxOscMessage message;
        _receiver.getNextMessage( &message );

        for( int l=0; l<listener.size(); l++ ) {

            bool hasReceivedFrame = false;

            if( message.getAddress() == listener[l].header+PositionHeader ) {
                
                listener[l].track->hasRotation = false;
                
                if( listener[l].mode == ROTATIONS ) {
                    
                    listener[l].mode = POSITIONS;
                    //listener[l].track->clear();
                }
                
                int nOfNodes = message.getNumArgs()/3;
                arma::mat lPos( 3, nOfNodes );

                for( int n=0; n<nOfNodes; n++ ) {
                    
                    lPos(X,n) = message.getArgAsFloat( 3*n );
                    lPos(Y,n) = message.getArgAsFloat( 3*n+1 );
                    lPos(Z,n) = message.getArgAsFloat( 3*n+2 );
                }
                
                if( listener[l].track->position.isTimed() ) {
                    
                    listener[l].track->position.push( lPos,
                    (double)ofGetElapsedTimeMillis()/1000.0 );
                    
                } else {
                    
                    listener[l].track->position.push( lPos );
                }
                
                hasReceivedFrame = true;
            }

            if( message.getAddress() == listener[l].header+RotationHeader ) {

                listener[l].track->hasRotation = true;

                if( listener[l].mode == POSITIONS ) {

                    listener[l].mode = ROTATIONS;
                    //listener[l].track->clear();
                }

                int nOfNodes = message.getNumArgs()/7;
                arma::mat lPos(3,nOfNodes);
                arma::mat lRot(4,nOfNodes);
                for( int n=0; n<nOfNodes; n++ ) {

                    lPos(X,n) = message.getArgAsFloat( 7*n );
                    lPos(Y,n) = message.getArgAsFloat( 7*n+1 );
                    lPos(Z,n) = message.getArgAsFloat( 7*n+2 );
                    lRot(qX,n) = message.getArgAsFloat( 7*n+3 );
                    lRot(qY,n) = message.getArgAsFloat( 7*n+4 );
                    lRot(qZ,n) = message.getArgAsFloat( 7*n+5 );
                    lRot(qW,n) = message.getArgAsFloat( 7*n+6 );

                }
                if (listener[l].track->rotation.isTimed())
                    listener[l].track->rotation.push( lRot ,(double)ofGetElapsedTimeMillis()/1000.0);
                else
                    listener[l].track->rotation.push( lRot );
                if (listener[l].track->position.isTimed())
                    listener[l].track->position.push( lPos ,(double)ofGetElapsedTimeMillis()/1000.0);
                else
                    listener[l].track->position.push( lPos );
                hasReceivedFrame = true;
            }

            if( hasReceivedFrame ) {
                //     listener[l].track->push( oscFrame );
                //setPlayerSize( listener[l].track->nOfFrames() );


                appMoment.setTime( (double)ofGetElapsedTimeMillis()/1000.0);
                //if (listener[l].track->position.isTimed())

                setPlayerSize( listener[l].track->minTime(),listener[l].track->maxTime() );
                //else
                //	setPlayerSize( listener[l].track->nOfFrames());


                onOscReceived(); // Trigger custom code here
            }
        }
    }

    update();

    if( sendOscFeatures ) {

        for( int f=0; f<feature.size(); f++ ) {

            if( feature[f].isFeasible && feature[f].isSent ) {

                ofxOscMessage msg;
                msg.setAddress( feature[f].oscHeader );

                if( feature[f].type == VECTOR ) {

                    msg.addFloatArg( feature[f].feature.tvec->get( getAppTime() ) );

                } else if( feature[f].type == MATRIX ) {

                    for( int e=0; e<feature[f].feature.tmat->nOfElems(); e++ ) {

                        msg.addFloatArg( feature[f].feature.tmat->elem( e ).get( getAppTime() ) );
                    }

                } else if( feature[f].type == CUBE ) {

                    for( int c=0; c<feature[f].feature.tcube->nOfCols(); c++ ) {

                        for( int r=0; r<feature[f].feature.tcube->nOfRows(); r++ ) {

                            msg.addFloatArg( feature[f].feature.tcube->col( c ).elem( r ).get( getAppTime() ) );
                        }
                    }
                }

                _sender.sendMessage( msg );
            }
        }
    }
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
        
        if( OF_VERSION_MINOR == 9 ) ofDrawGridPlane( gridSize, 30 );
        else ofDrawGridPlane( 4000, 40 ); // They changed it in 0.9
        
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

        if( _figure.size() > 0 ) {

            _figure[figureIdx].yMin = 1.0E12;
            _figure[figureIdx].yMax = -1.0E12;

            _figure[figureIdx].plot.clear();
            _figure[figureIdx].plotId = 0;
        }

        if( autoDrawFeatures ) {

            int shownFeatureId = 0;
            int nOfShownFeatures = 0;

            for( int f=0; f<nOfFeatures(); f++ ) {

                if( feature[f].isFeasible
                    && feature[f].isSelected ) {

                        nOfShownFeatures++;
                }
            }

            // if( nOfShownFeatures < 1 ) nOfShownFeatures = 1;

            setNumOfFigures( nOfShownFeatures );

            for( int f=0; f<nOfFeatures(); f++ ) {

                if( feature[ f ].isFeasible
                    && feature[ f ].isSelected ) {

                        figure( shownFeatureId );

                        if( feature[ f ].type == VECTOR ) {

                            string name = feature[ f ].name;
                            if( feature[ f ].isSent ) name = "~ " + name;
                            if( feature[ f ].isWek ) name = "> " + name;

                            draw( *(feature[ f ].feature.tvec), name );

                        } else if( feature[ f ].type == MATRIX ) {

                            string name = feature[ f ].name;
                            if( feature[ f ].isSent ) name = "~ " + name;
                            if( feature[ f ].isWek ) name = "> " + name;

                            draw( *(feature[ f ].feature.tmat), name );

                        } else if( feature[ f ].type == CUBE ) {

                            string name = feature[ f ].name;
                            if( feature[ f ].isSent ) name = "~ " + name;
                            if( feature[ f ].isWek ) name = "> " + name;

                            for( int s=0; s<feature[ f ].feature.tcube->nOfCols(); s++ ) {

                                draw( feature[ f ].feature.tcube->col( s ),
                                    name + " ( "+ofToString( s ) + " )" );
                            }
                        }

                        shownFeatureId++;
                }
            }

        } else {

            // Scene2D is now only active if autoDrawFeatures
            // is not, until we find a better way to handle it.

            scene2d(); // 2D figures
        }

        render2d(); // 2D render
    }

    if( isAnnotation ) {

        if( autoDrawLabelLists ) {

            for( int l=0; l<nOfLabelLists(); l++ ) {

                ofSetColor( Turquoise );
                draw( labelList( l ) );
            }
        }

        annotate(); // Annotations
    }

    if( isTimeline ) {

        float selLowBound = 0;
        float selHighBound = 0;
        float absPlaybackPos = 0;

        if( maxBound.time() > 0.0f ) {

            selLowBound = ofMap( lowBound.time(), 0, maxBound.time(), 1.5f, ofGetWidth()-0.5f );
            selHighBound = ofMap( highBound.time(), 0, maxBound.time(), 1.5f, ofGetWidth()-0.5f );
            absPlaybackPos = ofMap( appMoment.time(), 0, maxBound.time(), 0, ofGetWidth() );
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
    
    legacy();
}

void MoMa::SceneApp::exit( ofEventArgs &args ) {

    isExit = true;

    for( int k=0; k<_track.size(); k++ ) {

        delete _track[k].track;
    }

    Canvas::deleteCanvas();

    exit();
}

void MoMa::SceneApp::keyPressed( ofKeyEventArgs &key ) {

    if (!keyEnabled) return;

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

                if( Canvas::canvasOpened() ) {

                    //menuView->hideAll->setValue(true);
                    Canvas::closeAllCanvas();
                }
                else {

                    //menuView->hideAll->setValue(true);
                    Canvas::reopenCanvas(); // Show/hide the canvas
                }
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

                    previousIndex();
                }

                if ( key.key == OF_KEY_RIGHT ) {

                    nextIndex();
                }
            }
        }

        keyPressed( key.key );
    }
}

void MoMa::SceneApp::keyReleased( ofKeyEventArgs &key ) {

    if (!keyEnabled) return;

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

    if (!mouseEnabled) return;

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

            for( int l=0; l<( *mouseEventRegLabelList ).size(); l++ ) {

                if( ( *mouseEventRegLabelList )[l].state == HOVERED ) {

                    ( *mouseEventRegLabelList )[l].state = SELECTED;
                    isLabelSelected = true;
                    selectedLabelIdx = l;
                    newLabel = false;

                } else if( ( *mouseEventRegLabelList )[l].state == SELECTED ) {

                    ( *mouseEventRegLabelList )[l].state = UNSELECTED;
                    isLabelSelected = false;
                    newLabel = false;
                }
            }

            if( insertNewLabel == true && newLabel == true ) {

                ( *mouseEventRegLabelList ).push_back( Label( Moment( ofMap( mouse.x, 0,
                    ofGetWidth(), lowBound.time(), highBound.time() ), frameRate ), "new" ) );

                selectedLabelIdx = ( *mouseEventRegLabelList ).size()-1;
                ( *mouseEventRegLabelList ).back().state = SELECTED;
                isLabelSelected = true;
            }
        }

        break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mousePressed( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::mouseReleased( ofMouseEventArgs &mouse ) {

    if (!mouseEnabled) return;

    switch( activeMode ) {

    case SCENE3D:

        break;

    case SCENE2D:

        if( isZoom ) {

            zoomHighBound = mouse.x;

            if (zoomLowBound >= zoomHighBound) {

                zoomHighBound = zoomLowBound;
                zoomLowBound = mouse.x;
            }

            double newMin = ofMap( zoomLowBound, 0, ofGetWidth(), lowBound.time(), highBound.time() );
            double newMax = ofMap( zoomHighBound, 0, ofGetWidth(), lowBound.time(), highBound.time() );

            zoomLowBound = zoomHighBound = -1;
            zoom( newMin, newMax );
        }

        break;

    case ANNOTATE:

        if( isZoom ) {

            zoomHighBound = mouse.x;

            if (zoomLowBound >= zoomHighBound) {

                zoomHighBound = zoomLowBound;
                zoomLowBound = mouse.x;
            }

            double newMin = ofMap( zoomLowBound, 0, ofGetWidth(), lowBound.time(), highBound.time() );
            double newMax = ofMap( zoomHighBound, 0, ofGetWidth(), lowBound.time(), highBound.time() );

            zoomLowBound = zoomHighBound = -1;
            zoom( newMin, newMax );
        }

        break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mouseReleased( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::mouseDragged( ofMouseEventArgs &mouse ) {
    
    if (!mouseEnabled) return;

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

            ( *mouseEventRegLabelList )[ selectedLabelIdx ].moment.setTime( ofMap(
                mouse.x, 0, ofGetWidth(), lowBound.time(), highBound.time() ), frameRate );
        }

        break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mouseDragged( mouse.x, mouse.y, mouse.button );
}

void MoMa::SceneApp::mouseMoved( ofMouseEventArgs &mouse ) {

    if (!mouseEnabled) return;

    switch( activeMode ) {

    case SCENE3D:

        break;

    case SCENE2D:

        break;

    case ANNOTATE:

        if( hasMouseEventRegLabelList ) {

            for( int l=0; l<(*mouseEventRegLabelList).size(); l++ ) {

                int pixLabel = ofMap( ( *mouseEventRegLabelList )[l].moment.time(),
                    lowBound.time(), highBound.time(), 0, ofGetWidth() ); // Get pixels

                if( (*mouseEventRegLabelList)[l].state != SELECTED ) {

                    if( mouse.x > pixLabel-tolerance
                        && mouse.x < pixLabel+tolerance ) {

                            ( *mouseEventRegLabelList )[l].state = HOVERED;

                    } else {

                        ( *mouseEventRegLabelList )[l].state = UNSELECTED;
                    }
                }
            }
        }

        break;
    }

    mouseX = mouse.x; mouseY = mouse.y;
    mouseMoved( mouse.x, mouse.y );
}

void MoMa::SceneApp::windowResized(ofResizeEventArgs &resize) {

    if (!resizeEnabled) return;

    Canvas::resetPositions();
    windowResized(resize.width, resize.height);
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

        setFrameRate( dragEventRegTrack->frameRate() );
        setPlayerSize(dragEventRegTrack->minTime() , dragEventRegTrack->maxTime() );
    }

    if( hasDragEventRegLabelList ) {

        deque<string> labFileNames;

        for( int k=0; k<drag.files.size(); k++ ) {

            string labFileName = drag.files[k];
            string ext = labFileName.substr( labFileName.find_last_of(".") + 1 );

            if( ext == "lab" ) {

                dragEventRegLabelList->load( labFileName, frameRate );

            }
        }
    }

    dragEvent( drag );
}

void MoMa::SceneApp::render2d( void ) {

    for( int fIdx=0; fIdx<_figure.size(); fIdx++ ) {

        float shift; // Values
        bool lowTextLoc = false;
        float nIndex = 23; // Names

        // -- Display plot names --

        if( isCaptions ) {

            for( int f=0; f<_figure[fIdx].plot.size(); f++ ) {

                ofPushStyle();

                ofSetColor( _figure[fIdx].plot[f].color ); // Grab name from plot name field
                ofDrawBitmapString( _figure[fIdx].plot[f].name, 13, _figure[fIdx].yTop+nIndex );

                ofPopStyle();

                nIndex += 14;
            }
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

                float x = ofMap( _figure[fIdx].plot[f].data.time( n ), lowBound.time(), highBound.time(), 0, ofGetWidth() ); // Apply screen mapping here
                float y = ofMap( _figure[fIdx].plot[f].data.get( n ), _figure[fIdx].yMin, _figure[fIdx].yMax, _figure[fIdx].yBot-5, _figure[fIdx].yTop+5 );

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
        float appTimeX = ofMap( appAtTime, lowBound.time(), highBound.time(), 0, ofGetWidth(), true );

        ofSetLineWidth( 1.5f ); ofSetColor( 160, 160, 160, 128 ); // Draw PB time line
        ofLine( appTimeX, 0, appTimeX, ofGetHeight() ); // horizontal line & value @ bottom
        ofDrawBitmapString( ofToString( appAtTime ), appTimeX+6, _figure[fIdx].yBot-5 );

        ofPopStyle();

        for( int f=0; f<_figure[fIdx].plot.size(); f++ ) {

            // Compute plot-specific index at which we can grab plot value
            float appAtIdx = _figure[fIdx].plot[f].data.nearestIndex(appAtTime); 

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

void MoMa::SceneApp::draw( const Node &node ) {

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

void MoMa::SceneApp::draw(const TimedVec &tvec, int hue, std::string name ) {

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

void MoMa::SceneApp::draw(const TimedVec& tvec, std::string name ) {

    if( is2D ) {

        _figure[figureIdx].hue = ( 150 + 40*_figure[figureIdx].plotId )%255;
        draw( tvec, _figure[figureIdx].hue, name ); // Automatic hue
    }
}

void MoMa::SceneApp::draw( const TimedMat& tmat, string name ) {

    if( is2D ) {

        for( int r=0; r<tmat.nOfElems(); r++ ) {

            TimedVec tvec = tmat.elem( r );
            draw( tvec, name + " ( " + ofToString( r ) + " )" );
        }
    }
}

void MoMa::SceneApp::draw( const Trace &trace, string name ) {

    if( is2D ) {

        draw( trace.position, trace.name() + " ( position )" );

        if( trace.hasRotation() ) {

            draw( trace.rotation, trace.name() + " ( rotation )" );
        }
    }
}

void MoMa::SceneApp::draw(const vec &data, int hue, string name ) {

    TimedVec tvec;

    tvec.setData( frameRate, data );
    draw( tvec, hue, name );
}

void MoMa::SceneApp::draw(const vec &data, string name ) {

    TimedVec tvec;

    tvec.setData( frameRate, data );
    draw( tvec, name );
}

void MoMa::SceneApp::draw(const mat &data, string name ) {

    TimedMat tmat;

    tmat.setData( frameRate, data );
    draw( tmat, name );
}

void MoMa::SceneApp::draw( LabelList labelList ) {

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

        float labPos = ofMap( labelList[l].moment.time(), lowBound.time(), highBound.time(), 0, ofGetWidth() );
        ofLine( labPos, 0, labPos, ofGetHeight() ); // We draw labels line and label names
        ofPushMatrix();  
        ofRotateZ(90);
        ofSetDrawBitmapMode(OF_BITMAPMODE_MODEL);
        ofDrawBitmapString( labelList[l].name, 14, -(labPos+6) );  
        ofPopMatrix();

        ofPopStyle();
    }
}

void MoMa::SceneApp::draw(const Frame &frame ) {
	if (frame.hasBoneList) {

		for (int b = 0; b < frame.boneList->size(); b++) {

			//            ofVec3f beg = toVec3f( frame.node( frame.boneList->at( b ).first ).position );
			//            ofVec3f end = toVec3f( frame.node( frame.boneList->at( b ).second ).position );
			ofVec3f beg = toVec3f(frame.getPosition().col(frame.boneList->at(b).first));
			ofVec3f end = toVec3f(frame.getPosition().col(frame.boneList->at(b).second));

			ofSetLineWidth(2);
			ofLine(beg, end);
		}
	}

	if (frame.hasRotation()) {
		if (frame.hasBoneList == false) {
			ofBoxPrimitive box;
		
			for (int b = 0; b < frame.nOfNodes(); b++) {
				ofPushStyle();
				ofSetColor(DarkTurquoise, ofGetStyle().color.a);

				box.setPosition(toVec3f(frame.node(b).position));
				box.setOrientation(toQuaternion(frame.node(b).rotation));
				box.set(nodeSize); // Set position, rotation and radius            
				box.draw();

				ofPopStyle();
			}

		}

		if (frame.hasBoneList) {
			ofBone bone;

			for (int b = 0; b < frame.boneList->size(); b++) {
				
				if (frame.getRotationOffset().is_finite()) {
					{
						float s = arma::norm(frame.getPosition().col(frame.boneList->at(b).first) - frame.getPosition().col(frame.boneList->at(b).second));
						bone.setPosition(toVec3f(frame.getPosition().col(frame.boneList->at(b).first)));

						if (frame.boneList->hasOrigNodeRot_as_boneRot)
							bone.setOrientation(toQuaternion(frame.getRotationOffset().col(frame.boneList->at(b).second)) *toQuaternion(frame.getRotation().col(frame.boneList->at(b).first)));//
						else
							bone.setOrientation(toQuaternion(frame.getRotationOffset().col(frame.boneList->at(b).second)) *toQuaternion(frame.getRotation().col(frame.boneList->at(b).second)));//
						bone.setScale(s, 1, 1);
						bone.draw();
					}
				}
			}
		}
	}
	else {

		ofPushStyle();
		ofSetColor(DarkTurquoise, ofGetStyle().color.a);
		ofSpherePrimitive sphere;
		
		sphere.setRadius(DefaultNodeSize / 2); // Set position and radius
		for (int s = 0; s < frame.nOfNodes(); s++) {

			ofPushMatrix();
			ofTranslate(toVec3f(frame.node(s).position));
			sphere.draw();
			ofPopMatrix();
		}

		ofPopStyle();
	}
	ofPushStyle();
    ofSetColor(ofGetStyle().color, 120); // We keep the color but make it transparent

    for( int n=0; n<frame.nOfNodes(); n++ ) {

        string tag  = ""; if( frame.hasNodeList && isNodeNames ) tag = frame.nodeList->name( n );
        if( frame.hasTime() && isTimeTags && n==0 ) tag += ( "(" + ofToString( frame.time() ) + ")" );        
        ofDrawBitmapString( tag, toVec3f( frame.node(n).position ) + nodeSize/1.5f );        
    }

    ofPopStyle();
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

        if( _track[ k ].track->easyName== name ) {

            isFound = true;
            kFound = k;
        }
    }

    // TODO Very bad way to do this: memory leak!!!

    if( isFound ) return( *(_track[ kFound ].track) );
    else return( *( new Track() ) ); // Make decision
}

MoMa::Track &MoMa::SceneApp::track( int index ) {

    if( index >= 0 && index < _track.size() ) return( *(_track[ index ].track) );
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
    _ll.labelList->_name = name;

    _labelList.push_back( _ll );

    registerMouseEvent( *(_labelList.back().labelList) );
}

MoMa::LabelList &MoMa::SceneApp::labelList( std::string name ) {

    int kFound = -1;
    bool isFound = false;

    for( int k=0; k<_labelList.size(); k++ ) {

        if( _labelList[ k ].labelList->_name.compare( name ) == 0 ) {

            isFound = true;
            kFound = k;
        }
    }

    // TODO Very bad way to do this: memory leak!!!
    if( isFound ) return( *(_labelList[ kFound ].labelList) );
    else return( *( new LabelList() ) ); // Make decision
}

MoMa::LabelList &MoMa::SceneApp::labelList( int index ) {

    if( index >= 0 && index < _labelList.size() ) return( *(_labelList[ index ].labelList) );
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

void MoMa::SceneApp::addNewFeature( MoMa::TimedVec &feat,
string name, string osc, bool isShown, bool isSent ) {
    
    _Feature _feat;
    
    _feat.type = VECTOR;
    _feat.feature.tvec = &feat;
    
    _feat.name = name;
    _feat.oscHeader = osc;
    
    _feat.isFeasible = true;
    _feat.isSelected = isShown;
    _feat.isSent = isSent;
    _feat.isWek = false;
    
    
    feature.push_back( _feat );
}

void MoMa::SceneApp::addNewFeature( MoMa::TimedMat &feat,
string name, string osc, bool isShown, bool isSent ) {
    
    _Feature _feat;
    
    _feat.type = MATRIX;
    _feat.feature.tmat = &feat;
    
    _feat.name = name;
    _feat.oscHeader = osc;
    
    _feat.isFeasible = true;
    _feat.isSelected = isShown;
    _feat.isSent = isSent;
    _feat.isWek = false;
    
    feature.push_back( _feat );
}

void MoMa::SceneApp::addNewFeature( MoMa::TimedCube &feat,
string name, string osc, bool isShown, bool isSent ) {
    
    _Feature _feat;
    
    _feat.type = CUBE;
    _feat.feature.tcube = &feat;
    
    _feat.name = name;
    _feat.oscHeader = osc;
    
    _feat.isFeasible = true;
    _feat.isSelected = isShown;
    _feat.isSent = isSent;
    _feat.isWek = false;
    
    feature.push_back( _feat );
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

    if( figureIdx < 0 ) figureIdx = 0; // Bound the figure idx query
    else if( figureIdx > nOfFigures()-1 ) figureIdx = nOfFigures()-1;

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

    lowBound.setFrameRate( frameRate );
    highBound.setFrameRate( frameRate );
    maxBound.setFrameRate( frameRate );
    appMoment.setFrameRate( frameRate );

    // highBound.time = getTimeFromIndex( highBound.index );
    // appAtPos.time = getTimeFromIndex( appAtPos.index ); // Check app time
}

void MoMa::SceneApp::setPlayerSize( unsigned int nOfFrames ) {

    maxBound.setIndex( nOfFrames-1, frameRate );
    highBound.setIndex( maxBound.index(), frameRate );

    if( highBound.index() < 1 ) highBound.setIndex( 1, frameRate ); // Reajust current moments
    if( appMoment.index() > highBound.index() ) appMoment.setIndex( highBound.index(), frameRate );

    // maxBound.index = nOfFrames;
    // highBound.index = nOfFrames-1;

    // if( highBound.index < 1 ) highBound.index = 1;

    // highBound.time = getTimeFromIndex( highBound.index );

    // if( appAtPos.index > highBound.index ) appAtPos.index = highBound.index;
    // appAtPos.time = getTimeFromIndex( appAtPos.index ); // Check app time
}

void MoMa::SceneApp::setPlayerSize( double minTime,double maxTime ) {
    minBound.setTime( minTime, frameRate );
    maxBound.setTime( maxTime, frameRate );
    lowBound.setTime( minBound.time(), frameRate );
    highBound.setTime( maxBound.time(), frameRate );

    if( highBound.index() < 1 ) highBound.setIndex( 1, frameRate ); // Reajust current moments
    if( appMoment.index() > highBound.index() ) appMoment.setIndex( highBound.index(), frameRate );

    // setPlayerSize( getIndexFromTime( time ) );
}

unsigned int MoMa::SceneApp::getAppIndex( void ) {

    return( appMoment.index() );
}

double MoMa::SceneApp::getAppTime( void ) {

    return( appMoment.time() );
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

    // appAtPos.index = lowBound.index;
    // appAtPos.time = getTimeFromIndex( appAtPos.index );

    appMoment.setTime( lowBound.time(), frameRate );

    isPlayback = false;
    isBegin = true;
    onStop();
}

void MoMa::SceneApp::previousIndex( void ) {

    if( appMoment.index() <= lowBound.index() ) appMoment.setIndex( highBound.index()-1, frameRate );
    else appMoment.setIndex( appMoment.index()-1, frameRate ); // Decrement and update time

    // appAtPos.time = getTimeFromIndex( appAtPos.index );
}

void MoMa::SceneApp::nextIndex( void ) {

    if( appMoment.index() == highBound.index()-1 ) appMoment.setIndex( lowBound.index(), frameRate );
    else appMoment.setIndex( appMoment.index()+1, frameRate ); // Increment and update time

    // appAtPos.time = getTimeFromIndex( appAtPos.index );
}

bool MoMa::SceneApp::isPlaying( void ) {

    return( isPlayback );
}

void MoMa::SceneApp::zoom( double tMin, double tMax ) {

    lowBound.setTime( tMin, frameRate );
    highBound.setTime( tMax, frameRate );

    if( lowBound.index() <= 0 ) lowBound.setIndex( 0, frameRate );
    if( highBound.index() < 1 ) highBound.setIndex( 1, frameRate );

    // lowBound.time = getTimeFromIndex( lowBound.index );
    // highBound.time = getTimeFromIndex( highBound.index );

    appMoment.setTime( lowBound.time(), frameRate );

    // appAtPos.index = lowBound.index;
    // appAtPos.time = lowBound.time;
}

void MoMa::SceneApp::showAll( void ) {

    lowBound.setTime(minBound.time(), frameRate );
    highBound.setTime( maxBound.time(), frameRate );

    // highBound.index = maxBound.index-1;
    // lowBound.index = 0;

    // highBound.time = getTimeFromIndex( highBound.index );
    // lowBound.time = getTimeFromIndex( lowBound.index );
}

void MoMa::SceneApp::addOscListener( std::string header, MoMa::Track &track ) {

    MoMa::Listener list;

    list.header = header; list.track = &track;

    listener.push_back( list );
}

void MoMa::SceneApp::setOscListenerPort( int listenerPort ) {

    oscRcvPort = listenerPort;
}

void MoMa::SceneApp::setOscSenderPort( int senderPort ) {

    oscSndPort = senderPort;
}

void MoMa::SceneApp::sendFeaturesAsOsc( bool send ) {

    sendOscFeatures = send;
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

void MoMa::SceneApp::showCaptions( bool caps ) {

    isCaptions = caps;
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

void MoMa::SceneApp::enableShortcuts( void ) {

    isShortcut = true;
}

void MoMa::SceneApp::disableShortcuts( void ) {

    isShortcut = false;
}

void MoMa::SceneApp::addMenuView( void ) {

    if(!menuView) {

        menuView = new MenuView(this);
        if(playBar) playBar->remove();
        playBar = new PlayBar(this, DEFAULT, DEFAULT, menuView,1);
        if(optionsView) optionsView->remove();
        optionsView = new Options(this, RIGHTSIDE, BOTTOM, menuView,2);
    }
}

void MoMa::SceneApp::removeMenuView( void ) {

    if(menuView) {

        menuView->remove();
        //delete menuView;
    }
    menuView = NULL;
    playBar = NULL;
    optionsView = NULL;
}

void MoMa::SceneApp::addPlayerBar( void ) {

    if(!playBar) new PlayBar(this);
}

void MoMa::SceneApp::removePlayerBar( void ) {

    if(playBar) playBar->remove();
    playBar = NULL;
}

void MoMa::SceneApp::enableControl() {

    if (verbose) cout << "sceneApp control enabled" << endl;
    enableMouseControl();
    enableKeyControl();
    //enableResizeControl();
}
//--------------------------------------------------------------
void MoMa::SceneApp::disableControl() {

    if (verbose) cout << "sceneApp control disabled" << endl;
    disableMouseControl();
    disableKeyControl();
    //disableResizeControl();
}
//--------------------------------------------------------------
void MoMa::SceneApp::enableMouseControl() {

        mouseEnabled = true;
        camera.enableMouseInput();
}
//--------------------------------------------------------------
void MoMa::SceneApp::disableMouseControl() {

        mouseEnabled = false;
        camera.disableMouseInput();
}
//--------------------------------------------------------------
void MoMa::SceneApp::enableKeyControl() {

        keyEnabled = true;
}
//--------------------------------------------------------------
void MoMa::SceneApp::disableResizeControl() {
    
        resizeEnabled = false;
}
//--------------------------------------------------------------
void MoMa::SceneApp::enableResizeControl() {
        
        resizeEnabled = true;
}
//--------------------------------------------------------------
void MoMa::SceneApp::disableKeyControl() {
        
        keyEnabled = false;
}