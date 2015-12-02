#include "ofApp.h"

int previewWidth = 640;
int previewHeight = 480;

//--------------------------------------------------------------
void ofApp::setup() {
	kinect.open();
	kinect.initDepthSource();
	kinect.initColorSource();
	kinect.initInfraredSource();
	kinect.initBodySource();
	kinect.initBodyIndexSource();

	ofSetWindowShape(previewWidth * 2, previewHeight * 2);

	sender.setup("localhost", 8000);

	ofSetFrameRate(4);
}

//--------------------------------------------------------------
void ofApp::update() {
	kinect.update();


	//send kinect skeleton data via OSC -->
	//OSC address format --> /body{0}/skeleton or /body{0}/gesture .format(bodyIndex)
	//Unity will parse the body # to track individual skels

	ofVec3f & p = kinect.getBodySource()->getHeadPositions();

	ofxOscBundle bundle;
	ofxOscMessage m;
	m.setAddress("/skeleton/head");
	m.addFloatArg(p.x);
	m.addFloatArg(p.y);
	m.addFloatArg(p.z);
	bundle.addMessage(m);
	sender.sendBundle(bundle);
	
}

//--------------------------------------------------------------
void ofApp::draw() {
	kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark

																	   // Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio
	float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
	float colorTop = (previewHeight - colorHeight) / 2.0;
	//kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
	

	kinect.getInfraredSource()->draw(0, 0, ofGetWidth(), ofGetHeight());
	kinect.getBodySource()->drawProjected(0, 0, ofGetWidth(), ofGetHeight());

	//kinect.getBodyIndexSource()->draw(previewWidth, previewHeight, previewWidth, previewHeight);
	//kinect.getBodySource()->drawProjected(previewWidth, previewHeight, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
