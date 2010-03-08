#ifndef OFXPHOTO_H
#define OFXPHOTO_H

#include "gphoto2.h"
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

class ofxPhoto
{
    public:
        ofxPhoto();
        virtual ~ofxPhoto();

        void init();
        void exit();
        void capture();
        void capture_generic();

    protected:
    private:

    Camera	*camera;
	int	retval;
	GPContext *cameracontext;

    void capture_to_file(Camera *camera, GPContext *cameracontext, char *filename);


};

#endif // OFXPHOTO_H
