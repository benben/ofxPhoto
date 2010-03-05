#include "ofxPhoto.h"

ofxPhoto::ofxPhoto()
{
    //ctor
}

ofxPhoto::~ofxPhoto()
{
    //dtor
}

void ofxPhoto::capture () {
	Camera	*canon;
	int	retval;
	GPContext *canoncontext = gp_context_new();

	//gp_log_add_func(GP_LOG_ERROR, errordumper, NULL);
	gp_camera_new(&canon);

	/* When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
	 * init function seems to traverse the entire filesystem on the camera.  This
	 * is partly why it takes so long.
	 * (Marcus: the ptp2 driver does this by default currently.)
	 */
	printf("Camera init.  Takes about 10 seconds.\n");
	retval = gp_camera_init(canon, canoncontext);
	if (retval != GP_OK) {
		printf("  Retval: %d\n", retval);
		//exit (1);
	}
	canon_enable_capture(canon, TRUE, canoncontext);
	/*set_capturetarget(canon, canoncontext);*/
	capture_to_file(canon, canoncontext, "foo.jpg");
	gp_camera_exit(canon, canoncontext);
}

void ofxPhoto::capture_to_file(Camera *canon, GPContext *canoncontext, char *fn) {
	int fd, retval;
	CameraFile *canonfile;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

	/* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(canon, GP_CAPTURE_IMAGE, &camera_file_path, canoncontext);
	printf("  Retval: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

	fd = open(fn, O_CREAT | O_WRONLY, 0644);
	retval = gp_file_new_from_fd(&canonfile, fd);
	printf("  Retval: %d\n", retval);
	retval = gp_camera_file_get(canon, camera_file_path.folder, camera_file_path.name,
		     GP_FILE_TYPE_NORMAL, canonfile, canoncontext);
	printf("  Retval: %d\n", retval);

	printf("Deleting.\n");
	retval = gp_camera_file_delete(canon, camera_file_path.folder, camera_file_path.name,
			canoncontext);
	printf("  Retval: %d\n", retval);

	gp_file_free(canonfile);
}

int ofxPhoto::_lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child) {
	int ret;
	ret = gp_widget_get_child_by_name (widget, key, child);
	if (ret < GP_OK)
		ret = gp_widget_get_child_by_label (widget, key, child);
	return ret;
}

int ofxPhoto::canon_enable_capture (Camera *camera, int onoff, GPContext *context) {
	CameraWidget		*widget = NULL, *child = NULL;
	CameraWidgetType	type;
	int			ret;

	ret = gp_camera_get_config (camera, &widget, context);
	if (ret < GP_OK) {
		fprintf (stderr, "camera_get_config failed: %d\n", ret);
		return ret;
	}
	ret = _lookup_widget (widget, "capture", &child);
	if (ret < GP_OK) {
		/*fprintf (stderr, "lookup widget failed: %d\n", ret);*/
		goto out;
	}

	ret = gp_widget_get_type (child, &type);
	if (ret < GP_OK) {
		fprintf (stderr, "widget get type failed: %d\n", ret);
		goto out;
	}
	switch (type) {
        case GP_WIDGET_TOGGLE:
		break;
	default:
		fprintf (stderr, "widget has bad type %d\n", type);
		ret = GP_ERROR_BAD_PARAMETERS;
		goto out;
	}
	/* Now set the toggle to the wanted value */
	ret = gp_widget_set_value (child, &onoff);
	if (ret < GP_OK) {
		fprintf (stderr, "toggling Canon capture to %d failed with %d\n", onoff, ret);
		goto out;
	}
	/* OK */
	ret = gp_camera_set_config (camera, widget, context);
	if (ret < GP_OK) {
		fprintf (stderr, "camera_set_config failed: %d\n", ret);
		return ret;
	}
out:
	gp_widget_free (widget);
	return ret;
}
