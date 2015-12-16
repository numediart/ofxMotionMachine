#include "mmMenuView.h"

using namespace MoMa;

MenuView::MenuView( SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *relative, bool minified ) : 
    app(_app),
    Canvas( _app, "Menu", position, alignment, relative, NULL, NULL, true ) {

        vector<string> modeNames; // Mode names		
        modeNames.push_back( "Focus on 3D Scene" );
        modeNames.push_back( "Focus on 2D Figures" );
        modeNames.push_back( "Focus on Annotation" );
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
        playBar = addToggle("Player Bar",true);
        viewOptions = addToggle("View Options",false);
        hideAll = addToggle("View Other Canvas",true);
        initCanvas();
}

void MenuView::canvasEvent( ofxUIEventArgs &e ) {

    string name = e.widget->getName(); // We grab the name and test it

    if( name == "Focus on 3D Scene" ) app->setActiveMode( MoMa::SCENE3D );
    else if( name == "Focus on 2D Figures" ) app->setActiveMode( MoMa::SCENE2D );
    else if( name == "Focus on Annotation" ) app->setActiveMode( MoMa::ANNOTATE );
    //else if( name == "SCRUB MODE") app->setPlaybackMode( MoMa::SCRUB );
    //else if( name == "PLAY MODE") app->setPlaybackMode( MoMa::PLAY );

    /*if( app->activeMode == MoMa::SCENE3D ) app->show3dScene( true );
    else if( app->activeMode == MoMa::SCENE2D ) app->showFigures( true );
    else if( app->activeMode == MoMa::ANNOTATE ) app->showAnnotation( true );*/

    else if(name == "Player Bar") {

        if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(0);
        else closeChildren(0);
    }

    else if(name == "View Options") {

        if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(1);
        else closeChildren(1);
    }

    else if(name == "View Other Canvas") {

        if(!((ofxUIToggle*)(e.widget))->getValue()) {

            closeOtherCanvas();
        }

        else reopenOtherCanvas(); // Show/hide the canvas
    }

    /*if(name == "Options group 2") {

    if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(1);
    else closeChildren();
    }*/
}


void MenuView::update() {

    // We check active mode and we switch the UI representation accordingly ( coherence! )
    if( app->activeMode == MoMa::SCENE3D ) modeRadio->activateToggle( "Focus on 3D Scene" );
    else if( app->activeMode == MoMa::SCENE2D ) modeRadio->activateToggle( "Focus on 2D Figures" );
    else if( app->activeMode == MoMa::ANNOTATE ) modeRadio->activateToggle( "Focus on Annotation" );

    playBar->setValue( childrenOpened(0) );
    viewOptions->setValue( childrenOpened(1) );

    //if( app->playbackMode == MoMa::SCRUB ) playRadio->activateToggle( "SCRUB MODE" );
    //else if( app->playbackMode == MoMa::PLAY ) playRadio->activateToggle( "PLAY MODE" );
}

void MenuView::closeOtherCanvas() {

    vector<Canvas*> tmp = getAllCanvas();
    closedCanvas.clear();
    for(int i=0;i<tmp.size();i++) {

        if(tmp[i]->isVisible() && tmp[i]!= this && tmp[i]!=getChildren()[0][0] && tmp[i]!=getChildren()[1][0]  ) {

            tmp[i]->setVisible(false);
            closedCanvas.push_back(tmp[i]);
        }
    }
}

void MenuView::reopenOtherCanvas() {

    for(int i=0;i<closedCanvas.size();i++) {

        closedCanvas[i]->setVisible(true);
    }
    closedCanvas.clear();
}