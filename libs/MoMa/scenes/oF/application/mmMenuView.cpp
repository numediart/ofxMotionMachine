#include "mmMenuView.h"

using namespace MoMa;

MenuView::MenuView( SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *relative, bool minified ) : 
    app(_app),
    Canvas( _app, "Menu", position, alignment, relative, NULL, NULL, minified ) {

        vector<string> modeNames; // Mode names
        modeNames.push_back( "FOCUS ON 3D SCENE" );
        modeNames.push_back( "FOCUS ON 2D FIGURES" );
        modeNames.push_back( "FOCUS ON ANNOTATION" );
        addSpacer();
        // New radio button called "ActiveMode": switch between modes
        addRadio( "ActiveMode", modeNames, OFX_UI_ORIENTATION_VERTICAL );
        modeRadio = (ofxUIRadio *)getWidget( "ActiveMode" ); // Radio

        /*addToggle("Options group 1",false);
        addToggle("Options group 2",false);*/
        initCanvas();
}

void MenuView::canvasEvent( ofxUIEventArgs &e ) {
    
    string name = e.widget->getName(); // We grab the name and test it
    
    if( name == "FOCUS ON 3D SCENE" ) app->setActiveMode( MoMa::SCENE3D );
    else if( name == "FOCUS ON 2D FIGURES" ) app->setActiveMode( MoMa::SCENE2D );
    else if( name == "FOCUS ON ANNOTATION" ) app->setActiveMode( MoMa::ANNOTATE );

    if( app->activeMode == MoMa::SCENE3D ) app->show3dScene( true );
    if( app->activeMode == MoMa::SCENE2D ) app->showFigures( true );
    if( app->activeMode == MoMa::ANNOTATE ) app->showAnnotation( true );

    /*if(name == "Options group 1") {

        if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(0);
        else closeChildren();
    }

    if(name == "Options group 2") {

        if(((ofxUIToggle*)(e.widget))->getValue()) openChildren(1);
        else closeChildren();
    }*/
}


void MenuView::update() {

    // We check active mode and we switch the UI representation accordingly ( coherence! )
    if( app->activeMode == MoMa::SCENE3D ) modeRadio->activateToggle( "FOCUS ON 3D SCENE" );
    else if( app->activeMode == MoMa::SCENE2D ) modeRadio->activateToggle( "FOCUS ON 2D FIGURES" );
    else if( app->activeMode == MoMa::ANNOTATE ) modeRadio->activateToggle( "FOCUS ON ANNOTATION" );
}