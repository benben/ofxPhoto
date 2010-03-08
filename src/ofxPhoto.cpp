#include "ofxPhoto.h"

GPParams gp_params;

Camera * camera;


ofxPhoto::ofxPhoto()
{
    //ctor
}

ofxPhoto::~ofxPhoto()
{
    //dtor
}

void ofxPhoto::capture_generic () {
//int capture_generic (CameraCaptureType type, const char __unused__ *name, int download)

    gp_camera_new(&camera);

    CameraCaptureType type = GP_CAPTURE_IMAGE;
    int download = 1;

    static int  glob_interval = 0;
    static int	glob_bulblength = 0;
    static int  glob_frames = 0;


	CameraFilePath path, last;
	char *pathsep;
	int result, frames = 0;
	time_t next_pic_time, now, expose_end_time;
	int waittime;
	CameraAbilities	a;

	result = gp_camera_get_abilities (camera, &a);
	if (result != GP_OK) {
		printf("Could not get capabilities?\n");
		printf("result: %i\n",result);
	}
	//if (0 && (a.operations & GP_OPERATION_TRIGGER_CAPTURE))
		//return capture_generic_trigger (download);

	memset(&last, 0, sizeof(last));
	next_pic_time = time (NULL) + glob_interval;
	if(glob_interval) {
		if (!(gp_params.flags & FLAGS_QUIET)) {
			if (glob_interval != -1)
				printf("Time-lapse mode enabled (interval: %ds).\n",glob_interval);
            else
				printf("Standing by waiting for SIGUSR1 to capture.\n");
		}
	}

	if(glob_bulblength) {
		if (!(gp_params.flags & FLAGS_QUIET)) {
			printf("Bulb mode enabled (exposure time: %ds).\n",
				glob_bulblength);
		}
	}

	int capture_now = 0;
	//signal(SIGUSR1, sig_handler_capture_now);
	int end_next = 0;
	//signal(SIGUSR2, sig_handler_end_next);

	while(++frames) {
		if (!(gp_params.flags & FLAGS_QUIET) && glob_interval) {
			if(!glob_frames)
				printf ("Capturing frame #%d...\n", frames);
			else
				printf ("Capturing frame #%d/%d...\n", frames, glob_frames);
		}

		fflush(stdout);

		if(!glob_bulblength)
			result =  gp_camera_capture (camera, type, &path, gp_params.context);
		//else
		//	result = set_config_action (&gp_params, "bulb", "1");

		if (result != GP_OK) {
			printf("Could not capture.");
			//return (result);
		}

		expose_end_time = time (NULL) + glob_bulblength;	/*Set end time here, otherwise the time it takes to autofocus cuts into the exposure time */

		if(glob_bulblength) {
			CameraEventType type;
			void *data = NULL;
			waittime = expose_end_time - time(NULL);
			while(waittime > 0) {
				sleep(waittime);
				waittime = expose_end_time - time(NULL);
			}
			result = set_config_action (&gp_params, "bulb", "0");
			if (result != GP_OK) {
				printf("Could not end capture (bulb mode).");
				//return (result);
			}
			result = gp_camera_wait_for_event(gp_params.camera, 5000, &type, &data, gp_params.context);
			if ((result != GP_OK) || (type != GP_EVENT_FILE_ADDED)) {
				printf("Could not get filename (bulb mode).");
				//return (result);
			}
			path = *(CameraFilePath *)data;
		}

		/* If my Canon EOS 10D is set to auto-focus and it is unable to
		 * get focus lock - it will return with *UNKNOWN* as the filename.
		 */
		if (glob_interval && strcmp(path.name, "*UNKNOWN*") == 0) {
			if (!(gp_params.flags & FLAGS_QUIET)) {
				printf("Capture failed (auto-focus problem?)...\n");
				sleep(1);
				continue;
			}
		}

		if (strcmp(path.folder, "/") == 0)
			pathsep = "";
		else
			pathsep = "/";

		if (gp_params.flags & FLAGS_QUIET) {
			if (!(gp_params.flags & (FLAGS_STDOUT|FLAGS_STDOUT_SIZE)))
				printf ("%s%s%s\n", path.folder, pathsep, path.name);
		} else {
			printf("New file is in location %s%s%s on the camera\n",
				path.folder, pathsep, path.name);
		}

		if (download) {
			if (strcmp(path.folder, last.folder)) {
				memcpy(&last, &path, sizeof(last));

				result = set_folder_action(&gp_params, path.folder);
				if (result != GP_OK) {
					printf("Could not set folder.");
					//return (result);
				}
			}

			result = get_file_common (path.name, GP_FILE_TYPE_NORMAL);
			if (result != GP_OK) {
				printf("Could not get image.");
				if(result == GP_ERROR_FILE_NOT_FOUND) {
					/* Buggy libcanon.so?
					 * Can happen if this was the first capture after a
					 * CF card format, or during a directory roll-over,
					 * ie: CANON100 -> CANON101
					 */
					printf("Buggy libcanon.so?");
				}
				//return (result);
			}

			if (!(gp_params.flags & FLAGS_QUIET))
				printf("Deleting file %s%s%s on the camera\n",
					path.folder, pathsep, path.name);

			result = delete_file_action (&gp_params, path.name);
			if (result != GP_OK) {
				printf("Could not delete image.");
				//return (result);
			}
		}
		/* Break if we've reached the requested number of frames
		 * to capture.
		 */
		if(!glob_interval) break;

		if(glob_frames && frames == glob_frames) break;

		/* Break if we've been told to end before the next frame */
		if(end_next) break;

#if 0
		/* Marcus Meissner: Before you enable this, try to fix the
		 * camera driver first! camera_exit is NOT necessary for
		 * 2 captures in a row!
		 */

		/* Without this, it seems that the second capture always fails.
		 * That is probably obvious...  for me it was trial n' error.
		 */
		result = gp_camera_exit (gp_params.camera, gp_params.context);
		if (result != GP_OK) {
			printf("Could not close camera connection.");
		}
#endif
		/*
		 * Even if the interval is set to -1, it is better to take a
		 * picture first to prepare the camera driver for faster
		 * response when a signal is caught.
		 * [alesan]
		 */
		if (glob_interval != -1) {
			waittime = next_pic_time - time (NULL);
			if (waittime > 0) {
				int i;
				if (!(gp_params.flags & FLAGS_QUIET) && glob_interval)
					printf("Sleeping for %d second(s)...\n", waittime);
				/* We're not sure about sleep() semantics when catching a signal */
				for (i=0; (!capture_now) && (i<waittime); i++)
					sleep(1);
				if (capture_now && !(gp_params.flags & FLAGS_QUIET) && glob_interval)
					printf("Awakened by SIGUSR1...\n");
			} else {
				if (!(gp_params.flags & FLAGS_QUIET) && glob_interval)
					printf("not sleeping (%d seconds behind schedule)\n", - waittime);
			}
			if (capture_now && (gp_params.flags & FLAGS_RESET_CAPTURE_INTERVAL)) {
				next_pic_time = time(NULL) + glob_interval;
			}
			else if (!capture_now) {
				now = time(NULL) - glob_interval;
				/*
				 * In the case of a (huge) time-sync while gphoto is running,
				 * gphoto could percieve an extremely large amount of time and
				 * stay "behind schedule" quite forever. That's why I reduce the
				 * difference of time with the following loop.
				 * [alesan]
				 */
				do {
					next_pic_time += glob_interval;
				} while (next_pic_time < now);
			}
			capture_now = 0;
		} else {
			/* wait indefinitely for SIGUSR1 */
			do {
				pause();
			} while(!capture_now);
			capture_now = 0;
			if (!(gp_params.flags & FLAGS_QUIET))
				printf("Awakened by SIGUSR1...\n");
		}
	}

	//signal(SIGUSR1, SIG_DFL);
	//return (GP_OK);
}

