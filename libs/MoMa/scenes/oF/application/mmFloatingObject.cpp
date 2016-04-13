/** FloatingObject : Basic UI FloatingObject specific to the MoMa.
*
* Created by Mickaël Tits on 08/03/2016
* @file mmFloatingObject.cpp
* @brief MotionMachine source file for FloatingObject class.
* @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#include "mmFloatingObject.h"

using namespace std;
using namespace MoMa;

//vector<FloatingObject*> FloatingObject::objects;

//int FloatingObject::savedMode = -1;
bool FloatingObject::isFocusFree = true;
vector<FloatingObject*> FloatingObject::objects;
//vector<int> FloatingObject::order;

//--------------------------------------------------------------
FloatingObject::FloatingObject(SceneApp *app, std::string title, int x, int y, int width, int height, bool minified) :
    _app(app)
{
    verbose = false;
    savedWidth = width;
    titleHeight = DEFAULT_TITLE_HEIGHT;
    titleWidth = DEFAULT_TITLE_WIDTH;
    closingButtonWidth = CLOSIG_BUTTON_WIDTH;
    drawObject = true;
    lastTitleHitTime = 0.0;
    lastHitTime = 0.0;
    deltaTime = TIME_DELAY_FOR_DOUBLE_CLICK;
    titlePressed = false;
    hasFocus = false;
    titleColor = MoMa::Turquoise;
    closingButtonColor = MoMa::Red;
    backGroundColor = ofColor(150);
    this->minified = minified;
    setPosition(x, y);
    setTitle(title);
    setWidth(width);
    setHeight(height);
    isBackGround = true;
    keyEnabled = false;
    mouseEnabled = false;
    appEnabled = false;
    resizeEnabled = false;
    closingButtonFocused = false;
    resizing = false;
    onCorner = false;
    objectResizeEnabled = true;
    enableAllEvents();
    objects.push_back(this);
    //order.push_back(order.size());
    //index = objects.size();    
    mouseDX = 0;
    mouseDY = 0;
}
//--------------------------------------------------------------
FloatingObject::~FloatingObject() {

    if (verbose) cout << "deleting object :" << title << endl;
    disableAllEvents();
    setFocus(false);

    objects.erase(find(objects.begin(), objects.end(), this)); //Erase this object from objects static vector
    //order.erase(order.begin() + index);
}
//--------------------------------------------------------------
/*void FloatingObject::deleteAllObjects() {

    vector<FloatingObject*> tmp = objects;

    for (int i = 0; i < tmp.size(); ++i) {

        delete tmp[i];
    }
}*/

