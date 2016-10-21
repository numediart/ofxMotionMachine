#include "mmPlayBar.h"

using namespace MoMa;

PlayBar::PlayBar(SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *parentUI, int group, bool minified) :
    app(_app),
    Canvas(_app, "Player", position, alignment, NULL, parentUI, group, minified) {

    /*vector<string> buttons;
    //buttons.push_back( "PREV.");
    buttons.push_back("PLAY");
    buttons.push_back("PAUSE");
    buttons.push_back("STOP");
    //buttons.push_back("NEXT");
    playRadio = addRadio("Bar",buttons,OFX_UI_ORIENTATION_HORIZONTAL,OFX_UI_FONT_SMALL );*/

    libPath = getAbsoluteResPath();
    //cout << "playbar width" << getRect()->getWidth() << endl;
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

    addLabel("   ");
    bt_prevFrame = addImageButton("prev", libPath + "GUI/bt_previousframe.png", false);
    timeinput = addTextInput("time", "0.0", bt_prevFrame->getRect()->getWidth(), bt_prevFrame->getRect()->getHeight());
    bt_nextFrame = addImageButton("next", libPath + "GUI/bt_nextframe.png", false);

    addLabel("   ");
    bt_prevSpeed = addImageButton("speed-", libPath + "GUI/bt_previousframe.png", false);
    speedinput = addTextInput("speed", "1.0", bt_prevSpeed->getRect()->getWidth(), bt_prevSpeed->getRect()->getHeight());
    bt_nextSpeed = addImageButton("speed+", libPath + "GUI/bt_nextframe.png", false);

    setWidgetPosition(OFX_UI_WIDGET_POSITION_DOWN);

    //cout << "playbar width" << getRect()->getWidth() << endl;

    timeslider = addMinimalSlider("slider", app->lowBound.time(), app->highBound.time(), app->getAppTime());// , getRect()->getWidth() + 20);
    timeslider->setShowValue(false);
    mywidth = bt_nextSpeed->getRect()->getMaxX() - 10;
    /*cout << "bt_nextSpeed->getRect()->getMaxX(): " << bt_nextSpeed->getRect()->getMaxX() << endl;
    cout << "getRect()->getX(): " << getRect()->getX() << endl;
    cout << "getRect()->getWidth(): " << getRect()->getWidth() << endl;*/
    timeslider->getRect()->setWidth(mywidth); //Set slider width to canvas width
    timeslider->getRect()->setX(getRect()->getX() + 5);
    timeslider->setColorFill(Gray);
    //textlabel = addLabel("text", "This is a test. This is a test. This is a test. This is a test. This is a test. This is a test.", OFX_UI_FONT_SMALL);
    //textlabel->getRect()->setWidth(mywidth);
    //textarea = new ofxUITextArea("text", "This is a test. This is a test. This is a test. This is a test. This is a test. This is a test.", mywidth, 0.0, 0.0, getRect()->getMaxY(), OFX_UI_FONT_SMALL);
    //addWidget(textarea);
    //textarea = addTextArea("text", "This is a test. This is a test. This is a test. This is a test. This is a test. This is a test.", OFX_UI_FONT_SMALL);
    //setFontSize(OFX_UI_FONT_SMALL, 5);
    //textarea->getRect()->setWidth(mywidth);
    //textarea->getLabelWidget()->getRect()->setWidth(mywidth);
    //cout << "textarea->getRect()->getWidth(): " << textarea->getRect()->getWidth() << endl;
    //cout << "textarea->getLabelWidget()->getRect()->getWidth(): " << textarea->getLabelWidget()->getRect()->getWidth() << endl;

    //frslider = addMinimalSlider("framerate", 0.1, 1000, app->frameRate);
    //frslider->setShowValue(false);
    //frslider->getRect()->setWidth(getRect()->getWidth() + 20); //Set slider width to canvas width
    //frslider->setColorFill(Gray);

    next = false;
    prev = false;
    nextspeed = false;
    prevspeed = false;
    enteringFrame = false;
    enteringSpeed = false;
    playingState = app->isPlayback;
    setVisible(true);
    initCanvas();

    string title1 = "Time (s)";
    string title2 = "Speed";
    int title1width = canvasTitle->getStringWidth(title1);
    int framepos = timeinput->getRect()->getX(false);
    int framewidth = timeinput->getRect()->getWidth();
    int title1pos = framepos + (double)framewidth / 2 - (double)title1width / 2;
    int title2width = canvasTitle->getStringWidth(title2);
    int speedpos = speedinput->getRect()->getX(false);
    int speedwidth = speedinput->getRect()->getWidth();
    int title2pos = speedpos + (double)speedwidth / 2 - (double)title2width / 2;
    
    ofxUILabel *canvasTitle1 = new ofxUILabel(title1pos, canvasTitle->getRect()->getY(false), title1width+10, title1, title1, canvasTitle->getSize());
    ofxUILabel *canvasTitle2 = new ofxUILabel(title2pos, canvasTitle->getRect()->getY(false), title2width+10, title2, title2, canvasTitle->getSize());
    addWidget(canvasTitle1);
    addWidget(canvasTitle2);

    /*cout << "canvas title: " << canvasTitle->getRect()->getX(false) << endl;
    cout << canvasTitle->getRect()->getY(false) << endl;
    cout <<  canvasTitle->getRect()->getWidth() << endl;
    cout << "canvas title1: " << canvasTitle1->getRect()->getX(false) << endl;
    cout << canvasTitle1->getRect()->getY(false) << endl;
    cout << canvasTitle1->getRect()->getWidth() << endl;
    cout << "canvas title2: " << canvasTitle2->getRect()->getX(false) << endl;
    cout << canvasTitle2->getRect()->getY(false) << endl;
    cout << canvasTitle2->getRect()->getWidth() << endl;*/
}

