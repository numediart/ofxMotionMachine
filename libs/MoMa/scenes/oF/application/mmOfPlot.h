/**
*
*  @file mmOfPlot.h
 * @brief Utilitary functions to use MoMa in OF.
*  @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#ifndef __mmOfPlot__
#define __mmOfPlot__
namespace MoMa { class ofPlot; }

#include <armadillo>

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxUI.h"

#include "MoMa.h"
#include "mmCam.h"
#include "mmOfBone.h"
#include "mmOfUtils.h"
#include "mmCanvas.h"

#include "mmSceneApp.h"

namespace MoMa {
    class ofPlot
    {
        /**
        * A class that provides a way to plot 2D data curves in a MotionMachine OFX windows
        *
        *
        * example to use it in a ofApp:
        *
        * in the class header(ofApp.h):
        * shared_ptr<MoMa::ofPlot> plot;
        * in the code (ofApp.cpp)
        * in ofApp::setup() method:
        * plot=make_shared<MoMa::ofPlot>( this );// plot need to get information from ofApp.h
        * plot->add(dataName);
        * In ofApp::scene2d()
        * plot->draw()
        *
        */
    public:
        ofPlot( const MoMa::SceneApp* pApp ,int figureId=0 );
        ~ofPlot();
        void update();
        void draw(  );

        void add( const TimedVec &tvec, int hue, std::string name = "" ); // TimedVec
        void add( const TimedVec &tvec, std::string name = "" ); // TimedVec (no hue)
        void add( const TimedMat &tmat, std::string name = "" ); // TimedMat

        void add( const Trace &trace ); // Trace
        void add( const BoneTrace &boneTrace ); // BoneTrace

        void add( const arma::vec &data, int hue, std::string name = "" );
        void add( const arma::vec &data, std::string name = "" ); // Vec
        void add( const arma::mat &data, string name = "" ); // and Mat

        void clean();
        void setNofFigure( int noff ) { nofFigure = noff; };
    protected:
        const MoMa::SceneApp* app;
        Figure _figure;
        double curLowBoundTime;
        double curHighBoundTime;
        int curWidth;
        int nofFigure;
        int mFigureId;
    };
}
#endif//__mmOfPlot__