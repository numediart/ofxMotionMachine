#include "mmOptionsView.h"

using namespace MoMa;

Options::Options( SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *parentUI, int group, bool minified ) : 
    app(_app),
    Canvas( _app, "View Options", position, alignment, NULL, parentUI, group, minified ) {

        /*addLabelToggle( "SHOW 3D SCENE", app->is3dScene );
        addLabelToggle( "SHOW 3D GROUND", app->isGround );
        addLabelToggle( "SHOW NODE NAMES", app->isNodeNames );
        addLabelToggle( "SHOW ANNOTATIONS", app->isAnnotation );
        addLabelToggle( "SHOW 2D FIGURES", app->isFigure );
        addLabelToggle( "SHOW CAPTIONS", app->isCaptions );
        addLabelToggle( "SHOW TIMELINE", app->isTimeline );*/

        addSpacer();
        toggle3DScene = addToggle( "Show 3D Scene", app->is3dScene );
        toggleGround = addToggle( "Show 3D Ground", app->isGround );
        toggleNodeNames = addToggle( "Show Node Names", app->isNodeNames );
        toggleAnnotations = addToggle( "Show Annotations", app->isAnnotation );
        toggleFigure = addToggle( "Show 2D Figures", app->isFigure );
        toggleCaption = addToggle( "Show Captions", app->isCaptions );
        toggleTimeline = addToggle( "Show Timeline", app->isTimeline );

        //addLabelButton( "RESET OSC", false);

        setVisible(false);
        initCanvas();
}

void Options::canvasEvent( ofxUIEventArgs &e ) {

    string name = e.widget->getName(); // We grab the name and test it
    /*if( e.widget->getKind() == OFX_UI_WIDGET_LABELTOGGLE ) {

    ofxUILabelToggle *toggle = (ofxUILabelToggle *) e.widget; // Toggle

    if( name == "SHOW 3D SCENE" ) app->show3dScene( toggle->getValue() );
    else if( name == "SHOW 3D GROUND" ) app->showGround( toggle->getValue() );
    else if( name == "SHOW NODE NAMES" ) app->showNodeNames( toggle->getValue() );
    else if( name == "SHOW ANNOTATIONS" ) app->showAnnotation( toggle->getValue() );
    else if( name == "SHOW 2D FIGURES" ) app->showFigures( toggle->getValue() );
    else if( name == "SHOW CAPTIONS" ) app->showCaptions( toggle->getValue() );
    else if( name == "SHOW TIMELINE" ) app->showTimeline( toggle->getValue() );
    }*/

    if( e.widget->getKind() == OFX_UI_WIDGET_TOGGLE ) {

        ofxUIToggle *toggle = (ofxUIToggle *) e.widget; // Toggle

        if( name == "Show 3D Scene" ) app->show3dScene( toggle->getValue() );
        else if( name == "Show 3D Ground" ) app->showGround( toggle->getValue() );
        else if( name == "Show Node Names" ) app->showNodeNames( toggle->getValue() );
        else if( name == "Show Annotations" ) app->showAnnotation( toggle->getValue() );
        else if( name == "Show 2D Figures" ) app->showFigures( toggle->getValue() );
        else if( name == "Show Captions" ) app->showCaptions( toggle->getValue() );
        else if( name == "Show Timeline" ) app->showTimeline( toggle->getValue() );
    }


    /*else if(e.widget->getKind() == OFX_UI_WIDGET_LABELBUTTON ) {

    if( name == "RESET OSC" ) {


    }
    }*/
}


void Options::update() {

    toggle3DScene->setValue(app->is3dScene );
    toggleGround->setValue(app->isGround );
    toggleNodeNames->setValue(app->isNodeNames );
    toggleAnnotations->setValue(app->isAnnotation );
    toggleFigure->setValue(app->isFigure );
    toggleCaption->setValue(app->isCaptions );
    toggleTimeline->setValue(app->isTimeline );
}