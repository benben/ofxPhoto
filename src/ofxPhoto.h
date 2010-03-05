#ifndef OFXPHOTO_H
#define OFXPHOTO_H

#include "gphoto2.h"

#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <gphoto2/gphoto2.h>

#include "samples.h"
//#include "config.c"

extern int sample_autodetect (CameraList *list, GPContext *context);
extern int sample_open_camera (Camera ** camera, const char *model, const char *port);


//extern int get_config_value_string (Camera *, const char *, char **, GPContext *);
//extern int set_config_value_string (Camera *, const char *, const char *, GPContext *);

class ofxPhoto
{
    public:
        ofxPhoto();
        virtual ~ofxPhoto();
        void capture();

    protected:
    private:


    void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn);
    static void errordumper(GPLogLevel level, const char *domain, const char *format,va_list args, void *data);
    int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child);
    int canon_enable_capture (Camera *camera, int onoff, GPContext *context);
};

#endif // OFXPHOTO_H