void PlayBar::initCanvas() {

    //getCanvasTitle()->setVisible(false);
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

        if (!enteringFrame) { //Beginning typing

            pause();
            enteringFrame = true;
            timeinput->setTextString("");
        }

        if (!timeinput->isFocused()) { //Releasing

            enteringFrame = false;
            double mytime;

            try {
                mytime = stof(timeinput->getTextString());
                app->appMoment.setTime(mytime);
            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid timestamp: " << timeinput->getTextString() << '\n';
                timeinput->setTextString(ofxUIToString(app->getAppTime(), 2));
            }
        }
    }

    else if (name == "speed-" && bt_prevSpeed->getValue()) {

        app->setPlaybackMode(MoMa::PLAY);
        prevspeed = true; 
    }

    else if (name == "speed+" && bt_nextSpeed->getValue()) {

        nextspeed = true;
        app->setPlaybackMode(MoMa::PLAY);
    }

    else if (name == "speed") {

        if (!enteringSpeed) { //Beginning typing

            app->setPlaybackMode(MoMa::PLAY);
            enteringSpeed = true;
            speedinput->setTextString("");
        }

        if (!speedinput->isFocused()) { //Releasing

            enteringSpeed = false;            
            double myspeed;

            try {
                myspeed = stod(speedinput->getTextString());
                app->playSpeed = max(myspeed, 0.1);
                app->playSpeed = min(app->playSpeed, 100.0);
            }
            catch (const std::invalid_argument& ia) {
                std::cerr << "Invalid player speed: " << speedinput->getTextString() << '\n';
                speedinput->setTextString(to_string(app->playSpeed));
            }
        }
    }

    else if (name == "slider") {

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
        nextspeed = false;
        prevspeed = false;
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

    // note : bt_nextspeed->getRect()->getMaxX() is in local x (from beginning of superCanvas),
    //but bt_nextspeed->getRect()->getX() is in global. bt_nextspeed->getRect()->getX(false) would be in local
    mywidth = timeslider->getRect()->getWidth();    
    string sliderstring = ofxUIToString(app->getAppTime(), 2) + "s";// - frame " + ofToString(app->getAppIndex());//2 decimal precision
    int stringsize = timeslider->getLabelWidget()->getStringWidth(sliderstring);
    int space = (mywidth - stringsize) / 2;
    timeslider->getLabelWidget()->getRect()->setX(space);    

    timeslider->getLabelWidget()->setLabel(sliderstring);
    if(!enteringSpeed) speedinput->setTextString(ofxUIToString(app->playSpeed,1));
    if(!enteringFrame) timeinput->setTextString(ofxUIToString(app->getAppTime(), 2));

    /*if (app->frameRate > frslider->getMax()) frslider->setMax(1.5*app->frameRate);
    frslider->setValue(app->frameRate);

    frslider->getLabelWidget()->setLabel("framerate: " + ofToString(app->frameRate));*/

    if (next) {

        app->nextIndex();
    }
    else if (prev) {

        app->previousIndex();
    }
    if (nextspeed) {

        app->playSpeed = min(app->playSpeed + 0.1, 100.0);
    }
    else if (prevspeed) {

        app->playSpeed = max(app->playSpeed - 0.1, 0.1);
    }

    //Update text label
    /*string text;
    if (app->playbackMode == MoMa::SCRUB) text = "Press p for playback mode. ";
    else text = "Press s for scrub mode. ";
    if(app->activeMode == MoMa::SCENE3D) text = text + "Press 2/a for 2D/annotation focus. ";
    else if(app->activeMode == MoMa::SCENE2D) text = text + "Press 3/a for 3D/annotation focus. ";
    else if (app->activeMode == MoMa::ANNOTATE) text = text + "Press 2/3 for 2D/3D focus. ";
    if (app->lowBound.time() == app->minBound.time() && app->highBound.time() == app->maxBound.time()) text = text + "Hold z to zoom.";
    else text = text + "Press u to unzoom.";*/
    //textlabel->setLabel(text);
}

void PlayBar::pause() {

    app->setPlaybackMode(MoMa::PLAY);
    playingState = false;
    app->pause();
    bt_play->getImage()->loadImage(libPath + "GUI/bt_play.png");
}