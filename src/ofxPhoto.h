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

typedef enum {
	FLAGS_RECURSE		= 1 << 0,
	FLAGS_REVERSE		= 1 << 1,
	FLAGS_QUIET		= 1 << 2,
	FLAGS_FORCE_OVERWRITE	= 1 << 3,
	FLAGS_STDOUT		= 1 << 4,
	FLAGS_STDOUT_SIZE	= 1 << 5,
	FLAGS_NEW		= 1 << 6,
	FLAGS_RESET_CAPTURE_INTERVAL = 1 << 7
} Flags;

typedef enum {
	MULTI_UPLOAD,
	MULTI_UPLOAD_META,
	MULTI_DOWNLOAD,
	MULTI_DELETE
} MultiType;

typedef struct _GPParams GPParams;
struct _GPParams {
	Camera *camera;
	GPContext *context;
	char *folder;
	char *filename;

	unsigned int cols;

	Flags flags;

	/** This field is supposed to be private. Usually, you use the
	 * gp_camera_abilities_list() function to access it.
	 */
	CameraAbilitiesList *_abilities_list;

	GPPortInfoList *portinfo_list;
	int debug_func_id;

	MultiType	multi_type;
	CameraFileType	download_type; /* for multi download */

	char *hook_script; /* If non-NULL, hook script to run */
	char **envp;  /* envp from the main() function */
};

#define CR(result)       {int r=(result); if (r<0) return r;}



extern int sample_autodetect (CameraList *list, GPContext *context);
extern int sample_open_camera (Camera ** camera, const char *model, const char *port);


//extern int get_config_value_string (Camera *, const char *, char **, GPContext *);
//extern int set_config_value_string (Camera *, const char *, const char *, GPContext *);

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


    int set_config_action (GPParams *p, const char *name, const char *value);
    int set_folder_action (GPParams *p, const char *folder);
    int	get_file_common (const char *arg, CameraFileType type );
    int delete_file_action (GPParams *p, const char *filename);
    int save_file_to_file (Camera *camera, GPContext *context, Flags flags, const char *folder, const char *filename, CameraFileType type);
    int save_file_action (GPParams *p, const char *filename);

    void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn);
    static void errordumper(GPLogLevel level, const char *domain, const char *format,va_list args, void *data);
    int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child);
    int canon_enable_capture (Camera *camera, int onoff, GPContext *context);

    Camera	*canon;
	int	retval;
	GPContext *canoncontext;
};

#endif // OFXPHOTO_H
