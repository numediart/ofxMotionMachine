/** Canvas : Basic UI canvas specific to the MoMa.
*
* Created by Mickaël Tits on 03/07/2015
* @file mmCanvas.cpp
* @brief MotionMachine source file for Canvas class.
* @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmCanvas.h"

using namespace std;
using namespace MoMa;

/** Static vectors storing addresses of the canvas*/
vector<Canvas*> Canvas::mainCanvas;
vector<Canvas*> Canvas::allCanvas;

int Canvas::_limit(0);

Canvas::Canvas( SceneApp *app, string title, Position position, Position alignment, Canvas *relative, Canvas *parent, int group, bool minified ) : 
    ofxUISuperCanvas( title ), 
    _app(app), 
    _relative(relative),
    _parent(parent),  
    _group(group-1),
    _position(position), 
    _alignment(alignment), 
    _minified(minified)
{

    setupCanvas();
}

Canvas::Canvas( SceneApp *app, std::string title, Canvas *parent, int group,  bool minified ) : 
    ofxUISuperCanvas( title ), 
    _app(app), 
    _relative(NULL),
    _parent(parent),  
    _group(group-1),
    _position(DEFAULT), 
    _alignment(DEFAULT), 
    _minified(minified) {

        setupCanvas();
}

void Canvas::setupCanvas() {

	_allIndex = allCanvas.size();
    allCanvas.push_back(this);
    childrenCanvas.reserve(1);

    if(_parent == NULL) {

        _index = mainCanvas.size();
        mainCanvas.push_back(this);
    }
    else {       
        if(_group<0) _group = 0;
        if(_group+1 > _parent->childrenCanvas.size()) 
            _parent->childrenCanvas.resize(_group+1);
        _index = _parent->childrenCanvas[_group].size();
        _parent->childrenCanvas[_group].push_back(this);
        setVisible(false);
    }

    ofAddListener( newGUIEvent, this, &Canvas::guiEvent );
    setColorBack( ofColor( MoMa::DarkTurquoise, 190 ) ); 
    _isInitialized = false;
    savedMode = _app->activeMode;
    _isCanvasHit = false;
    _isShortCutDisabled = false;
}

void Canvas::initCanvas() {        

    autoSizeToFitWidgets(); //fit to widgets of the herited class

    setMinified(false); //(temporary for canvas placement)

    Position position(_position), alignment(_alignment);
    Canvas* relative(_relative);

    if(_index == 0) _limit = 0;


    if( ( relative == NULL && position == DEFAULT && _index > 0 ) ||
        ( relative == NULL && position == DEFAULT && _parent != NULL ) ) {

            // Default relative placement ( first canvas (index = 0) placed on
            // Top/Right corner; next ones placed below, right edge aligned )

            if(_parent != NULL) { //Children placement

                if(_index != 0) relative = _parent->childrenCanvas[_group][_index-1]; //Child placed relative to the previous child
                else relative = _parent; //First cihld placed relative to the parent
            }
            else relative = mainCanvas[_index-1]; //Main canvas placed relative to previous canvas

            float H = getRect()->getHeight();
            float W = getRect()->getWidth();
            float relativeX = relative->getRect()->getX();
            float relativeY = relative->getRect()->getY();
            float relativeH = relative->getRect()->getHeight();
            float relativeW = relative->getRect()->getWidth();

            if(relativeY + relativeH + 10 + H < ofGetHeight() && _limit <1 ) { // test window bottom limit

                position = BELOW;
                alignment = RIGHT;
            }
            else if(relativeX - 10 - W > 0 && _limit < 2) { // test window left limit

                position = LEFT;
                if(_limit == 0) alignment = BOTTOM;
                else alignment = BELOW;
                _limit = 1;
            }
            else if(relativeY - 10 - H > 0 && _limit < 3) { // test window top limit

                position = ABOVE;
                if(_limit == 1 ) alignment = LEFTSIDE;			
                else alignment = LEFT;			
                _limit = 2;
            }  
            else if(relativeX + relativeW + 10 + W  < ofGetWidth() - 10 - mainCanvas[0]->getRect()->getWidth() - 10 && _limit < 4) { // test window right limit (taking into account the first Canvas canvas supposed to be on the top/right corner)

                position = RIGHT;
                if(_limit == 2 ) alignment = TOP;
                else alignment = ABOVE;			
                _limit = 3;
            }  

            else {

                relative = NULL;
                _limit = 4;
                cout << "Too much mainCanvas for default handle\n";
                setPosition(0,0);
                return;            
            } 
    }

    setPos(position,alignment,relative);
    setMinified(_minified);

    _isInitialized = true;
}


