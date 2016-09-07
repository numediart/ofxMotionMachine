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
    public:
        ofPlot( const MoMa::SceneApp* pApp );
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
    protected:
        const MoMa::SceneApp* app;
        Figure _figure;
        double curLowBoundTime;
        double curHighBoundTime;
        int curWidth;
    };
}
#endif//__mmOfPlot__