//--------------------------------------------------------------
void FloatingObject::enableAllEvents() {

    if (isFocusFree || hasFocus) {

        enableMouseEvents();
        enableKeyEvents();
    }

    enableResizeEvents();
    enableAppEvents();

    if (!isFocusFree && !hasFocus) {

        //Repush on front of the call stack the focused object!
        for (int i = 0; i < objects.size(); ++i) {

            if (objects[i]->isFocused()) {

                objects[i]->disableAllEvents();
                objects[i]->enableAllEvents();
            }
        }
    }
}
//--------------------------------------------------------------
void FloatingObject::disableAllEvents() {

    disableMouseEvents();
    disableKeyEvents();
    disableResizeEvents();
    disableAppEvents();
}
//--------------------------------------------------------------
void FloatingObject::enableMouseEvents() {

    if (!mouseEnabled) {

        ofAddListener(ofEvents().mousePressed, this, &FloatingObject::_mousePressed, OF_EVENT_ORDER_BEFORE_APP);
        ofAddListener(ofEvents().mouseMoved, this, &FloatingObject::_mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
        ofAddListener(ofEvents().mouseDragged, this, &FloatingObject::_mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
        ofAddListener(ofEvents().mouseReleased, this, &FloatingObject::_mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
        mouseEnabled = true;
    }
}
//--------------------------------------------------------------
void FloatingObject::disableMouseEvents() {

    if (mouseEnabled) {

        ofRemoveListener(ofEvents().mousePressed, this, &FloatingObject::_mousePressed, OF_EVENT_ORDER_BEFORE_APP);
        ofRemoveListener(ofEvents().mouseMoved, this, &FloatingObject::_mouseMoved, OF_EVENT_ORDER_BEFORE_APP);
        ofRemoveListener(ofEvents().mouseDragged, this, &FloatingObject::_mouseDragged, OF_EVENT_ORDER_BEFORE_APP);
        ofRemoveListener(ofEvents().mouseReleased, this, &FloatingObject::_mouseReleased, OF_EVENT_ORDER_BEFORE_APP);
        mouseEnabled = false;
    }
}
//--------------------------------------------------------------
void FloatingObject::enableKeyEvents() {

    if (!keyEnabled) {

        ofAddListener(ofEvents().keyPressed, this, &FloatingObject::_keyPressed, OF_EVENT_ORDER_BEFORE_APP);
        ofAddListener(ofEvents().keyReleased, this, &FloatingObject::_keyReleased, OF_EVENT_ORDER_BEFORE_APP);
        keyEnabled = true;
    }
}
//--------------------------------------------------------------
void FloatingObject::disableResizeEvents() {

    if (resizeEnabled) {

        ofRemoveListener(ofEvents().keyPressed, this, &FloatingObject::_keyPressed, OF_EVENT_ORDER_BEFORE_APP);
        ofRemoveListener(ofEvents().keyReleased, this, &FloatingObject::_keyReleased, OF_EVENT_ORDER_BEFORE_APP);
        resizeEnabled = false;
    }
}
//--------------------------------------------------------------
void FloatingObject::enableResizeEvents() {

    if (!resizeEnabled) {

        ofAddListener(ofEvents().windowResized, this, &FloatingObject::_windowResized);
        resizeEnabled = true;
    }
}
//--------------------------------------------------------------
void FloatingObject::disableKeyEvents() {

    if (keyEnabled) {

        ofRemoveListener(ofEvents().windowResized, this, &FloatingObject::_windowResized);
        keyEnabled = false;
    }
}
//--------------------------------------------------------------
void FloatingObject::enableAppEvents() {

    if (!appEnabled) {

        if (verbose) cout << "enabling _update of " << title << endl;
        ofAddListener(ofEvents().update, this, &FloatingObject::_update);
        ofAddListener(ofEvents().draw, this, &FloatingObject::_draw, OF_EVENT_ORDER_AFTER_APP);
        appEnabled = true;
    }
}
//--------------------------------------------------------------
void FloatingObject::disableAppEvents() {

    if (appEnabled) {

        if (verbose) cout << "disabling _update of " << title << endl;
        ofRemoveListener(ofEvents().update, this, &FloatingObject::_update);
        ofRemoveListener(ofEvents().draw, this, &FloatingObject::_draw, OF_EVENT_ORDER_AFTER_APP);
        appEnabled = false;
    }
}
//--------------------------------------------------------------
void FloatingObject::setPosition(int x, int y)
{
    this->x = x;
    this->y = y;
}
//--------------------------------------------------------------
void FloatingObject::_keyPressed(ofKeyEventArgs &key)
{
    if (hasFocus) {

        keyPressed(key.key);
    }
}
//--------------------------------------------------------------
void FloatingObject::_keyReleased(ofKeyEventArgs &key)
{
    if (hasFocus) {

        keyReleased(key.key);
    }
}
//--------------------------------------------------------------
void FloatingObject::_mouseMoved(ofMouseEventArgs & evt) {

    if (isInsideClosingButton(evt.x, evt.y)) { //Over Closing Button

        closingButtonFocused = true;
    }
    else {

        closingButtonFocused = false;
    }

    //Focus on move!
    /*if (isInsideObjectOrTitle(evt.x, evt.y)) { //Over overall object

        setFocus(true);
    }
    else {

        setFocus(false);
    }*/


    if (!minified) {

        if (isOnCorner(evt.x, evt.y)) { //Resizing

            onCorner = true;
        }
        else onCorner = false;

        mouseMoved(evt.x, evt.y);
    }
}
//--------------------------------------------------------------
void FloatingObject::_mousePressed(ofMouseEventArgs &evt)
{
    if (isInsideClosingButton(evt.x, evt.y)) { //Inside Closing Button

        hide();
        return;
    }

    if (isInsideTitle(evt.x, evt.y)) { //Inside Title

        titlePressed = true;
        if ((ofGetElapsedTimef() - lastTitleHitTime) < deltaTime)
        {
            toggleMinified();
        }
        lastTitleHitTime = ofGetElapsedTimef();

        mouseDX = evt.x - x;
        mouseDY = evt.y - y;
    }

    //Focus on press
    if (isInsideObjectOrTitle(evt.x, evt.y)) { //Inside overall object

        setFocus(true);
    }
    else {

        setFocus(false);
    }

    if (!minified) {

        if (isOnCorner(evt.x, evt.y)) { //Resizing

            resizing = true;
            setFocus(true);
        }

        else {

            mousePressed(evt.x, evt.y, evt.button);
        }
    }
}
//--------------------------------------------------------------
void FloatingObject::_mouseDragged(ofMouseEventArgs &evt)
{
    if (titlePressed) { //Dragging object

        x = evt.x - mouseDX;
        y = evt.y - mouseDY;
    }

    if (!minified) {

        if (resizing) { //Resizing object

            //width = max(DEFAULT_FLOATING_OBJECT_WIDTH,int(evt.x - x));
            width = max(10, int(evt.x - x));
            height = max(10, int(evt.y - y));
        }

        else {

            mouseDragged(evt.x, evt.y, evt.button);
        }
    }
}
//--------------------------------------------------------------
void FloatingObject::_mouseReleased(ofMouseEventArgs &evt)
{
    titlePressed = false;
    resizing = false;

    if (!minified) {

        if ((ofGetElapsedTimef() - lastHitTime) < deltaTime) //double-clicked
        {
            doubleClicked(evt.x, evt.y, evt.button);
        }
        else mouseReleased(evt.x, evt.y, evt.button);

        lastHitTime = ofGetElapsedTimef();

    }
}
//--------------------------------------------------------------
void FloatingObject::_windowResized(ofResizeEventArgs & evt) {

    windowResized(evt.width, evt.height);
}
//--------------------------------------------------------------
void FloatingObject::_draw(ofEventArgs & args) {

    if (drawObject) {

        ofPushStyle();
        ofPushMatrix();
        ofTranslate(x, y - titleHeight);
        // draw title
        ofFill();
        ofSetColor(titleColor, 200);
        int tmpW = max(width, titleWidth);
        if (minified) tmpW = titleWidth;
        ofDrawRectangle(0, 0, tmpW, titleHeight);
        ofSetColor(255);
        ofDrawBitmapString(title, 1, titleHeight - 3);

        // draw closing button
        ofSetColor(closingButtonColor, 200);
        if (closingButtonFocused) ofSetColor(closingButtonColor, 255);
        if (closingButtonWidth > titleHeight) closingButtonWidth = titleHeight;
        ofDrawRectangle(tmpW - 5 - closingButtonWidth, (titleHeight - closingButtonWidth) / 2, closingButtonWidth, closingButtonWidth);
        ofSetColor(255, 200);
        ofLine(tmpW - 5 - closingButtonWidth, (titleHeight - closingButtonWidth) / 2, tmpW - 5, (titleHeight + closingButtonWidth) / 2);
        ofLine(tmpW - 5 - closingButtonWidth, (titleHeight + closingButtonWidth) / 2, tmpW - 5, (titleHeight - closingButtonWidth) / 2);

        if (!minified) {

            ofTranslate(0, titleHeight); //We are at (x,y)
            // draw background
            ofFill();
            ofSetColor(backGroundColor, transparency());
            ofDrawRectangle(0, 0, width, height);

            // draw object
            draw();
        }

        //draw resizing indicator
        if (onCorner) {

            ofSetColor(MoMa::Red, 100);
            ofDrawCircle(width, height, 4);
        }
        ofPopMatrix();
        ofPopStyle();
    }
}
//--------------------------------------------------------------
void FloatingObject::_update(ofEventArgs &args) {

    update();
}
//--------------------------------------------------------------
void FloatingObject::toggleMinified() {

    minified = !minified;

    if (minified) {

        savedWidth = width;
        width = DEFAULT_FLOATING_OBJECT_WIDTH;
    }
    else width = savedWidth;
}
//--------------------------------------------------------------
void FloatingObject::setFocus(bool val) {

    if (verbose) {

        cout << "setFocus(" << val << ") on object " << title << ", for which hasFocus = " << hasFocus << ". isFocusFree = " << isFocusFree << endl;
    }

    if (_app->isExit) return; //Avoid bugs due to bad scenemode or canvas access

    if (val && hasFocus) { //Ensure the focused object is on front if it was not already (if another new object was opened for instance)

        //Push this object on front in the call stack (control events before, draw event after!)
        disableAllEvents();
        enableAllEvents();
    }

    if (val && isFocusFree) {//focus must be set, no other object already focused => we can set the focus

        hasFocus = true;
        isFocusFree = false;
        titleColor = MoMa::DarkTurquoise;

        //Push this object on front in the call stack (control events before, draw event after!)
        disableAllEvents();
        enableAllEvents();

        //Disable control of app, canvas and other objects 
        MoMa::Canvas::disableAllCanvas();
        _app->disableControl();
        lockOtherObjects(this);
    }

    if (!val && hasFocus) { //object is focused and must be unfocused

        //release the focus right
        hasFocus = false;
        isFocusFree = true;
        titleColor = MoMa::Turquoise;

        //Re-enable app, canvas and other objects 
        MoMa::Canvas::enableAllCanvas();
        _app->enableControl();
        unlockOtherObjects(this);

        //recall listeners as objects are now unlocked and could catch the focus!
        ofMouseEventArgs evt;
        evt.x = ofGetMouseX();
        evt.y = ofGetMouseY();

        ofNotifyEvent(ofEvents().mousePressed, evt);
    }

    /*if (val && !hasFocus) {

        hasFocus = true;
        titleColor = MoMa::DarkTurquoise;



        if (isFocusFree) { //First object focused! We disable app, canvas and other objects
            //savedMode = _app->activeMode; // save mode previous to click
            //_app->setActiveMode(-1); //_app->setActiveMode(-1); disable all controls (idem as MoMa::CANVAS actually!)
                                               //_app->isShortcut = false; //we do not want to block completely shortcuts of ofApp. setActiveMode to -1 is enough
            MoMa::Canvas::disableAllCanvas(); // disable canvas
            _app->disableControl();

            disableAppEvents();
            enableAppEvents(); // put the listener at the end of the stack for control!

            lockOtherObjects(this);
        }
        isFocusFree = false;
    }
    else if (!val && hasFocus) {

        hasFocus = false;
        titleColor = MoMa::Turquoise;
        isFocusFree = true;

        //_app->setActiveMode(savedMode);
        //_app->isShortcut = true;
        MoMa::Canvas::enableAllCanvas();
        _app->enableControl();
        unlockOtherObjects(this);
    }
        */
}
//--------------------------------------------------------------
void FloatingObject::hide() {

    toggleMinified();
    setFocus(false);
    drawObject = false;
    disableAllEvents();
}
//--------------------------------------------------------------
void FloatingObject::show() {

    if (minified) toggleMinified();
    drawObject = true;
    enableAllEvents();
}
//--------------------------------------------------------------
bool FloatingObject::isInsideObjectOrTitle(int mouseX, int mouseY) {

    //if (!isMinified()) return ofRectangle(x, y - titleHeight, max(width, titleWidth), height + titleHeight).inside(mouseX, mouseY);
    //else return isInsideTitle(mouseX, mouseY);

    return (isInsideObject(mouseX, mouseY) + isInsideTitle(mouseX, mouseY));
}
//--------------------------------------------------------------
bool FloatingObject::isInsideObject(int mouseX, int mouseY) {

    if (!isMinified()) return ofRectangle(x, y, width, height).inside(mouseX, mouseY);
    else return false;
}
//--------------------------------------------------------------
bool FloatingObject::isInsideTitle(int mouseX, int mouseY) {

    if (!isMinified()) return ofRectangle(x, y - titleHeight, max(width, titleWidth), titleHeight).inside(mouseX, mouseY);
    else return ofRectangle(x, y - titleHeight, titleWidth, titleHeight).inside(mouseX, mouseY);
}
//--------------------------------------------------------------
bool FloatingObject::isInsideClosingButton(int mouseX, int mouseY) {

    if (!isMinified()) return ofRectangle(x + max(width, titleWidth) - 5 - closingButtonWidth, y - (titleHeight + closingButtonWidth) / 2, closingButtonWidth, closingButtonWidth).inside(mouseX, mouseY);
    else return ofRectangle(x + titleWidth - 5 - closingButtonWidth, y - (titleHeight + closingButtonWidth) / 2, closingButtonWidth, closingButtonWidth).inside(mouseX, mouseY);
}
//--------------------------------------------------------------
bool FloatingObject::isOnCorner(int mouseX, int mouseY) {

    if (!objectResizeEnabled) return false;
    return ofRectangle(x + width - 4, y + height - 4, 8, 8).inside(mouseX, mouseY);
}
//--------------------------------------------------------------
void FloatingObject::lockOtherObjects(FloatingObject *thisObject) {

    for (int i = 0; i < objects.size(); ++i) {

        if (objects[i] != thisObject) {

            objects[i]->disableMouseEvents();
            objects[i]->disableKeyEvents();
        }
    }
}
//--------------------------------------------------------------
void FloatingObject::unlockOtherObjects(FloatingObject *thisObject) {

    for (int i = 0; i < objects.size(); ++i) {

        if (objects[i] != thisObject) {

            objects[i]->enableMouseEvents();
            objects[i]->enableKeyEvents();
        }
    }
}

int FloatingObject::transparency() {

    if (hasFocus) return 200;
    else return 50;
}