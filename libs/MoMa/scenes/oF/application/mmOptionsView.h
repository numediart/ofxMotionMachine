/** OptionsView class
*
* Created by Mickaël Tits on 02/09/2015
*
*/

#ifndef __mmOptionsView__
#define __mmOptionsView__

/*namespace MoMa { 

    class Options;
    //class Canvas;
}*/

#include "mmCanvas.h"

namespace MoMa {

    //class SceneApp;
    //class Canvas;

    class Options : public MoMa::Canvas {

    public:

        Options( SceneApp *app, MoMa::Position position = DEFAULT, MoMa::Position alignment = MoMa::DEFAULT, MoMa::Canvas *parentUI = NULL, int group = 1, bool minified = false );


        void update();
        void canvasEvent( ofxUIEventArgs &e );
        //void initCanvas();

    protected:

        SceneApp *app;
        ofxUIToggle *toggle3DScene, *toggleGround, *toggleNodeNames, *toggleAnnotations, *toggleFigure, *toggleCaption, *toggleTimeline;
    };
}

#endif