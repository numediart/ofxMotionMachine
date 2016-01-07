#include "mmPlayBar.h"

using namespace MoMa;

PlayBar::PlayBar( SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *parentUI, int group, bool minified ) : 
    app(_app),
    Canvas( _app, "Player Bar", position, alignment, NULL, parentUI, group, minified ) {

        /*vector<string> buttons;
        //buttons.push_back( "PREV.");
        buttons.push_back("PLAY");
        buttons.push_back("PAUSE");
        buttons.push_back("STOP");
        //buttons.push_back("NEXT");
        playRadio = addRadio("Bar",buttons,OFX_UI_ORIENTATION_HORIZONTAL,OFX_UI_FONT_SMALL );*/

        
        //txt_index = addTextArea("index", "Index : 0" );
        setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        //txt_time = addTextArea("time", "Time : 0", OFX_UI_FONT_SMALL );
        setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
        setGlobalButtonDimension(42);        
        bt_play = addImageButton("play", getAbsoluteResPath() + "GUI/bt_play.png", false); //path is relative to the data path of the application!
        setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
        //bt_pause = addImageToggle("pause", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_pause.png", false);
        bt_stop = addImageButton("stop", getAbsoluteResPath() + "GUI/bt_stop.png", false);
        //bt_loop = addImageToggle("loop", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_loop.png", false);
        bt_scrub = addImageToggle("scrub", getAbsoluteResPath() + "GUI/bt_scrub.png", false);
        //bt_recordVideo = addImageToggle("record", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_recordvideo.png", false);
        //bt_snapshot = addImageButton("snap", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_snapshot.png", false);
        bt_prevFrame = addImageButton("prev", getAbsoluteResPath() + "GUI/bt_previousframe.png", false);
        bt_nextFrame = addImageButton("next", getAbsoluteResPath() + "GUI/bt_nextframe.png", false);

        playingState = false;
        setVisible(true);
        initCanvas();
}

void PlayBar::initCanvas() {

    //getCanvasTitle()->setVisible(false);
    autoSizeToFitWidgets();
    setMinified(false);
    setPosition( round( ofGetWidth()/2 - getRect()->getHalfWidth() ), round( ofGetHeight() - 20 - getRect()->getHeight() ) );    
}

void PlayBar::canvasEvent( ofxUIEventArgs &e ) {

    string name = e.widget->getName(); // We grab the name and test it

    /*if( name == "PLAY" ) app->play();
    else if( name == "PAUSE" ) app->pause();
    else if( name == "STOP" ) app->stop();*/

    if( name == "play" && playingState == false && ((ofxUIButton*)(e.widget))->getValue() ) { //PLAY!

        app->setPlaybackMode( MoMa::PLAY );
        playingState = true;
        app->play();
        bt_play->getImage()->loadImage(getAbsoluteResPath() + "GUI/bt_pause.png");
    }
    else if( name == "play" && playingState == true && ((ofxUIButton*)(e.widget))->getValue() ) { //PAUSE!

        app->setPlaybackMode( MoMa::PLAY );
        playingState = false;
        app->pause();
        bt_play->getImage()->loadImage(getAbsoluteResPath() + "GUI/bt_play.png");
    }

    else if( name == "stop" ) { //STOP!

        app->setPlaybackMode( MoMa::PLAY );
        playingState = false;
        app->stop();
        bt_play->getImage()->loadImage(getAbsoluteResPath() + "GUI/bt_play.png");
    }

    else if( name == "scrub" && ((ofxUIToggle*)(e.widget))->getValue() ) { //SCRUB MODE

        if( app->activeMode == MoMa::SCENE3D ) app->setActiveMode( MoMa::SCENE2D );
        app->setPlaybackMode( MoMa::SCRUB );
    }

    else if( name == "scrub" && !((ofxUIToggle*)(e.widget))->getValue() ) { //SCRUB MODE

        app->setPlaybackMode( MoMa::PLAY );
    }

    else if( name == "prev" && ((ofxUIToggle*)(e.widget))->getValue() && app->playbackMode == MoMa::PLAY && !app->isPlaying() ) { //Previous Index

        app->previousIndex();
    }

    else if( name == "next" && ((ofxUIToggle*)(e.widget))->getValue() && app->playbackMode == MoMa::PLAY && !app->isPlaying() ) { //Next Index

        app->nextIndex();
    }
}


void PlayBar::update() {

    /*if( app->isPlaying() ) playRadio->activateToggle( "PLAY" );
    else if( app->getAppIndex()>0 ) playRadio->activateToggle( "PAUSE" );
    else playRadio->activateToggle( "STOP" );*/

    if( playingState != app->isPlaying() ) {

        playingState=app->isPlaying();
        if( playingState ) bt_play->getImage()->loadImage(getAbsoluteResPath() + "GUI/bt_pause.png");
        else bt_play->getImage()->loadImage(getAbsoluteResPath() + "GUI/bt_play.png");
    }

    if( app->playbackMode == MoMa::SCRUB ) bt_scrub->setValue(true);
    else bt_scrub->setValue(false);
    //getCanvasTitle()->setLabel( "Index : " + ofToString(app->getAppIndex()) + "\t - Time : " + ofToString(app->getAppTime()) );
    getCanvasTitle()->setLabel( ofToString( app->getAppTime() ) + " s | frame " + ofToString( app->getAppIndex() ) );
    //txt_index->setTextString("Index : " + ofToString(app->getAppIndex()) );

}