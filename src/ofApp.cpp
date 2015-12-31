#include "ofApp.h"

#define DEPTH_WIDTH 512
#define DEPTH_HEIGHT 424
#define DEPTH_SIZE DEPTH_WIDTH * DEPTH_HEIGHT

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

	ofDisableArbTex();

	//the depth image to analyse
	depthImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
	depthImg.setColor(ofColor::black);
	depthImg.update();

	//background image to subtract
	bgImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
	bgImg.setColor(ofColor::black);
	bgImg.update();

	//final masked image
	maskedImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
	maskedImg.setColor(ofColor::black);
	maskedImg.update();

	//maskedFbo for subtraction ops
	//maskedFbo.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, GL_RED);

	//background pixel container
	bgPix.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, 1);
	bgPix.setColor(ofColor(0));

	float sqrResolution = tracker.getResolution();
	sqrResolution *= sqrResolution;
	//minPoints = (int)(0.001*(float)(512 * 424)/sqrResolution);

	camera.setPosition(ofGetWidth() / 4.0, ofGetHeight() / 2.0, 1000.);
	camera.lookAt(ofVec3f(0));

	thresholdNear.set("Threshold Near", 1, 0.1, 5);
	thresholdFar.set("Threshold Far", 2.2, 0.1, 8);
	bg_tolerance.set("BG Tolerance", 20, 12, 60);

	boxMin.set("boxMin", ofVec3f(-8), ofVec3f(-10), ofVec3f(0));
	boxMax.set("boxMax", ofVec3f(8), ofVec3f(0), ofVec3f(10));
	thresh3D.set("thresh3D", ofVec3f(.2, .2, .3), ofVec3f(0), ofVec3f(1));

	thresh2D.set("thresh2D", 1, 0, 255);
	minVol.set("minVol", 0.01, 0, 0.5);
	maxVol.set("maxVol", 2, 0, 40);
	minPoints.set("minPoints", 50, 0, 500);
	maxBlobs.set("maxBlobs", 10, 0, 100);
	trackedBlobs.set("trackdBlobs", 0, 0, maxBlobs);

	gui.setup("Settings", "settings.xml");
	gui.add(thresholdNear);
	gui.add(thresholdFar);
	gui.add(bg_tolerance);
	gui.add(boxMin);
	gui.add(boxMax);
	gui.add(thresh3D);
	gui.add(thresh2D);
	gui.add(minVol);
	gui.add(maxVol);
	gui.add(minPoints);
	gui.add(maxBlobs);
	gui.add(trackedBlobs);
	gui.loadFromFile("settings.xml");

	visible = true;
	save = false;

	string HOST, PORT;

	//load info from the config and store it
	ifstream config;
	config.open(ofToDataPath("config.txt").c_str());
	config >> HOST >> PORT;
	config.close();
	
	sender.setup(HOST, stoi(PORT));
	//sender.setup("localhost", 9000);

	ofSetFrameRate(60);
}

//--------------------------------------------------------------
void ofApp::update() {
	//update the kinect device
	kinect.update();

	//handle blob tracking stuff
	static bool bTrackerInit = false;

	// kinect 2 init is async, so just chill till it's ready, OK?
	if (kinect.getDepthSource()->isFrameNew()) {
		if (!bTrackerInit) {
			bTrackerInit = true;
			tracker.init(&kinect, false);
			tracker.setScale(ofVec3f(0.001));
		}

		updateDepthImage();
	}

	if (tracker.isInited()) {
		tracker.findBlobs(&maskedImg, boxMin, boxMax, thresh3D, thresh2D, minVol, maxVol, minPoints, maxBlobs);
		trackedBlobs = tracker.nBlobs;
	}


	//send kinect skeleton data via OSC -->
	//OSC address format --> /body{0}/skeleton or /body{0}/gesture .format(bodyIndex)
	//Unity will parse the body # to match gestures to body position

	/*

	auto* myBod = static_cast<ofxKFW2::Source::CustomBody*>(kinect.getBodySource().get());

	vector<ofxKFW2::Source::bodyData> & data = myBod->getBodyData();

	//vector<gestureType> & gestures = myBod->getGesture();

	ofxOscBundle bundle;

	//add position messages
	for (ofxKFW2::Source::bodyData d : data) {
		ofxOscMessage m;
		//the address should store the index of the body being tracked
		//does the address need to change based on which kinect we're getting the data from?
		//or can that be surmised by the Unity app?
		m.setAddress("/body/positions");
		m.addIntArg(d.id);
		//m.addIntArd(p.bodyIndex) //the body index. p needs to become a struct that holds this data
		for (ofVec3f p : d.positions) {
			m.addFloatArg(p.x);
			m.addFloatArg(p.y);
			m.addFloatArg(p.z);
		}
		bundle.addMessage(m);
	}

	
	for (gestureType p : gestures) {
		ofxOscMessage m;
		m.setAddress("/body/gesture");
		m.addIntArg(p.bodyIndex); //the body index
		m.addStringArg(p.gestureName); //name of the gesture
		bundle.addMessage(m);
	}

	sender.sendBundle(bundle);

	*/
	
	
}

