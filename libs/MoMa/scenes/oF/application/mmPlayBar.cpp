#include "mmPlayBar.h"

using namespace MoMa;

PlayBar::PlayBar(SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *parentUI, int group, bool minified) :
    app(_app),
    Canvas(_app, "Player Bar", position, alignment, NULL, parentUI, group, minified) {

    /*vector<string> buttons;
    //buttons.push_back( "PREV.");
    buttons.push_back("PLAY");
    buttons.push_back("PAUSE");
    buttons.push_back("STOP");
    //buttons.push_back("NEXT");
    playRadio = addRadio("Bar",buttons,OFX_UI_ORIENTATION_HORIZONTAL,OFX_UI_FONT_SMALL );*/

    libPath = getAbsoluteResPath();

    //txt_index = addTextArea("index", "Index : 0" );
    setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    //txt_time = addTextArea("time", "Time : 0", OFX_UI_FONT_SMALL );
    setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);
    setGlobalButtonDimension(42);
    bt_play = addImageButton("play", libPath + "GUI/bt_play.png", false); //path is relative to the data path of the application!
    setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    //bt_pause = addImageToggle("pause", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_pause.png", false);
    bt_stop = addImageButton("stop", libPath + "GUI/bt_stop.png", false);
    //bt_loop = addImageToggle("loop", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_loop.png", false);
    bt_scrub = addImageToggle("scrub", libPath + "GUI/bt_scrub.png", false);
    //bt_recordVideo = addImageToggle("record", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_recordvideo.png", false);
    //bt_snapshot = addImageButton("snap", ofFilePath::getCurrentExeDir() + app->getAppPath() + "data/resources/GUI/bt_snapshot.png", false);
    bt_prevFrame = addImageButton("prev", libPath + "GUI/bt_previousframe.png", false);
    bt_nextFrame = addImageButton("next", libPath + "GUI/bt_nextframe.png", false);

    setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    timeslider = addMinimalSlider("time", app->lowBound.time(), app->highBound.time(), app->getAppTime());
    timeslider->setShowValue(false);
    timeslider->getRect()->setWidth(getRect()->getWidth() + 20); //Set slider width to canvas width
    timeslider->setColorFill(Gray);

    //frslider = addMinimalSlider("framerate", 0.1, 1000, app->frameRate);
    //frslider->setShowValue(false);
    //frslider->getRect()->setWidth(getRect()->getWidth() + 20); //Set slider width to canvas width
    //frslider->setColorFill(Gray);

    next = false;
    prev = false;

    playingState = app->isPlayback;
    setVisible(true);
    initCanvas();
}

void PlayBar::initCanvas() {

    getCanvasTitle()->setVisible(false);
    autoSizeToFitWidgets();
    setMinified(false);
    setPosition(round(ofGetWidth() / 2 - getRect()->getHalfWidth()), round(ofGetHeight() - 20 - getRect()->getHeight()));
}

void PlayBar::canvasEvent(ofxUIEventArgs &e) {

    string name = e.widget->getName(); // We grab the name and test it

    /*if( name == "PLAY" ) app->play();
    else if( name == "PAUSE" ) app->pause();
    else if( name == "STOP" ) app->stop();*/

    if (name == "play" && playingState == false && bt_play->getValue()) { //PLAY!

        app->setPlaybackMode(MoMa::PLAY);
        playingState = true;
        app->play();
        bt_play->getImage()->loadImage(libPath + "GUI/bt_pause.png");
    }
    else if (name == "play" && playingState == true && bt_play->getValue()) { //PAUSE!

        pause();
    }

    else if (name == "stop") { //STOP!

        app->setPlaybackMode(MoMa::PLAY);
        playingState = false;
        app->stop();
        bt_play->getImage()->loadImage(libPath + "GUI/bt_play.png");
    }

    else if (name == "scrub" && bt_scrub->getValue()) { //SCRUB MODE

        pause();
        if (app->activeMode == MoMa::SCENE3D) app->setActiveMode(MoMa::SCENE2D);
        app->setPlaybackMode(MoMa::SCRUB);
    }

    else if (name == "scrub" && !bt_scrub->getValue()) { //SCRUB MODE

        app->setPlaybackMode(MoMa::PLAY);
    }

    else if (name == "prev" && bt_prevFrame->getValue()) {// && app->playbackMode == MoMa::PLAY && !app->isPlaying()) { //Previous Index

        //app->previousIndex();
        pause();
        prev = true;
    }

    else if (name == "next" && bt_nextFrame->getValue()) {// && app->playbackMode == MoMa::PLAY && !app->isPlaying()) { //Next Index

        //app->nextIndex();
        pause();
        next = true;
    }
    else if (name == "time") {

        pause();
        app->appMoment.setTime(timeslider->getValue());
    }

    //else if (name == "framerate") {

    //    //int playersize = app->highBound.index() - app->lowBound.index() + 1;
    //    //float min = app->minBound.time();
    //    //float max = app->maxBound.time();
    //    app->setFrameRate(frslider->getValue());
    //    //app->setPlayerSize(min,max);
    //}

    else {

        next = false;
        prev = false;
    }
}


void PlayBar::update() {

    /*if( app->isPlaying() ) playRadio->activateToggle( "PLAY" );
    else if( app->getAppIndex()>0 ) playRadio->activateToggle( "PAUSE" );
    else playRadio->activateToggle( "STOP" );*/

    if (playingState != app->isPlaying()) {

        playingState = app->isPlaying();
        if (playingState) bt_play->getImage()->loadImage(libPath + "GUI/bt_pause.png");
        else bt_play->getImage()->loadImage(libPath + "GUI/bt_play.png");
    }

    if (app->playbackMode == MoMa::SCRUB) bt_scrub->setValue(true);
    else bt_scrub->setValue(false);
    //getCanvasTitle()->setLabel( "Index : " + ofToString(app->getAppIndex()) + "\t - Time : " + ofToString(app->getAppTime()) );
    //getCanvasTitle()->setLabel(ofToString(app->getAppTime()) + " s | frame " + ofToString(app->getAppIndex()));
    //txt_index->setTextString("Index : " + ofToString(app->getAppIndex()) );

    timeslider->setMin(app->lowBound.time());
    timeslider->setMax(app->highBound.time());
    timeslider->setValue(app->getAppTime());

    string timestring = ofToString(app->getAppTime());
    size_t dot = timestring.find_last_of(".");
    size_t newsize = min(dot + 3, timestring.size());
    timestring.resize(newsize); //2 decimal precision
    int space = 20 - timestring.size();
    string spacestring(space,' '); // a string composed of spaces
    /*for (int i = 0; i < space; i++) {

        spacestring.push_back(' ');
    }*/


    timeslider->getLabelWidget()->setLabel(spacestring + timestring + " s - frame " + ofToString(app->getAppIndex()));

    /*if (app->frameRate > frslider->getMax()) frslider->setMax(1.5*app->frameRate);
    frslider->setValue(app->frameRate);

    frslider->getLabelWidget()->setLabel("framerate: " + ofToString(app->frameRate));*/

    if (next) {

        app->nextIndex();
    }
    else if (prev) {

        app->previousIndex();
    }
}

void PlayBar::pause() {

    app->setPlaybackMode(MoMa::PLAY);
    playingState = false;
    app->pause();
    bt_play->getImage()->loadImage(libPath + "GUI/bt_play.png");
}