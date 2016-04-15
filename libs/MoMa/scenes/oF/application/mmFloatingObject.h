/** FloatingObject : Basic UI FloatingObject specific to the MoMa.
*
* Created by Mickaël Tits on 08/03/2016, inspired from https://github.com/memo/ofxMSAInteractiveObject and https://github.com/genekogan/ofxSpreadsheet
* @author Mickaël Tits
* @file mmFloatingObject.h
* @brief MotionMachine header file for FloatingObject class.
* @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#ifndef __mmFloatingObject__
#define __mmFloatingObject__

#define DEFAULT_FLOATING_OBJECT_WIDTH 211
#define DEFAULT_FLOATING_OBJECT_HEIGHT 200
#define TIME_DELAY_FOR_DOUBLE_CLICK 0.35
#define DEFAULT_TITLE_HEIGHT 25
#define DEFAULT_TITLE_WIDTH 211
#define CLOSING_BUTTON_WIDTH 15
#define SAVING_BUTTON_WIDTH 15


/*namespace MoMa { 

    class FloatingObject;
}*/

#include "mmSceneApp.h"
#include "ofMain.h"

namespace MoMa {     

    class FloatingObject {

    public:  

        FloatingObject( SceneApp *app, std::string title = "unnamed", int x = 0, int y = 0, int width = DEFAULT_FLOATING_OBJECT_WIDTH, int height = DEFAULT_FLOATING_OBJECT_HEIGHT, bool minified = false );

        ~FloatingObject();

        void enableAllEvents();				// enable all event callbacks
        void disableAllEvents();			// disable all event callbacks
        void enableMouseEvents();			// call this if object should receive mouse events (default)
        void disableMouseEvents();			// call this if object doesn't need to receive mouse events
        void enableKeyEvents();				// call this if object should receive key events (default)
        void disableKeyEvents();			// call this if object doesn't need to receive key events
        void enableAppEvents();				// call this if object should update/draw automatically	(default)
        void disableAppEvents();			// call this if object doesn't need to update/draw automatically
        void enableResizeEvents();				// call this if object should receive resize events (default)
        void disableResizeEvents();			// call this if object doesn't need to receive resize events

        void enableResize() { objectResizeEnabled = true; }
        void disableResize() { objectResizeEnabled = false; }

        void setPosition(int x, int y);

        //void setActive(bool active);
        //bool getActive() { return active; }

        void setTitle(std::string title) { this->title = title; }
        std::string getTitle() { return title; }

        void hide();
        void show();

        inline bool isVisible() { return drawObject; }
        inline bool isFocused() { return hasFocus; }
        inline bool isMinified() { return minified; }
        inline bool isInsideObject(int x, int y);
        inline bool isInsideObjectOrTitle(int x, int y);
        inline bool isInsideTitle(int x, int y);
        inline bool isInsideClosingButton(int x, int y);
        inline bool isInsideSavingButton(int x, int y);
        inline bool isOnCorner(int x, int y);
        inline int getX() { return x; }
        inline int getY() { return y; }
        inline void setWidth(int w) { width = w; }
        inline void setHeight(int h) { height = h; }
        inline int getWidth() { return width; }
        inline int getHeight() { return height; }
        inline int totHeight() { return height + titleHeight; }

        void disableBackGround() { isBackGround = false; }
        void enableBackGround() { isBackGround = true; }
        void setBackGround(ofColor color) { backGroundColor = color; }
        void saveAsImage();
        
        //virtual void setup(void) {}
        virtual void update(void) {}
        //virtual void exit(void) {}

        virtual void draw(void) {};

        virtual void keyPressed(int key) {}
        virtual void keyReleased(int key) {}

        virtual void mousePressed(int x, int y, int button) {}
        virtual void mouseReleased(int x, int y, int button) {}
        virtual void mouseDragged(int x, int y, int button) {}
        virtual void mouseMoved(int x, int y) {}
        virtual void doubleClicked(int x, int y, int button) {}

        virtual void windowResized(int w, int h) {}
        virtual void windowReduced() {}

        virtual void dragEvent(ofDragInfo dragInfo) {}

        //static void deleteAllObjects();
        static void lockOtherObjects(FloatingObject* thisObject);
        static void unlockOtherObjects(FloatingObject* thisObject);

        int x, y, width, height, savedWidth;
        int mouseDX, mouseDY;
        int titleHeight;
        int titleWidth;
        ofColor titleColor;
        ofColor closingButtonColor;
        ofColor savingButtonColor;
        ofColor backGroundColor;
        int transparency();

    private:

        void toggleMinified();
        void setFocus(bool val);

        void _keyPressed(ofKeyEventArgs &evt);
        void _keyReleased(ofKeyEventArgs &evt);
        void _mouseMoved(ofMouseEventArgs &evt);
        void _mousePressed(ofMouseEventArgs &evt);
        void _mouseDragged(ofMouseEventArgs &evt);
        void _mouseReleased(ofMouseEventArgs &evt);
        void _windowResized(ofResizeEventArgs &evt);        
        void _draw(ofEventArgs &args);
        void _update(ofEventArgs &args);

        static std::vector<FloatingObject*> objects;
        //static std::vector<int> order; // tells in which order the objects are displayed
        //static int savedMode;
        static bool isFocusFree;
        int index;

        bool keyEnabled, mouseEnabled, appEnabled, resizeEnabled;        
        
        bool minified;
        bool titlePressed;
        bool hasFocus;
        bool drawObject;
        bool closingButtonFocused;
        bool savingButtonFocused;
        bool resizing;
        bool onCorner;
        bool objectResizeEnabled;
        bool isBackGround;
        float lastTitleHitTime;
        float lastHitTime;
        //float lastObjectHitTime;
        float deltaTime;        
        int buttonSize; 
        int savingButtonWidth;
        std::string title;
        SceneApp* _app;

        bool verbose;

        bool saveIt;
        //unsigned char* pixels;
    };
}

#endif