int ofxPhoto::delete_file_action (GPParams *p, const char *filename)
{
	if (p->flags & FLAGS_NEW) {
		CameraFileInfo info;

		CR (gp_camera_file_get_info (p->camera, p->folder, filename,
					     &info, p->context));
		if (info.file.fields & GP_FILE_INFO_STATUS &&
		    info.file.status == GP_FILE_STATUS_DOWNLOADED)
			return (GP_OK);
	}
	return (gp_camera_file_delete (p->camera, p->folder, filename,
				       p->context));
}


int ofxPhoto::get_file_common (const char *arg, CameraFileType type )
{
        gp_log (GP_LOG_DEBUG, "main", "Getting '%s'...", arg);

	gp_params.download_type = type; /* remember for multi download */
	/*
	 * If the user specified the file directly (and not a number),
	 * get that file.
	 */
        if (strchr (arg, '.'))
                return (save_file_to_file (gp_params.camera, gp_params.context, gp_params.flags,
					   gp_params.folder, arg, type));

        /*switch (type) {
        case GP_FILE_TYPE_PREVIEW:
		//CR (for_each_file_in_range (&gp_params, save_thumbnail_action, arg));
		break;
        case GP_FILE_TYPE_NORMAL:
                CR (for_each_file_in_range (&gp_params, save_file_action, arg));
		break;
        case GP_FILE_TYPE_RAW:
               // CR (for_each_file_in_range (&gp_params, save_raw_action, arg));
		break;
	case GP_FILE_TYPE_AUDIO:
		CR (for_each_file_in_range (&gp_params, save_audio_action, arg));
		break;
	case GP_FILE_TYPE_EXIF:
		CR (for_each_file_in_range (&gp_params, save_exif_action, arg));
		break;
	case GP_FILE_TYPE_METADATA:
		CR (for_each_file_in_range (&gp_params, save_meta_action, arg));
		break;
        default:
                return (GP_ERROR_NOT_SUPPORTED);
        }

	return (GP_OK);*/
}

