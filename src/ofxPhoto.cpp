#include "ofxPhoto.h"

ofxPhoto::ofxPhoto() {
    //ctor
}

ofxPhoto::~ofxPhoto() {
    //dtor
}

void ofxPhoto::init () {

    bCameraInit = false;
    bCaptureSucceeded = false;
    bCamIsBusy = false;

    captureWidth = 0;
    captureHeight = 0;

    printf("Camera init.  Takes about 10 seconds.\n");
	cameracontext = gp_context_new();

	//gp_log_add_func(GP_LOG_ERROR, NULL, NULL);
	gp_camera_new(&camera);
	/* When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
	 * init function seems to traverse the entire filesystem on the camera.  This
	 * is partly why it takes so long.
	 * (Marcus: the ptp2 driver does this by default currently.)
	 */

	retval = gp_camera_init(camera, cameracontext);
	if (retval != GP_OK) {
        printf("Retval: %d %s\n", retval, gp_result_as_string(retval));
	}
	else {
        printf("Initializing finished!\n");
        bCameraInit = true;
	}
}

void ofxPhoto::exit() {
    printf("Camera closing...\n");
    gp_camera_exit(camera, cameracontext);
    printf("Finished!\n");
}

void ofxPhoto::threadedFunction(){
    while( isThreadRunning() != 0 ){
        if( lock() ){
            capture_to_of(camera, cameracontext);
            unlock();
            stopThread();
            //printf("Thread stopped!\n");
        }
    }
}

void ofxPhoto::startCapture(){
    bCamIsBusy = true;
    startThread(true, false);
    //printf("Thread started!\n");
}

unsigned char * ofxPhoto::capture() {
        //reset the bool
        bCaptureSucceeded = false;
        return pix.getPixels();
}

int ofxPhoto::getCaptureWidth(){
    return captureWidth;
}

int ofxPhoto::getCaptureHeight(){
    return captureHeight;
}

bool ofxPhoto::isBusy(){
    return bCamIsBusy;
}

bool ofxPhoto::captureSucceeded(){
    return bCaptureSucceeded;
}

bool ofxPhoto::capture_to_of(Camera *camera, GPContext *cameracontext) {

    if(bCameraInit) {
        printf("Capturing (this may take some time) ...\n");

        //force camera to take a picture
        retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, cameracontext);
        if(retval == GP_OK) {
            //printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

            //create new camerafile
            gp_file_new(&camerafile);

            //download picture from camera to camerafile
            retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,GP_FILE_TYPE_NORMAL, camerafile, cameracontext);
            if(retval == GP_OK) {
                //get data and size of the picture
                gp_file_get_data_and_size(camerafile, &ptr, &size);

                //create a FreeImage Object from Picture in Memory
                hmem = FreeImage_OpenMemory((uint8_t *)ptr,size);
                fif = FreeImage_GetFileTypeFromMemory(hmem, 0);
                bmp = FreeImage_LoadFromMemory(fif, hmem, 0);

                putBmpIntoPixels(bmp, pix);

                printf("Deleting picture on camera...\n");
                retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,cameracontext);
                if(retval != GP_OK) {
                    printf("Cannot delete picture on camera.\n");
                    printf("Retval: %d %s\n", retval, gp_result_as_string(retval));
                }

                gp_file_free(camerafile);
                printf("Capturing finished!\n");

                //Camera is not busy anymore
                bCamIsBusy = false;

                bCaptureSucceeded = true;
                return bCaptureSucceeded;
            }
            else {
                printf("Retval: %d %s\n", retval, gp_result_as_string(retval));

                //Camera is not busy anymore
                bCamIsBusy = false;

                return bCaptureSucceeded;
            }
        }
        else {
            printf("Retval: %d %s\n", retval, gp_result_as_string(retval));

            //Camera is not busy anymore
            bCamIsBusy = false;

            return bCaptureSucceeded;
        }
    }
    else {
        printf("Camera is not initiated!\n");

        //Camera is not busy anymore
        bCamIsBusy = false;

        return bCaptureSucceeded;
    }
}

void ofxPhoto::putBmpIntoPixels(FIBITMAP * bmp, ofPixels &pix){
	captureWidth			= FreeImage_GetWidth(bmp);
	captureHeight			= FreeImage_GetHeight(bmp);
	int bpp				= FreeImage_GetBPP(bmp);
	int bytesPerPixel	= bpp / 8;
	//------------------------------------------
	// call the allocation routine (which checks if really need to allocate) here:
	//allocatePixels(pix, captureWidth, captureHeight, bpp);
    pix.allocate(captureWidth, captureHeight,3);
	FreeImage_ConvertToRawBits(pix.getPixels(), bmp, captureWidth*bytesPerPixel, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);  // get bits
}

void ofxPhoto::allocatePixels(ofPixels &pix, int width, int height, int bpp){
//
//	bool bNeedToAllocate = false;
//	if (pix.isAllocated() == true){
//		if ( (pix.getWidth() == width) && (pix.getHeight() == height) && (pix.getBitsPerPixel() == bpp)){
//			//ofLog(OF_LOG_NOTICE,"we are good, no reallocation needed");
//			bNeedToAllocate = false;
//		 } else {
//			delete[] pix.getPixels();
//			bNeedToAllocate = true;
//		 }
//	} else {
//		bNeedToAllocate = true;
//	}
//
//	int byteCount = bpp / 8;
//
//	if (bNeedToAllocate == true){
//        pix.allocate(<#int w#>, <#int h#>, <#int channels#>)
//		pix.width			= width;
//		pix.height			= height;
//		pix.bitsPerPixel	= bpp;
//		pix.bytesPerPixel	= bpp / 8;
//		switch (pix.getBitsPerPixel()){
//			case 8:
//				pix.glDataType		= GL_LUMINANCE;
//				pix.ofImageType		= OF_IMAGE_GRAYSCALE;
//				break;
//			case 24:
//				pix.glDataType		= GL_RGB;
//				pix.ofImageType		= OF_IMAGE_COLOR;
//				break;
//			case 32:
//				pix.glDataType		= GL_RGBA;
//				pix.ofImageType		= OF_IMAGE_COLOR_ALPHA;
//				break;
//		}
//
//        pix.alloc
//		pix.pixels			= new unsigned char[pix.width*pix.height*byteCount];
//		pix.bAllocated		= true;
//	}
}

void ofxPhoto::capture_to_file(Camera *camera, GPContext *cameracontext, char *filename) {
	/* NOT IMPLEMENTED YET
	int fd;

	printf("Capturing.\n");

	// NOP: This gets overridden in the library to /capt0000.jpg
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
	printf("Capturing finished!\n"); */
}
