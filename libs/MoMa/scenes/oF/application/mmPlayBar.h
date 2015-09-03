/** PlayBar class
*
* Created by Mickaël Tits on 20/08/2015
*
*/

#ifndef __mmPlayBar__
#define __mmPlayBar__

/*namespace MoMa { 

    class PlayBar;
    //class Canvas;
}*/

#include "mmCanvas.h"

namespace MoMa {

    //class SceneApp;
    //class Canvas;

    class PlayBar : public MoMa::Canvas {

    public:

        PlayBar( SceneApp *app, MoMa::Position position = DEFAULT, MoMa::Position alignment = MoMa::DEFAULT, MoMa::Canvas *parentUI = NULL, int group = 1, bool minified = false );


        void update();
        void canvasEvent( ofxUIEventArgs &e );
        void initCanvas();

    protected:

        SceneApp *app;
        ofxUIRadio *playRadio;
        //ofxUIImageToggle *bt_play, *bt_pause;
        ofxUIImageToggle *bt_scrub;
        ofxUIImageButton *bt_play, *bt_stop, *bt_nextFrame, *bt_prevFrame;
        bool playingState;
        ofxUITextArea *txt_index, *txt_time;
    };
}

#endif