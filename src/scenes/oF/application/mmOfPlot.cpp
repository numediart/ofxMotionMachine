#include "mmOfPlot.h"


using namespace MoMa;

ofPlot::ofPlot( const MoMa::SceneApp* pApp, int figureId ) : app(pApp)
{
    _figure.yMin = -1.0f;
    _figure.yMax = 1.0f;

    _figure.plot.clear();
    _figure.plotId = 0;
    _figure.yTop = (float)0;
    _figure.yBot = (float)ofGetHeight();
    curLowBoundTime = 0;
    curHighBoundTime = 0;
    curWidth = 0;
    mFigureId = figureId;
    nofFigure = 1;
	mLineWidth =1.5f;

}


ofPlot::~ofPlot()
{
    app = 0;
}

void ofPlot::update() {

	if (curLowBoundTime != app->getLowBoundTime() || curHighBoundTime != app->getHighBoundTime() || curWidth != ofGetWidth())
		this->updatePlot();
}
void ofPlot::updatePlot() {
    float figWidth = (float)ofGetHeight() / (float)(nofFigure);
    {

        _figure.yTop = (float)mFigureId * figWidth;
        _figure.yBot = (float)( mFigureId + 1 ) * figWidth;
    }

    curLowBoundTime = app->getLowBoundTime();
    curHighBoundTime = app->getHighBoundTime();
    curWidth = ofGetWidth();

    for( int f = 0; f < _figure.plot.size(); f++ ) {

        _figure.plot[f].line.clear(); // Clear first

        for( unsigned int n = 0; n < _figure.plot[f].data.nOfFrames(); n++ ) {

            float x = ofMap( _figure.plot[f].data.time( n ), app->getLowBoundTime(), app->getHighBoundTime(), 0, ofGetWidth() ); // Apply screen mapping here
            float y = ofMap( _figure.plot[f].data.get( n ), _figure.yMin, _figure.yMax, _figure.yBot - 5, _figure.yTop + 5 );
			_figure.plot[f].line.addVertex( ofVec3f( x, y ) ); // Add vertex
        }
    }
}


void ofPlot::draw() {
    if( mFigureId >= nofFigure )
        return;
    float figWidth = (float)ofGetHeight() / (float)nofFigure;

    if(fabs( _figure.yBot - (float)( mFigureId + 1 ) * figWidth)>1E-6)
        this->update();
    else
    if( fabs( curLowBoundTime - app->getLowBoundTime() )>1E-6 )
        this->update();
    else
    if( fabs( curHighBoundTime - app->getHighBoundTime() )>1E-6 )
        this->update();
    else
    if( curWidth != ofGetWidth() )
        this->update();


    float shift; // Values
    bool lowTextLoc = false;
    float nIndex = 23; // Names

                       // -- Display plot names --


    for( int f = 0; f < _figure.plot.size(); f++ ) {

        ofPushStyle();

        ofSetColor( _figure.plot[f].color ); // Grab name from plot name field
        ofDrawBitmapString( _figure.plot[f].name, 13, _figure.yTop + nIndex );

        ofPopStyle();

        nIndex += 14;
    }

    // -- Display zero line --

    ofPushStyle();

    ofSetLineWidth(mLineWidth);
    ofSetColor( 160, 160, 160, 128 );
    float zLn = ofMap( 0, _figure.yMin, _figure.yMax, _figure.yBot - 5, _figure.yTop + 5 );
    ofLine( 0, zLn, ofGetWidth(), zLn );

    ofPopStyle();

    // -- Display figures --

    for( int f = 0; f < _figure.plot.size(); f++ ) {

        //_figure.plot[f].line.clear(); // Clear first

        //for( unsigned int n = 0; n < _figure.plot[f].data.nOfFrames(); n++ ) {

        //    float x = ofMap( _figure.plot[f].data.time( n ), app->getLowBoundTime(), app->getHighBoundTime(), 0, ofGetWidth() ); // Apply screen mapping here
        //    float y = ofMap( _figure.plot[f].data.get( n ), _figure.yMin, _figure.yMax, _figure.yBot - 5, _figure.yTop + 5 );

        //    _figure.plot[f].line.addVertex( ofVec2f( x, y ) ); // Add vertex
        //}

        ofPushStyle();

        ofSetLineWidth(mLineWidth);
        ofSetColor( _figure.plot[f].color ); 
        _figure.plot[f].line.draw();

        ofPopStyle();
    }

    // -- Display playback head --

    ofPushStyle();

    double appAtTime = app->getAppTime(); // Sync fig time reading with application time
    float appTimeX = ofMap( appAtTime, app->getLowBoundTime(), app->getHighBoundTime(), 0, ofGetWidth(), true );

    ofSetLineWidth(mLineWidth); ofSetColor( 160, 160, 160, 128 ); // Draw PB time line
    ofLine( appTimeX, 0, appTimeX, ofGetHeight() ); // horizontal line & value @ bottom
    ofDrawBitmapString( ofToString( appAtTime ), appTimeX + 6, _figure.yBot - 5 );

    ofPopStyle();

    for( int f = 0; f < _figure.plot.size(); f++ ) {

        // Compute plot-specific index at which we can grab plot value
        float appAtIdx = _figure.plot[f].data.nearestIndex( appAtTime );

        ofPoint pnt = _figure.plot[f].line.getPointAtIndexInterpolated( appAtIdx ); // Apply screen mapping here
        float value = ofMap( pnt.y, _figure.yBot - 5, _figure.yTop + 5, _figure.yMin, _figure.yMax );

        if( lowTextLoc == false ) { shift = -3.0f; lowTextLoc = true; }
        else { shift = +11.0f; lowTextLoc = false; } // Alternating

        ofPushStyle();

        ofSetColor( _figure.plot[f].color ); ofSetLineWidth(mLineWidth);
        ofDrawBitmapString( ofToString( value ), pnt.x + 6, pnt.y + shift );
        ofNoFill(); ofCircle( pnt, 5 ); // Bubble and text

        ofPopStyle();
    }
}