void Canvas::setPos(Position position, Position alignment, Canvas *relative) {

    float H = this->getRect()->getHeight();
    float W = this->getRect()->getWidth();

    //Default : Top, Right
    float xPos = ofGetWidth() - W - 10; 
    float yPos = 10;

    if(relative == NULL) { // Set positions relative to main OF Window

        if(position == LEFT || position == LEFTSIDE || alignment == LEFT || alignment == LEFTSIDE ) xPos = 10;
        if(position == BELOW || position == BOTTOM || alignment == BELOW || alignment == BOTTOM ) yPos = ofGetHeight() - H - 10;

    }

    else { // Set relative positions

        float relativeX = relative->getRect()->getX();
        float relativeY = relative->getRect()->getY();
        float relativeH = relative->getRect()->getHeight();
        float relativeW = relative->getRect()->getWidth();

        // Default: Below, right edges aligned
        xPos = relativeX + relativeW - W;
        yPos = relativeY + relativeH + 10;

        switch( position ) {

        case LEFTSIDE : xPos = 10; break;
        case RIGHTSIDE : xPos = ofGetWidth() - W - 10; break;
        case LEFT : xPos = relativeX - W - 10; break;
        case RIGHT : xPos = relativeX + relativeW + 10; break;
        case TOP : yPos = 10; break;
        case BOTTOM : yPos = ofGetHeight() - H - 10; break;
        case ABOVE : yPos = relativeY - 10 - H; break;
        case BELOW : yPos = relativeY + relativeH + 10; break;
        case DEFAULT: break;
        }

        switch( alignment ) {

        case LEFTSIDE : xPos = 10; break;
        case RIGHTSIDE : xPos = ofGetWidth() - W - 10; break;
        case LEFT : xPos = relativeX; break; //Align left edges
        case RIGHT : xPos = xPos = relativeX + relativeW - W; break; //Align right edges
        case TOP : yPos = 10; break;
        case BOTTOM : yPos = ofGetHeight() - H - 10; break;
        case ABOVE : yPos = relativeY; break; //Align upper edges
        case BELOW : yPos = relativeY + relativeH - H; break; //Align below edges
        case DEFAULT: break;
        }
    }

    setPosition(xPos,yPos);
}


void Canvas::resetPositions() {

    _limit = 0;
    /*for(int i=0;i<mainCanvas.size();i++) {

    mainCanvas[i]->initUI();
    mainCanvas[i]->closeChildren();
    }*/
    for(int i=0;i<allCanvas.size();i++) {

        allCanvas[i]->initCanvas();
    }
}

void Canvas::remove() {

	//delete children
	for(int g=0;g<childrenCanvas.size();g++) {
        for(int i=0;i<childrenCanvas[g].size();i++) {

            childrenCanvas[g][i]->remove();

        }
    }
	
	//erase from canvas groups
	if(_parent == NULL) {

		mainCanvas.erase(mainCanvas.begin()+_index);
    }
    else {  

		_parent->childrenCanvas.erase(_parent->childrenCanvas.begin()+_group,_parent->childrenCanvas.begin()+_index);
    }
	allCanvas.erase(allCanvas.begin()+_allIndex);
	delete this;
}

