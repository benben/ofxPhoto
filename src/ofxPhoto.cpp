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

unsigned char * ofxPhoto::capture () {
	//capture_to_file(camera, cameracontext, (char*)"foo.jpg");
	return capture_to_of(camera, cameracontext);
	//return pix.pixels;
}

unsigned char * ofxPhoto::capture_to_of(Camera *camera, GPContext *cameracontext) {

	CameraFile *camerafile;
	CameraFilePath camera_file_path;

	printf("Capturing.\n");

    /* NOP: This gets overridden in the library to /capt0000.jpg */
	strcpy(camera_file_path.folder, "/");
	strcpy(camera_file_path.name, "foo.jpg");

	retval = gp_camera_capture(camera, GP_CAPTURE_IMAGE, &camera_file_path, cameracontext);
	printf("  Retval: %d\n", retval);

	printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

    gp_file_new(&camerafile);

    retval = gp_camera_file_get(camera, camera_file_path.folder, camera_file_path.name,GP_FILE_TYPE_NORMAL, camerafile, cameracontext);
	printf("  Retval: %d\n", retval);

	const char *ptr;
	unsigned long int size;

	gp_file_get_data_and_size(camerafile, &ptr, &size);

	printf("size: %d \n", size);

	ofPixels pix;

    FIMEMORY *hmem = FreeImage_OpenMemory((uint8_t *)ptr,size);

    FREE_IMAGE_FORMAT fif = FreeImage_GetFileTypeFromMemory(hmem, 0);

    FIBITMAP *bmp = FreeImage_LoadFromMemory(fif, hmem, 0);

    //###############COPIED FROM ofImage::loadImageIntoPixels

    int width, height, bpp;

    width 		= FreeImage_GetWidth(bmp);
    height 		= FreeImage_GetHeight(bmp);
	bpp 		= FreeImage_GetBPP(bmp);

    bool bPallette = (FreeImage_GetColorType(bmp) == FIC_PALETTE);

		switch (bpp){
			case 8:
				if (bPallette) {
					FIBITMAP 	* bmpTemp =		FreeImage_ConvertTo24Bits(bmp);
					if (bmp != NULL)			FreeImage_Unload(bmp);
					bmp							= bmpTemp;
					bpp							= FreeImage_GetBPP(bmp);
				} else {
					// do nothing we are grayscale
				}
				break;
			case 24:
				// do nothing we are color
				break;
			case 32:
				// do nothing we are colorAlpha
				break;
			default:
				FIBITMAP 	* bmpTemp =		FreeImage_ConvertTo24Bits(bmp);
				if (bmp != NULL)			FreeImage_Unload(bmp);
				bmp							= bmpTemp;
				bpp							= FreeImage_GetBPP(bmp);
		}


		int byteCount = bpp / 8;
        //------------------------------------------
		// call the allocation routine (which checks if really need to allocate) here:
		allocate(pix, width, height, bpp);

		FreeImage_ConvertToRawBits(pix.pixels, bmp, width*byteCount, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK, true);  // get bits

		#ifdef TARGET_LITTLE_ENDIAN
			if (byteCount != 1) swapRgb(pix);
		#endif
		//------------------------------------------


	if (bmp != NULL){
		FreeImage_Unload(bmp);
	}

    //###########################END OF COPY ofImage::loadImageIntoPixels

	printf("Deleting.\n");
	retval = gp_camera_file_delete(camera, camera_file_path.folder, camera_file_path.name,
			cameracontext);
	printf("  Retval: %d\n", retval);

	gp_file_free(camerafile);
	printf("Capturing finished!\n");

	return pix.pixels;
}

void ofxPhoto::swapRgb(ofPixels &pix){
	if (pix.bitsPerPixel != 8){
		int sizePixels		= pix.width*pix.height;
		int cnt				= 0;
		unsigned char temp;
		int byteCount		= pix.bitsPerPixel/8;

		while (cnt < sizePixels){
			temp					= pix.pixels[cnt*byteCount];
			pix.pixels[cnt*byteCount]		= pix.pixels[cnt*byteCount+2];
			pix.pixels[cnt*byteCount+2]		= temp;
			cnt++;
		}
	}
}

void ofxPhoto::allocate(ofPixels &pix, int width, int height, int bpp){

	bool bNeedToAllocate = false;
	if (pix.bAllocated == true){
		if ( (pix.width == width) && (pix.height == height) && (pix.bitsPerPixel == bpp)){
			//ofLog(OF_LOG_NOTICE,"we are good, no reallocation needed");
			bNeedToAllocate = false;
		 } else {
			delete[] pix.pixels;
			bNeedToAllocate = true;
		 }
	} else {
		bNeedToAllocate = true;
	}

	int byteCount = bpp / 8;

	if (bNeedToAllocate == true){
		pix.width			= width;
		pix.height			= height;
		pix.bitsPerPixel	= bpp;
		pix.bytesPerPixel	= bpp / 8;
		switch (pix.bitsPerPixel){
			case 8:
				pix.glDataType		= GL_LUMINANCE;
				pix.ofImageType		= OF_IMAGE_GRAYSCALE;
				break;
			case 24:
				pix.glDataType		= GL_RGB;
				pix.ofImageType		= OF_IMAGE_COLOR;
				break;
			case 32:
				pix.glDataType		= GL_RGBA;
				pix.ofImageType		= OF_IMAGE_COLOR_ALPHA;
				break;
		}

		pix.pixels			= new unsigned char[pix.width*pix.height*byteCount];
		pix.bAllocated		= true;
	}
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
	printf("Capturing finished!\n");
}
