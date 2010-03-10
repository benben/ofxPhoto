#ifndef OFXPHOTO_H
#define OFXPHOTO_H

//#include <stdio.h>
//#include <string.h>
//#include <fcntl.h>

#include "gphoto2.h"
#include "FreeImage.h"
#include "ofMain.h"
#include "ofxThread.h"

class ofxPhoto : public ofxThread
{
    public:
        ofxPhoto();
        virtual ~ofxPhoto();

        void init();
        void exit();
        unsigned char * capture();
        bool captureSucceeded();
        int getCaptureWidth();
        int getCaptureHeight();
        void threadedFunction();

        void startCapture();

        bool isBusy();

    protected:
    private:
        Camera	*camera;
        int	retval;
        GPContext *cameracontext;
        CameraFile *camerafile;
        CameraFilePath camera_file_path;

        const char *ptr;
        unsigned long int size;

        FIMEMORY *hmem;
        FREE_IMAGE_FORMAT fif;
        FIBITMAP *bmp;

        int width, height, bpp;

        ofPixels pix;

        bool bCameraInit;
        bool bCaptureSucceeded;
        bool bCamIsBusy;

        int captureWidth, captureHeight;

        bool capture_to_of(Camera *camera, GPContext *cameracontext);

        //COPIED FROM ofImage.cpp
        void allocatePixels(ofPixels &pix, int width, int height, int bpp);
        void putBmpIntoPixels(FIBITMAP * bmp, ofPixels &pix);

        //NOT IMPLEMENTED YET
        void capture_to_file(Camera *camera, GPContext *cameracontext, char *filename);
};

#endif // OFXPHOTO_H