int ofxPhoto::save_file_action (GPParams *p, const char *filename)
{
	return (save_file_to_file (p->camera, p->context, p->flags,
				   p->folder, filename, GP_FILE_TYPE_NORMAL));
}


int ofxPhoto::save_file_to_file (Camera *camera, GPContext *context, Flags flags, const char *folder, const char *filename, CameraFileType type)
{
        int fd, res;
        CameraFile *file;
	char	tmpname[20], *tmpfilename;
	struct privstr *ps;

	if (flags & FLAGS_NEW) {
		CameraFileInfo info;

		CR (gp_camera_file_get_info (camera, folder, filename,
					     &info, context));
		switch (type) {
		case GP_FILE_TYPE_PREVIEW:
			if (info.preview.fields & GP_FILE_INFO_STATUS &&
			    info.preview.status == GP_FILE_STATUS_DOWNLOADED)
				return (GP_OK);
			break;
		case GP_FILE_TYPE_NORMAL:
		case GP_FILE_TYPE_RAW:
		case GP_FILE_TYPE_EXIF:
			if (info.file.fields & GP_FILE_INFO_STATUS &&
			    info.file.status == GP_FILE_STATUS_DOWNLOADED)
				return (GP_OK);
			break;
		case GP_FILE_TYPE_AUDIO:
			if (info.audio.fields & GP_FILE_INFO_STATUS &&
			    info.audio.status == GP_FILE_STATUS_DOWNLOADED)
				return (GP_OK);
			break;
		default:
			return (GP_ERROR_NOT_SUPPORTED);
		}
	}
	strcpy (tmpname, "tmpfileXXXXXX");
	fd = mkstemp(tmpname);
	if (fd == -1) {
        	CR (gp_file_new (&file));
		tmpfilename = NULL;
	} else {
		if (time(NULL) & 1) { /* to test both methods. */
			gp_log (GP_LOG_DEBUG, "save_file_to_file","using fd method");
			res = gp_file_new_from_fd (&file, fd);
			if (res < GP_OK) {
				close (fd);
				unlink (tmpname);
				return res;
			}
		} else {
			gp_log (GP_LOG_DEBUG, "save_file_to_file","using handler method");
			//ps = malloc (sizeof(*ps));
			if (!ps) return GP_ERROR_NO_MEMORY;
			//ps->fd = fd;
			/* just pass in the file pointer as private */
			/*res = gp_file_new_from_handler (&file, &xhandler, ps);
			if (res < GP_OK) {
				close (fd);
				unlink (tmpname);
				return res;
			}*/
		}
		tmpfilename = tmpname;
	}
        res = gp_camera_file_get (camera, folder, filename, type,
				  file, context);
	if (res < GP_OK) {
		gp_file_unref (file);
		if (tmpfilename) unlink (tmpfilename);
		return res;
	}

	if (flags & FLAGS_STDOUT) {
                const char *data;
                unsigned long int size;

                CR (gp_file_get_data_and_size (file, &data, &size));

		if (flags & FLAGS_STDOUT_SIZE) /* this will be difficult in fd mode */
                        printf ("%li\n", size);
                if (1!=fwrite (data, size, 1, stdout))
			fprintf(stderr,"fwrite failed writing to stdout.\n");
                gp_file_unref (file);
		unlink (tmpname);
                return (GP_OK);
        }
/*        res = save_camera_file_to_file (folder, filename, type, file, tmpfilename);
        gp_file_unref (file);
	if ((res!=GP_OK) && tmpfilename)
		unlink (tmpfilename);*/
        return (res);
}


