#include "mmBoundingBoxUI.h"

using namespace MoMa;

void BoundingBox::draw( Frame frame ) {
    
    ofCylinderPrimitive cyl;
    
    Node ctr = center( frame );
    float rad = width( frame )/2.0f;
    float hei = height( frame );
    
    cyl.setPosition( ctr.position(X), ctr.position(Y), ctr.position(Z) );
    cyl.setRadius( rad ); cyl.setHeight( hei ); cyl.setResolution( 10, 10 );
    cyl.setOrientation( ofVec3f( 90, 0, 0 ) );
    
    ofPushStyle();
    ofSetLineWidth( 1 );
    ofSetColor( ofGetStyle().color, 120 );
    cyl.drawWireframe();
    ofPopStyle();
}
