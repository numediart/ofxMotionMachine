/** FiguresView class
*
* Created by Mickaël Tits on 28/07/2015
*
*/

#ifndef __mmMenuView__
#define __mmMenuView__

namespace MoMa { 

    class MenuView;
    //class Canvas;
}

#include "mmCanvas.h"

namespace MoMa {

    //class SceneApp;
    //class Canvas;

    class MenuView : public MoMa::Canvas {

    public:

        MenuView( SceneApp *app, MoMa::Position position = DEFAULT, MoMa::Position alignment = MoMa::DEFAULT, MoMa::Canvas *parentUI = NULL, bool minified = false );

        void update();
        void canvasEvent( ofxUIEventArgs &e );

    protected:
        
        SceneApp *app;
        ofxUITextArea *activeMode;
        ofxUIRadio *modeRadio, *playRadio;
    };
}

#endif