void ofPlot::add( const TimedVec &tvec, int hue, std::string name ) {

    Plot plot;

    plot.data = tvec; plot.name = name;
    plot.color.setHsb( hue, 255, 255, 128 );

    if( tvec.nOfFrames() > 1 ) {

        if( _figure.yMin > (float)tvec.getData().min() ) _figure.yMin = (float)tvec.getData().min();
        if( _figure.yMax < (float)tvec.getData().max() ) _figure.yMax = (float)tvec.getData().max();

        _figure.plot.push_back( plot );
        _figure.plotId++;
    }
    this->updatePlot();
}

void ofPlot::add( const TimedVec &tvec, std::string name ) {
    _figure.hue = ( 150 + 40 * _figure.plotId ) % 255;
    add( tvec, _figure.hue, name ); // Automatic hue
} // TimedVec (no hue)


void ofPlot::add( const TimedMat &tmat, std::string name  ) {
    for( int r = 0; r < tmat.nOfElems(); r++ ) {

        TimedVec tvec = tmat.elem( r );
        add( tvec, name + " ( " + ofToString( r ) + " )" );
    }

} // TimedMat

void ofPlot::add( const Trace &trace ) {
    add( trace.position, trace.name() + " ( position )" );


} // Trace
void ofPlot::add( const BoneTrace &trace ) {
    
    add( trace.position, trace.name() + " ( position )" );

    if( trace.hasRotation() ) {

        add( trace.rotation, trace.name() + " ( rotation )" );
    }
} // BoneTrace

void ofPlot::add( const arma::vec &data,  int hue, std::string name ) {
    TimedVec tvec;

    tvec.setData( app->frameRate, data );
    add( tvec, hue, name );
}
void ofPlot::add( const arma::vec &data, std::string name ) {
    TimedVec tvec;

    tvec.setData( app->frameRate, data );
    add( tvec, name );
} // Vec
void ofPlot::add( const arma::mat &data, string name ) {

    TimedMat tmat;

    tmat.setData( app->frameRate, data );
    add( tmat, name );
} // and Mat

void ofPlot::clean() {
    _figure.yMin = -1.0f;
    _figure.yMax = 1.0f;

    _figure.plot.clear();
    _figure.plotId = 0;

}