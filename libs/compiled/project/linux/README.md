##Introduction

On Linux, Motion Machine (MoMa) is composed of two parts, as opposed to what is done in OSX and Windows where xcode/VS configs take care of everything as one big project. The two parts are:

1. libmoma, a shared library based on the files in `./libs/MoMa/{core,features,parsers}`. You can use it to load, access and process mocap data in your applications independently of openFrameworks.
2. ofxMoMa, an openFrameworks (oFx) "adapter" to make it easy to use MoMa into oFx.

Additionaly, examples of oFx applications are available into `./apps/` and they can be used as starting points to your own applications.

In the following, we assume that you downloaded openFrameworks and unzipped it into a folder `$(OF_ROOT)` (`/opt/openframeworks` is the default set into the example of MoMa, adapt accordingly) and that MoMa is installed into a folder `$(MOMA_ROOT)`

##Compilation of libmoma

libmoma depends only on armadillo (4.650 or later, so you might need to install it manually). To build and install it properly, do the following:

```
$ cd $(MOMA_ROOT)/libs/compiled/project/linux
$ mkdir build
$ cd build
$ cmake ..
$ make -j4
$ sudo make install
$ sudo ldconfig
```

(you can also not use a `build` subfolder and do `cmake .` directly into the folder where CMakeLists.txt is. Using a build folder makes it easier to clean cmake stuffs)

`libmoma.so` (we don't use version numbers in file naming yet) should be installed into `/usr/local/lib` and all header files should go into a new folder `/usr/local/include/moma/`

##ofxMoMa addon

```
$ cd $(OF_ROOT)/addons
$ mkdir -p ofxMoMa/src
$ cd ofxMoMa
$ cp $(MOMA_ROOT)/libs/MoMa/scenes/oF/*/* src/
$ cp $(MOMA_ROOT)/libs/compiled/project/linux/ofxMoMa/addon_config.mk .
```

That's it.

##Other plugins

Apart from ofxMoMa, the basic empty example in MoMa depends on three other addons, ofxUI, ofxOsc and ofxXmlSettings. Depending on your version of openFrameworks (only version 0.8.4 has been tested so far for Linux), these may or may not work properly (\*cough\* ofxUI \*cough\*). In order to avoid any inconvenience, we suggest you use the fixed versions we have in `$(MOMA_ROOT)/libs/oF/addons/`. To do so, simply copy them to overwrite the ones in `$(OF_ROOT)/addons/`

##Compiling and running the empty example

The example supposes that openFrameworks is installed into `/opt/openframeworks`, edit and adapt the file config.make as needed if that's not the case.

```
$ cd $(MOMA_ROOT)/apps/example/mmEmptyExample
$ make
$ make run
```

If your project has dependencies, you need to edit its config.make and adapt `PROJECT_LDFLAGS` and `PROJECT_CFLAGS`. For instance if you have a dependency to armadillo, you need to add an `-laramdillo` flag to the linker parameters:

```
PROJECT_LDFLAGS = -lmoma -larmadillo
```

Same thing if the header files are not in the default path, you need to edit the compiler flags:

```
PROJECT_CFLAGS = -I/usr/local/include/moma -I/usr/local/include/myawesomelib
```
