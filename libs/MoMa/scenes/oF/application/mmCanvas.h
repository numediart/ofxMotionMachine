/** Canvas : Basic UI canvas specific to the MoMa.
*    Canvas heritates from ofxUISuperCanvas.
*    Specific functionnalities :
*        - automatically handle MoMa views management
*        - handle canvas positions (automatically and manually)
*        - automatically manage children canvas
*        - adapted parameters relative to the MoMa.
*    Constructor :
*           MoMa::SceneApp *app : pointer to the current scene application, used to interact with it
*           std::string title = "" : title of the ofxUISuperCanvas
*       Optional options (positioning and parent/children hierarchy) :
*           Position position = DEFAULT : position of the canvas, relative to another canvas (the relative), or to the main ofWindow (if relative = NULL)
*           Position alignment = DEFAULT : alignment of the canvas with the relative (defines which edges of the rectangles are aligned)
*           Canvas *relative = NULL : relative canvas (for position)
*           Canvas *parent = NULL : parent canvas (the parent can open/close its children canvas)
*           int group = 1 : (begins at 1) a parent may have several groups of children. group defines in which group add the child 
*           bool minified = false : minify or not the canvas at its construction
*
* Created by Mickaël Tits on 03/07/2015
* @file mmCanvas.h
* @brief MotionMachine header file for Canvas class.
* @copyright Numediart Institute, UMONS (c) 2014-2015
*
*/

#ifndef __mmCanvas__
#define __mmCanvas__

/*namespace MoMa { 

    class Canvas;
}*/

#include "mmSceneApp.h"
#include "ofMain.h"
#include "ofxUI.h"

namespace MoMa {
    
    enum Position {
        
        DEFAULT,
        LEFT,
        RIGHT,
        ABOVE,
        BELOW,
        LEFTSIDE,
        RIGHTSIDE,
        TOP,
        BOTTOM
    };
    
    //class SceneApp;
    
    class Canvas : public ofxUISuperCanvas {

    public:  

        Canvas( SceneApp *app, std::string title, Position position = DEFAULT, Position alignment = DEFAULT, Canvas *relative = NULL, Canvas *parent = NULL, int group = 1,  bool minified = false );  
        Canvas( SceneApp *app, std::string title, Canvas *parent, int group = 1,  bool minified = false );

        /** setPos : set position of the canvas relative to another */
        void setPos(Position position, Position alignment, Canvas* relative = NULL);
        void remove();

        /** resetPositions (static) : reset positions of the main canvas */
        static void resetPositions();

        /** deleteCanvas (static) : delete all canvas */
        static void deleteCanvas();

        static void openMainCanvas();
        static bool canvasOpened();
        static void closeMainCanvas();
        static void closeAllCanvas();


    protected : 

        /** virtual methods of ofxUISuperCanvas, overriden for MoMa views management (3D view, 2D view and Canvas view) */
        virtual void onMousePressed(ofMouseEventArgs &mouse);
        virtual void onMouseReleased(ofMouseEventArgs &mouse);

        /** virtual methods of ofxUISuperCanvas */
        virtual void update();
        virtual void windowResized(int w, int h);

        /** guiEvent : virtual method called by the event newGUIEvent (see mmCanvas) */
        virtual void guiEvent( ofxUIEventArgs &e ); 
        virtual void canvasEvent( ofxUIEventArgs &e ); 

        /** Canvas parameters (position and alignment relative to the parent, index in the vector of Canvass, and is the canvas minified at initialization) */
        Canvas *_relative, *_parent;
        Position _position, _alignment;
        int _index, _group, _allIndex;
        bool _minified;

        /** setupCanvas : setup the canvas */
        void setupCanvas();

        /** initCanvas : initialize the canvas (size, position and minified) */
        virtual void initCanvas();

        /** methods to open and close canvas */
        void openChildren(int group = 0);
        void openChild(int index, int group = 0);
        void closeChildren( void );
        void closeChildren(int groupe);
        bool childrenOpened(int group = 0);    

        /** close all canvas exept main canvas */
        static void mainView();

    private :

        /** _app : pointer to the application, needed to interact with it */
        SceneApp *_app;      

        /** Static vector storing addresses of main canvas */
        static std::vector<Canvas*> mainCanvas;   

        /** Static vector storing addresses of all canvas */
        static std::vector<Canvas*> allCanvas;  

        /** vector of vector storing addresses of children canvas (one vector for each group) */
        std::vector< std::vector<Canvas*> > childrenCanvas;
        
        bool _isInitialized, _isCanvasHit, _isShortCutDisabled;
        static int _limit;
        int savedMode;
    };
}

#endif
