#include "mmCam.h"

// when an ofEasyCam is moving due to momentum, this keeps it
// from moving forever by assuming small values are zero.
float minDifference = 0.1e-3f;

//----------------------------------------
mmCam::mmCam(){
    lastDistance = 0;
    drag = 0.9f;
    sensitivityRot = 1.0f;//when 1 moving the mouse from one side to the other of the arcball (min(viewport.width, viewport.height)) will rotate 180degrees. when .5, 90 degrees.
    sensitivityXY = 1.0f;
    sensitivityZ = 2.0f;
    
    bMouseInputEnabled = false;
    bDoRotate = false;
    bApplyInertia = false;
    bDoZoom = false;
    bDoTranslate = false;
    bValidClick = false;
    bEnableMouseMiddleButton = true;
    doTranslationKey = 'm';
    
    reset();
    enableMouseInput();
}

//----------------------------------------
mmCam::~mmCam(){
    disableMouseInput();
}
//----------------------------------------
void mmCam::update(ofEventArgs & args){
    if(bMouseInputEnabled){
    
        rotationFactor = sensitivityRot * 180 / min(viewport.width, viewport.height);
        if (bMouseInputEnabled) {
            updateMouse();
        }
        
        if (bDoRotate) {
            updateRotation();
        }else if (bDoTranslate) {
            updateTranslation();
        }
    }
}
//----------------------------------------
void mmCam::begin(ofRectangle viewport){
    this->viewport = viewport;
    ofCamera::begin(viewport);  
}

