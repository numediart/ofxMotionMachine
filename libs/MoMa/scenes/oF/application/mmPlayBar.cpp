#include "mmPlayBar.h"

using namespace MoMa;

PlayBar::PlayBar( SceneApp *_app, MoMa::Position position, MoMa::Position alignment, MoMa::Canvas *parentUI, bool minified ) : 
	app(_app),
	Canvas( _app, "Play Bar", position, alignment, NULL, parentUI, NULL, minified ) {

		vector<string> buttons;
		//buttons.push_back( "PREV.");
		buttons.push_back("PLAY");
		buttons.push_back("PAUSE");
		buttons.push_back("STOP");
		//buttons.push_back("NEXT");
		playRadio = addRadio("Bar",buttons,OFX_UI_ORIENTATION_HORIZONTAL,OFX_UI_FONT_SMALL );


		/*setGlobalButtonDimension(24);
		bt_play = addImageToggle("play", app->getLibPath()+"resources/GUI/bt_play.png", false);
		setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
		bt_pause = addImageToggle("pause", app->getLibPath()+"resources/GUI/bt_pause.png", false);
		//addWidgetRight(bt_pause);
		bt_stop = addImageButton("stop", app->getLibPath()+"resources/GUI/bt_stop.png", false);*/


		//addWidgetRight(bt_stop);
		/*bt_nextFrame = addImageButton("next", app->getLibPath()+"resources/GUI/bt_nextframe.png", false);
		bt_prevFrame = addImageButton("prev", app->getLibPath()+"resources/GUI/bt_previousframe.png", false);
		bt_loop = addImageToggle("loop", app->getLibPath()+"resources/GUI/bt_loop.png", false);
		bt_recordVideo = addImageToggle("record", app->getLibPath()+"resources/GUI/bt_recordvideo.png", false);
		bt_snapshot = addImageButton("snap", app->getLibPath()+"resources/GUI/bt_snapshot.png", false);
		bt_scrub = addImageToggle("scrub", app->getLibPath()+"resources/GUI/bt_scrub.png", false);*/
		initCanvas();
}

void PlayBar::initCanvas() {

	autoSizeToFitWidgets();
	setMinified(false);
	setPosition( round( ofGetWidth()/2 - getRect()->getHalfWidth() ), round( ofGetHeight() - 20 - getRect()->getHeight() ) );
}

void PlayBar::canvasEvent( ofxUIEventArgs &e ) {

	string name = e.widget->getName(); // We grab the name and test it

	if( name == "PLAY" ) app->play();
	else if( name == "PAUSE" ) app->pause();
	else if( name == "STOP" ) app->stop();
}


void PlayBar::update() {

	if( app->isPlaying() ) playRadio->activateToggle( "PLAY" );
	else if( app->getAppIndex()>0 ) playRadio->activateToggle( "PAUSE" );
	else playRadio->activateToggle( "STOP" );
}