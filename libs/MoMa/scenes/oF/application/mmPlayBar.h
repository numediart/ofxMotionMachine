/** FiguresView class
*
* Created by Mickaël Tits on 28/07/2015
*
*/

#ifndef __mmPlayBar__
#define __mmPlayBar__

namespace MoMa { 

    class PlayBar;
    //class Canvas;
}

#include "mmCanvas.h"

namespace MoMa {

    //class SceneApp;
    //class Canvas;

    class PlayBar : public MoMa::Canvas {

    public:

        PlayBar( SceneApp *app, MoMa::Position position = DEFAULT, MoMa::Position alignment = MoMa::DEFAULT, MoMa::Canvas *parentUI = NULL, bool minified = false );

		
        void update();
        void canvasEvent( ofxUIEventArgs &e );
		void initCanvas();

    protected:
        
        SceneApp *app;
		ofxUIRadio *playRadio;
		ofxUIImageToggle *bt_play, *bt_pause;
		ofxUIImageButton *bt_stop;
    };
}

#endif