void Canvas::deleteCanvas() {

    for(int i=0;i<allCanvas.size();i++) {

        if(allCanvas[i] != NULL) delete(allCanvas[i]);
    }
}

void Canvas::openChildren(int group) {

    if(group < childrenCanvas.size() ) {

        closeChildren();
        for(int i=0;i<childrenCanvas[group].size();i++) {

            childrenCanvas[group][i]->initCanvas();
            childrenCanvas[group][i]->setVisible(true);
        }
    }
    else cout << "That group of children canvas does not exist\n";
}

void Canvas::openChild(int index, int group) {

    if(group < childrenCanvas.size() && index < childrenCanvas[group].size() ) {

        closeChildren();
        childrenCanvas[group][index]->initCanvas();
        childrenCanvas[group][index]->setVisible(true);
    }
    else cout << "That child canvas does not exist\n";
}

bool Canvas::childrenOpened(int group) {

    bool opened = false;
    if(group < childrenCanvas.size() ) {

        for(int i=0;i<childrenCanvas[group].size();i++) {

            opened = opened || childrenCanvas[group][i]->isVisible();
        }
    }
    else cout << "That group of children canvas does not exist\n";
    return opened;
}


bool Canvas::canvasOpened() {

    bool opened = false;
    for(int i=0;i<allCanvas.size();i++) {

        opened = opened || allCanvas[i]->isVisible();
    }    

    return opened;
}

void Canvas::closeChildren() {

    for(int g=0;g<childrenCanvas.size();g++) {
        for(int i=0;i<childrenCanvas[g].size();i++) {

            childrenCanvas[g][i]->setVisible(false);
        }
    }
}


void Canvas::openMainCanvas() {

    for(int i=0;i<mainCanvas.size();i++) {

        mainCanvas[i]->initCanvas();
        mainCanvas[i]->setVisible(true);
    }
}

void Canvas::closeMainCanvas() {

    for(int i=0;i<mainCanvas.size();i++) {

        mainCanvas[i]->setVisible(false);
    }
}

void Canvas::closeAllCanvas() {

    for(int i=0;i<allCanvas.size();i++) {

        allCanvas[i]->setVisible(false);
    }
}

void Canvas::mainView() {

    closeAllCanvas();
    openMainCanvas();
}

void Canvas::windowResized(int w, int h) {

    resetPositions();
}

void Canvas::onMousePressed( ofMouseEventArgs &data ) {

    if( isHit( data.x, data.y ) && !_isCanvasHit ) {

        _isCanvasHit = true;
        savedMode = _app->activeMode; // save mode previous to click
        _app->setActiveMode( CANVAS ); // switch to Canvas
    }

    ofxUISuperCanvas::onMousePressed(data);
}

void Canvas::onMouseReleased( ofMouseEventArgs &data ) {

    if(_isCanvasHit) {

        _app->setActiveMode( savedMode ); // restore previous mode
        _isCanvasHit = false;
    }

    ofxUISuperCanvas::onMouseReleased(data);
}


void Canvas::update() {

}

void Canvas::guiEvent( ofxUIEventArgs &e ) {

    // disable shortcuts when focus is on a textbox            
    if(hasKeyBoard) {

        _app->disableShortcuts();
        _isShortCutDisabled = true;
    }
    else if(_isShortCutDisabled) {

        _app->enableShortcuts();
        _isShortCutDisabled = false;
    }

    /*if(e.widget->getKind() == OFX_UI_WIDGET_TEXTINPUT) {

    ofxUITextInput *ti = (ofxUITextInput *) e.widget;
    if(ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_FOCUS)
    {
    _app->disableShortcuts();
    }
    else if(ti->getInputTriggerType() == OFX_UI_TEXTINPUT_ON_UNFOCUS)
    {
    _app->enableShortcuts();
    }
    }*/
    canvasEvent(e);
}

void Canvas::canvasEvent( ofxUIEventArgs &e ) {


}