//----------------------------------------
void mmCam::reset(){
    target.resetTransform();
    
    target.setPosition(0, 0, 0);
    lookAt(target, ofVec3f(0, 0, 1));
    
    resetTransform();
    setPosition(0, 0, lastDistance);
    
        
    xRot = 0;
    yRot = 0;
    zRot = 0;
    
    moveX = 0;
    moveY = 0;
    moveZ = 0;
}
//----------------------------------------
void mmCam::setTarget(const ofVec3f& targetPoint){
    target.setPosition(targetPoint);
    lookAt(target, ofVec3f(0, 0, 1)); // see upside-down !
}
//----------------------------------------
void mmCam::setTarget(ofNode& targetNode){
    target = targetNode;
    lookAt(target, ofVec3f(0, 0, 1)); // see upside-down !
}
//----------------------------------------
ofNode& mmCam::getTarget(){
    return target;
}
//----------------------------------------
void mmCam::setDistance(float distance){
    setDistance(distance, true);
}
//----------------------------------------
void mmCam::setDistance(float distance, bool save){//should this be the distance from the camera to the target?
    if (distance > 0.0f){
        if(save){
            this->lastDistance = distance;
        }
        setPosition(target.getPosition() + (distance * getZAxis()));
    }
}
//----------------------------------------
float mmCam::getDistance() const {
    return target.getPosition().distance(getPosition());
}
//----------------------------------------
void mmCam::setDrag(float drag){
    this->drag = drag;
}
//----------------------------------------
float mmCam::getDrag() const {
    return drag;
}
//----------------------------------------
void mmCam::setTranslationKey(char key){
    doTranslationKey = key;
}
//----------------------------------------
char mmCam::getTranslationKey(){
    return doTranslationKey;
}
//----------------------------------------
void mmCam::enableMouseInput(){
    if(!bMouseInputEnabled){
        bMouseInputEnabled = true;
        ofAddListener(ofEvents().update , this, &mmCam::update);
    }
}
//----------------------------------------
void mmCam::disableMouseInput(){
    if(bMouseInputEnabled){
        bMouseInputEnabled = false;
        ofRemoveListener(ofEvents().update, this, &mmCam::update);
    }
}
//----------------------------------------
bool mmCam::getMouseInputEnabled(){
    return bMouseInputEnabled;
}
//----------------------------------------
void mmCam::enableMouseMiddleButton(){
    bEnableMouseMiddleButton = true;
}
//----------------------------------------
void mmCam::disableMouseMiddleButton(){
    bEnableMouseMiddleButton = false;
}
//----------------------------------------
bool mmCam::getMouseMiddleButtonEnabled(){
    return bEnableMouseMiddleButton;
}
//----------------------------------------
void mmCam::updateTranslation(){
    if (bApplyInertia) {
        moveX *= drag;
        moveY *= drag;
        moveZ *= drag;
        if (ABS(moveX) <= minDifference && ABS(moveY) <= minDifference && ABS(moveZ) <= minDifference) {
            bApplyInertia = false;
            bDoTranslate = false;
            bDoZoom = false;
        }
    }
    ofVec3f translation = (getXAxis() * moveX) + (getYAxis() * moveY) + (getZAxis() * moveZ);
    move(translation);
    if(!bDoZoom) {
        target.move(translation);
    }
    moveLikeHuman();
}   
//----------------------------------------
void mmCam::updateRotation(){
    if (bApplyInertia) {
        xRot *=drag; 
        yRot *=drag;
        zRot *=drag;
        
        if (ABS(xRot) <= minDifference && ABS(yRot) <= minDifference && ABS(zRot) <= minDifference) {
            bApplyInertia = false;
            bDoRotate = false;
        }
    }
    curRot = ofQuaternion(xRot, ofCamera::getXAxis(), yRot, ofCamera::getYAxis(), zRot, ofCamera::getZAxis());
    setPosition((ofCamera::getGlobalPosition()-target.getGlobalPosition())*curRot +target.getGlobalPosition());
    rotateLikeHuman();
}
//----------------------------------------
void mmCam::updateMouse(){
    mouse = ofVec2f(ofGetMouseX(), ofGetMouseY());
    if(viewport.inside(mouse.x, mouse.y) && !bValidClick && ofGetMousePressed()){
                
        if ((bEnableMouseMiddleButton && ofGetMousePressed(OF_MOUSE_BUTTON_MIDDLE)) || ofGetKeyPressed(doTranslationKey)){
            bDoTranslate = true;
            bDoRotate = false;
            bApplyInertia = false;
            bDoZoom = false;
        }else if (ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT)) {
            bDoTranslate = true;
            bDoRotate = false;
            bApplyInertia = false;
            bDoZoom = true;
        }else if (ofGetMousePressed(OF_MOUSE_BUTTON_LEFT)) {
            bDoTranslate = false;
            bDoRotate = true;
            bApplyInertia = false;
            bDoZoom = false;
        }
        lastMouse = mouse;
        bValidClick = true;
        bApplyInertia = false;
    }
    
    if (bValidClick) {
        if (!ofGetMousePressed()) {
            bApplyInertia = true;
            bValidClick = false;
        }else {
            int vFlip;
            if(isVFlipped()){
                vFlip = -1;
            }else{
                vFlip =  1;
            }

            mouseVel = mouse  - lastMouse;
            
            if (bDoTranslate) {
                moveX = 0;
                moveY = 0;
                moveZ = 0;
                if (ofGetMousePressed(OF_MOUSE_BUTTON_RIGHT)) {
                    moveZ = mouseVel.y * sensitivityZ * (getDistance() + FLT_EPSILON)/ viewport.height;             
                }else {
                    moveX = -mouseVel.x * sensitivityXY * (getDistance() + FLT_EPSILON)/viewport.width;
                    moveY = vFlip * mouseVel.y * sensitivityXY * (getDistance() + FLT_EPSILON)/viewport.height;
                }
            }else {
                xRot = vFlip * -mouseVel.y * rotationFactor;
                yRot = -mouseVel.x * rotationFactor;
                zRot = 0;
            }
            lastMouse = mouse;
        }
    }
}

void mmCam::moveLikeHuman() {
    
    // a human cannot see under the floor !
    ofVec3f pos = getGlobalPosition();
    if (pos.z < 0)
    {
        pos.z = 0;
        setPosition(pos);
    }
}

void mmCam::rotateLikeHuman() {

    moveLikeHuman();
    
    // a human always see upside-down !
    lookAt(target, ofVec3f(0, 0, 1));
}

