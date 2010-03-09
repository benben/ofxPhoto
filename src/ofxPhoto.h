#ifndef OFXPHOTO_H
#define OFXPHOTO_H

#include <stdio.h>
#include <string.h>
#include <fcntl.h>

#include "gphoto2.h"
#include "FreeImage.h"
#include "ofMain.h"
#include "ofImage.h"

class ofxPhoto
{
    public:
        ofxPhoto();
        virtual ~ofxPhoto();

        void init();
        void exit();
        unsigned char * capture();
        void capture_generic();

        ofPixels pix;

    protected:
    private:

    Camera	*camera;
	int	retval;
	GPContext *cameracontext;
    CameraFile *camerafile;
	CameraFilePath camera_file_path;

    const char *ptr;
	unsigned long int size;

    void swapRgb(ofPixels &pix);

    unsigned char * capture_to_of(Camera *camera, GPContext *cameracontext);

    void capture_to_file(Camera *camera, GPContext *cameracontext, char *filename);
    void allocate(ofPixels &pix, int width, int height, int bpp);

    FIMEMORY *hmem;
    FREE_IMAGE_FORMAT fif;
    FIBITMAP *bmp;

    int width, height, bpp;
};

#endif // OFXPHOTO_H