int ofxPhoto::set_folder_action (GPParams *p, const char *folder)
{
	if (p->folder)
		free (p->folder);
	p->folder = strdup (folder);
	return (p->folder ? GP_OK: GP_ERROR_NO_MEMORY);
}


int ofxPhoto::set_config_action (GPParams *p, const char *name, const char *value) {
	CameraWidget *rootconfig,*child;
	int	ret, ro;
	CameraWidgetType	type;

	//ret = _find_widget_by_name (p, name, &child, &rootconfig);
	if (ret != GP_OK)
		return ret;

	ret = gp_widget_get_readonly (child, &ro);
	if (ret != GP_OK) {
		gp_widget_free (rootconfig);
		return ret;
	}
	if (ro == 1) {
		gp_context_error (p->context, "Property %s is read only.", name);
		gp_widget_free (rootconfig);
		return GP_ERROR;
	}
	ret = gp_widget_get_type (child, &type);
	if (ret != GP_OK) {
		gp_widget_free (rootconfig);
		return ret;
	}

	switch (type) {
	case GP_WIDGET_TEXT: {		/* char *		*/
		ret = gp_widget_set_value (child, value);
		if (ret != GP_OK)
			gp_context_error (p->context, "Failed to set the value of text widget %s to %s.", name, value);
		break;
	}
	case GP_WIDGET_RANGE: {	/* float		*/
		float	f,t,b,s;

		ret = gp_widget_get_range (child, &b, &t, &s);
		if (ret != GP_OK)
			break;
		if (!sscanf (value, "%f", &f)) {
			gp_context_error (p->context, "The passed value %s is not a floating point value.", value);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
		}
		if ((f < b) || (f > t)) {
			gp_context_error (p->context, "The passed value %f is not within the expected range %f - %f.", f, b, t);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
		}
		ret = gp_widget_set_value (child, &f);
		if (ret != GP_OK)
			gp_context_error (p->context, "Failed to set the value of range widget %s to %f.", name, f);
		break;
	}
	case GP_WIDGET_TOGGLE: {	/* int		*/
		int	t;

		t = 2;
		if (	!strcasecmp (value, "off")	|| !strcasecmp (value, "no")	||
			!strcasecmp (value, "false")	|| !strcmp (value, "0")		||
			!strcasecmp (value, "off")	|| !strcasecmp (value, "no")	||
			!strcasecmp (value, "false")
		)
			t = 0;
		if (	!strcasecmp (value, "on")	|| !strcasecmp (value, "yes")	||
			!strcasecmp (value, "true")	|| !strcmp (value, "1")		||
			!strcasecmp (value, "on")	|| !strcasecmp (value, "yes")	||
			!strcasecmp (value, "true")
		)
			t = 1;
		/*fprintf (stderr," value %s, t %d\n", value, t);*/
		if (t == 2) {
			gp_context_error (p->context, "The passed value %s is not a valid toggle value.", value);
			ret = GP_ERROR_BAD_PARAMETERS;
			break;
		}
		ret = gp_widget_set_value (child, &t);
		if (ret != GP_OK)
			gp_context_error (p->context, "Failed to set values %s of toggle widget %s.", value, name);
		break;
	}
	case GP_WIDGET_DATE:  {		/* int			*/
		int	t = -1;
		struct tm xtm;

#ifdef HAVE_STRPTIME
		if (strptime (value, "%c", &xtm) || strptime (value, "%Ec", &xtm))
			t = mktime (&xtm);
#endif
		if (t == -1) {
			if (!sscanf (value, "%d", &t)) {
				gp_context_error (p->context, "The passed value %s is neither a valid time nor an integer.", value);
				ret = GP_ERROR_BAD_PARAMETERS;
				break;
			}
		}
		ret = gp_widget_set_value (child, &t);
		if (ret != GP_OK)
			gp_context_error (p->context, "Failed to set new time of date/time widget %s to %s.", name, value);
		break;
	}
	case GP_WIDGET_MENU:
	case GP_WIDGET_RADIO: { /* char *		*/
		int cnt, i;
		char *endptr;

		cnt = gp_widget_count_choices (child);
		if (cnt < GP_OK) {
			ret = cnt;
			break;
		}
		ret = GP_ERROR_BAD_PARAMETERS;
		for ( i=0; i<cnt; i++) {
			const char *choice;

			ret = gp_widget_get_choice (child, i, &choice);
			if (ret != GP_OK)
				continue;
			if (!strcmp (choice, value)) {
				ret = gp_widget_set_value (child, value);
				break;
			}
		}
		if (i != cnt)
			break;

		/* make sure we parse just 1 integer, and there is nothing more.
		 * sscanf just does not provide this, we need strtol.
		 */
		i = strtol (value, &endptr, 10);
		if ((value != endptr) && (*endptr == '\0')) {
			if ((i>= 0) && (i < cnt)) {
				const char *choice;

				ret = gp_widget_get_choice (child, i, &choice);
				if (ret == GP_OK)
					ret = gp_widget_set_value (child, choice);
				break;
			}
		}
		/* Lets just try setting the value directly, in case we have flexible setters,
		 * like PTP shutterspeed. */
		ret = gp_widget_set_value (child, value);
		if (ret == GP_OK)
			break;
		gp_context_error (p->context, "Choice %s not found within list of choices.", value);
		break;
	}

	/* ignore: */
	case GP_WIDGET_WINDOW:
	case GP_WIDGET_SECTION:
	case GP_WIDGET_BUTTON:
		gp_context_error (p->context, "The %s widget is not configurable.", name);
		ret = GP_ERROR_BAD_PARAMETERS;
		break;
	}
	if (ret == GP_OK) {
		ret = gp_camera_set_config (p->camera, rootconfig, p->context);
		if (ret != GP_OK)
			gp_context_error (p->context, "Failed to set new configuration value %s for configuration entry %s.", value, name);
	}
	gp_widget_free (rootconfig);
	return (ret);
}

void ofxPhoto::init () {
    printf("Camera init.  Takes about 10 seconds.\n");
	canoncontext = gp_context_new();

	//gp_log_add_func(GP_LOG_ERROR, errordumper, NULL);
	gp_camera_new(&canon);
	/* When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
	 * init function seems to traverse the entire filesystem on the camera.  This
	 * is partly why it takes so long.
	 * (Marcus: the ptp2 driver does this by default currently.)
	 */

	retval = gp_camera_init(canon, canoncontext);
	if (retval != GP_OK) {
		printf("  Retval: %d\n", retval);
		//exit (1);
	}
	else {
        printf("Initializing finished!\n");
	}
}

void ofxPhoto::exit () {
    printf("Camera closing...\n");
    gp_camera_exit(canon, canoncontext);
    printf("Finished!\n");
}

void ofxPhoto::capture () {
	//canon_enable_capture(canon, TRUE, canoncontext);
	/*set_capturetarget(canon, canoncontext);*/
	capture_to_file(canon, canoncontext, (char*)"foo.jpg");
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
