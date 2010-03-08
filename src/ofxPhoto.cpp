#include "ofxPhoto.h"

ofxPhoto::ofxPhoto()
{
    //ctor
}

ofxPhoto::~ofxPhoto()
{
    //dtor
}

void ofxPhoto::init () {
    printf("Camera init.  Takes about 10 seconds.\n");
	cameracontext = gp_context_new();

	gp_log_add_func(GP_LOG_ERROR, NULL, NULL);
	gp_camera_new(&camera);
	/* When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
	 * init function seems to traverse the entire filesystem on the camera.  This
	 * is partly why it takes so long.
	 * (Marcus: the ptp2 driver does this by default currently.)
	 */

	retval = gp_camera_init(camera, cameracontext);
	if (retval != GP_OK) {
		printf("  Retval: %d\n", retval);
	}
	else {
        printf("Initializing finished!\n");
	}
}

void ofxPhoto::exit () {
    printf("Camera closing...\n");
    gp_camera_exit(camera, cameracontext);
    printf("Finished!\n");
}

void ofxPhoto::capture () {
	capture_to_file(camera, cameracontext, (char*)"foo.jpg");
}

void ofxPhoto::capture_to_file(Camera *camera, GPContext *cameracontext, char *filename) {
	int fd;
	CameraFile *camerafile;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

	/* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, cameracontext);
	printf("  Retval: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	fd = open(filename, O_CREAT | O_WRONLY, 0644);
	retval = gp_file_new_from_fd(&camerafile, fd);
	printf("  Retval: %d\n", retval);
	retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,
		     GP_FILE_TYPE_NORMAL, camerafile, cameracontext);
	printf("  Retval: %d\n", retval);

	printf("Deleting.\n");
	retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,
			cameracontext);
	printf("  Retval: %d\n", retval);

	gp_file_free(camerafile);
	printf("Capturing finished!");
}
