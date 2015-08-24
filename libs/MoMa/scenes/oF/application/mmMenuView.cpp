#include "mmMenuView.h"

using namespace MoMa;

MenuView::MenuView( SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *relative, bool minified ) : 
    app(_app),
    Canvas( _app, "Menu", position, alignment, relative, NULL, NULL, minified ) {

        vector<string> modeNames; // Mode names		
        modeNames.push_back( "FOCUS ON 3D SCENE" );
        modeNames.push_back( "FOCUS ON 2D FIGURES" );
        modeNames.push_back( "FOCUS ON ANNOTATION" );
        /*vector<string> playNames; // Mode names		
        playNames.push_back( "SCRUB MODE" );
        playNames.push_back( "PLAY MODE" );*/

        //addSpacer();
        //addTextArea( "Scene Mode", "Scene Mode", OFX_UI_FONT_SMALL );
        addSpacer();
        modeRadio = addRadio( "ActiveMode", modeNames, OFX_UI_ORIENTATION_VERTICAL );
        //addSpacer();
        //addTextArea("Playback Mode", "Playback Mode", OFX_UI_FONT_SMALL );
        addSpacer();
        //playRadio = addRadio( "ActivePlayMode", playNames, OFX_UI_ORIENTATION_VERTICAL );
        //addSpacer();
        addToggle("Player",true);
        initCanvas();
}

void MenuView::canvasEvent( ofxUIEventArgs &e ) {

    string name = e.widget->getName(); // We grab the name and test it

    if( name == "FOCUS ON 3D SCENE" ) app->setActiveMode( MoMa::SCENE3D );
    else if( name == "FOCUS ON 2D FIGURES" ) app->setActiveMode( MoMa::SCENE2D );
    else if( name == "FOCUS ON ANNOTATION" ) app->setActiveMode( MoMa::ANNOTATE );
    //else if( name == "SCRUB MODE") app->setPlaybackMode( MoMa::SCRUB );
    //else if( name == "PLAY MODE") app->setPlaybackMode( MoMa::PLAY );

    if( app->activeMode == MoMa::SCENE3D ) app->show3dScene( true );
    else if( app->activeMode == MoMa::SCENE2D ) app->showFigures( true );
    else if( app->activeMode == MoMa::ANNOTATE ) app->showAnnotation( true );

    if(name == "Player") {

        if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(0);
        else closeChildren();
    }

    /*if(name == "Options group 2") {

    if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(1);
    else closeChildren();
    }*/
}


void MenuView::update() {

    // We check active mode and we switch the UI representation accordingly ( coherence! )
    if( app->activeMode == MoMa::SCENE3D ) modeRadio->activateToggle( "FOCUS ON 3D SCENE" );
    else if( app->activeMode == MoMa::SCENE2D ) modeRadio->activateToggle( "FOCUS ON 2D FIGURES" );
    else if( app->activeMode == MoMa::ANNOTATE ) modeRadio->activateToggle( "FOCUS ON ANNOTATION" );

    //if( app->playbackMode == MoMa::SCRUB ) playRadio->activateToggle( "SCRUB MODE" );
    //else if( app->playbackMode == MoMa::PLAY ) playRadio->activateToggle( "PLAY MODE" );
}