//--------------------------------------------------------------
void ofApp::updateDepthImage() {
	if (!depthImg.isAllocated()) {
		depthImg.allocate(DEPTH_WIDTH, DEPTH_HEIGHT, OF_IMAGE_GRAYSCALE);
	}

	auto& depthPix = kinect.getDepthSource()->getPixels();

	for (int y = 0; y < DEPTH_HEIGHT; y++) {
		for (int x = 0; x < DEPTH_WIDTH; x++) {
			int index = x + (y*DEPTH_WIDTH);
			// Multiply thresholds by 1000 because the values are in meters in world
			// space but in mm in the depthPix array
			float depth = depthPix[index];
			float val = depth == 0 ? 0 : ofMap(depth, thresholdNear * 1000, thresholdFar * 1000, 255, 0, true);
			depthImg.setColor(x, y, val);
		}
	}

	depthImg.update();

	auto d = depthImg.getPixels();
	auto b = bgImg.getPixels();
	for (int y = 0; y < DEPTH_HEIGHT; y++) {
		for (int x = 0; x < DEPTH_WIDTH; x++) {
			int index = x + (y*DEPTH_WIDTH);

			// Build the 8bit, thresholded image for drawing to screen
			if (d.getWidth() && d.getHeight()) {
				// Multiply thresholds by 1000 because the values are in meters in world
				// space but in mm in the depthPix array
				float diff = d[index] - b[index];
				float val = diff > bg_tolerance ? d[index] : 0;
				maskedImg.setColor(x, y, ofColor(val));
			}
		}
	}

	//maybe add some GUI controls for dilation/erosion
	maskedImg.update();
	depthCv.setFromPixels(maskedImg.getPixelsRef());
	//
	depthCv.erode_3x3();
	depthCv.erode();
	depthCv.dilate_3x3();
	maskedImg.setFromPixels(depthCv.getPixels());
	maskedImg.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	//kinect.getDepthSource()->draw(0, 0, previewWidth, previewHeight);  // note that the depth texture is RAW so may appear dark

	if (tracker.isInited()) {

		ofSetColor(255);
		depthImg.draw(ofGetWidth() - DEPTH_WIDTH / 2., 0, DEPTH_WIDTH / 2., DEPTH_HEIGHT / 2.);
		bgImg.draw(ofGetWidth() - DEPTH_WIDTH, 0, DEPTH_WIDTH / 2., DEPTH_HEIGHT / 2.);

		maskedImg.draw(ofGetWidth() - DEPTH_WIDTH / 2., DEPTH_HEIGHT / 2., DEPTH_WIDTH / 2., DEPTH_HEIGHT / 2.);

		glPointSize(3.0);
		ofPushMatrix();
		camera.begin();

		//ofTranslate(ofGetWidth()/2.0, 0.);
		ofScale(100., 100., 100.);
		ofEnableDepthTest();

		// draw blobs
		for (unsigned int i = 0; i < tracker.blobs.size(); i++) {
			ofPushMatrix();
			ofColor color;
			color.setSaturation(200);
			color.setBrightness(225);
			color.setHue(ofMap(i, 0, tracker.blobs.size(), 0, 255));

			ofSetColor(color);

			// draw blobs
			tracker.blobs[i].draw();

			ofPopMatrix();

			ofVec3f bbMax = tracker.blobs[i].boundingBoxMax;
			ofVec3f bbMin = tracker.blobs[i].boundingBoxMin;

			ofNoFill();
			ofDrawBox(tracker.blobs[i].centroid, tracker.blobs[i].dimensions.x, tracker.blobs[i].dimensions.y, tracker.blobs[i].dimensions.z);
			ofFill();
		}
		ofSetColor(255);
		ofDisableDepthTest();
		camera.end();
		ofPopMatrix();

		if (visible) gui.draw();

	}

																	   // Color is at 1920x1080 instead of 512x424 so we should fix aspect ratio
	//float colorHeight = previewWidth * (kinect.getColorSource()->getHeight() / kinect.getColorSource()->getWidth());
	//float colorTop = (previewHeight - colorHeight) / 2.0;
	//kinect.getColorSource()->draw(previewWidth, 0 + colorTop, previewWidth, colorHeight);
	

	//kinect.getInfraredSource()->draw(0, 0, ofGetWidth(), ofGetHeight());
	kinect.getBodySource()->drawProjected(0, 0, ofGetWidth(), ofGetHeight());

	//kinect.getBodyIndexSource()->draw(previewWidth, previewHeight, previewWidth, previewHeight);
	//kinect.getBodySource()->drawProjected(previewWidth, previewHeight, previewWidth, previewHeight, ofxKFW2::ProjectionCoordinates::DepthCamera);

	//shaderThreshold.begin();
	//ofDrawRectangle(0, 0, 204, 204);
	//shaderThreshold.end();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
	if (key == 'g') {
		visible ^= 1;
	}

	if (key == 's') {
		//save = false;
		//gui.saveToFile("settings.xml");
	}
	if (key == ' ') {
		//set background thingy
		auto& bgPix = kinect.getDepthSource()->getPixels();
		for (int y = 0; y < DEPTH_HEIGHT; y++) {
			for (int x = 0; x < DEPTH_WIDTH; x++) {
				int index = x + (y*DEPTH_WIDTH);

				// Build the 8bit, thresholded image for drawing to screen
				if (bgPix.getWidth() && bgPix.getHeight()) {
					// Multiply thresholds by 1000 because the values are in meters in world
					// space but in mm in the depthPix array
					float depth = bgPix[index];
					float val = depth == 0 ? 0 : ofMap(depth, thresholdNear * 1000, thresholdFar * 1000, 255, 0, true);
					bgImg.setColor(x, y, ofColor(val));
				}
			}
		}

		bgImg.update();
		ofLog() << "Captured Background";
	}
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
