#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    photo.init();
    bCamIsBusy = false;
}

//--------------------------------------------------------------
void testApp::update(){

                if(photo.captureSucceeded()){
                    printf("Try to loading data...\n");
                    pic = photo.capture();
                    test.setFromPixels(pic,photo.getCaptureWidth(),photo.getCaptureHeight(),OF_IMAGE_COLOR,0);
                    printf("Loading finished!\n");
                    test.resize(1024,768);
                }
}

//--------------------------------------------------------------
void testApp::draw(){
    test.draw(0,0);
    ofDrawBitmapString(ofToString(ofGetFrameRate()) + " fps",20,20);
    ofDrawBitmapString("Press space to take a photo...",20,35);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    if(key == ' '){
        if(!photo.isBusy()){
            photo.startCapture();
        }
        else {
            printf("Cam is busy.\n");
        }
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

void testApp::exit(){
    photo.exit();
}
