/** PlayBar class
*
* Created by Mickaël Tits on 20/08/2015
*
*/

#ifndef __mmTextDialog__
#define __mmTextDialog__

#include "mmCanvas.h"

namespace MoMa {

    class TextDialog : public MoMa::Canvas {

    public:

        TextDialog(SceneApp *app);

        void canvasEvent(ofxUIEventArgs &e);
        std::string getText();
        void set(std::string &textinput);
        void setOff();
        void initCanvas();

    protected:

        SceneApp *app;
        std::string *text;
        std::string oldtext;
        bool isTextEntered;
        ofxUITextInput* txtInput;
    };

}
#endif