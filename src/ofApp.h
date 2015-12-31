#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxGui.h"
#include "ofxOpenCv.h"
#include "ofxKinectBlobFinder.h"
#include "ofxKinectForWindows2.h"
#include "BodyExtensions.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);

		void updateDepthImage();

		ofxKFW2::Device kinect;
		
		ofxKinectBlobFinder tracker;

		ofImage depthImg, bgImg, maskedImg;
		ofFbo maskedFbo;

		ofxCvGrayscaleImage depthCv;

		ofPixels bgPix;

		ofShader shaderThreshold, shaderSubtract;

		ofEasyCam camera;

		ofxPanel gui;
		ofParameter<int> thresh2D, minPoints, maxBlobs, trackedBlobs;
		ofParameter<float> thresholdNear, thresholdFar, minVol, maxVol, bg_tolerance;
		ofParameter<ofVec3f> boxMin, boxMax, thresh3D;

		bool visible, save;

		ofxOscSender sender;
		
};
