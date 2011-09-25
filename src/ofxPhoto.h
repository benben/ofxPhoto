/*
 * Copyright (c) 2010, Benjamin Knofe
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the Lesser GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the Lesser GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef OFXPHOTO_H
#define OFXPHOTO_H

#include "gphoto2.h"
#include "FreeImage.h"
#include "ofMain.h"
#include "ofThread.h"

class ofxPhoto : public ofThread
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
