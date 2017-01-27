#include "mmTextDialog.h"

using namespace MoMa;
using namespace std;

TextDialog::TextDialog(SceneApp *_app) :
    app(_app),
    Canvas(_app, "Text Dialog", NULL, 1) {
    
    addSpacer();
    txtInput = addTextInput("Text Input", "", OFX_UI_FONT_SMALL);
    addButton("OK", false);
    setWidgetPosition(OFX_UI_WIDGET_POSITION_RIGHT);
    addButton("Cancel", false);

    text = NULL;

    initCanvas();

}

void TextDialog::initCanvas() {

    setVisible(false);
    autoSizeToFitWidgets();
    setMinified(false);
    setPosition(round(ofGetWidth() / 2 - getRect()->getHalfWidth()), round(ofGetHeight() / 2 - getRect()->getHalfHeight()));
}

string TextDialog::getText() {

    return *text;
}

void TextDialog::set(string &textinput) {

    setVisible(true);    
    text = &textinput;
    oldtext = *text;
    *text = "";
    txtInput->setTextString(*text);
    txtInput->setFocus(true);
}

void TextDialog::setOff() {

    setVisible(false);
    text = NULL;
}

void TextDialog::canvasEvent(ofxUIEventArgs &e) {

    string name = e.widget->getName();

    if (name == "Text Input" && text)
    {
        ofxUITextInput *ti = (ofxUITextInput *)e.widget;

        *text = ti->getTextString();


        int test = ti->getInputTriggerType();
        if (test == OFX_UI_TEXTINPUT_ON_ENTER && !ti->isFocused()) { //Enter pressed to unfocus the text input

            setOff(); //button released
        }



        isTextEntered = true;

    }

    else if (name == "OK")
    {
        ofxUIButton* button = (ofxUIButton*)(e.widget);

        if (!button->getValue()) {

            setOff(); //button released
        }
    }
    else if (name == "Cancel" && text)
    {
        ofxUIButton* button = (ofxUIButton*)(e.widget);

        if (!button->getValue()) //button released
        {
            isTextEntered = false;
            *text = oldtext;
            setOff();
        }
    }
}
