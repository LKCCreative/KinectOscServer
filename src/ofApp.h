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
		void exit(ofEventArgs &args);
		void captureBackground();

		void updateDepthImage();

		ofxKFW2::Device kinect;
		
		ofxKinectBlobFinder tracker;

		ofImage depthImg, bgImg, maskedImg;

		ofxCvGrayscaleImage depthCv;

		ofShader shaderThreshold, shaderSubtract;

		ofEasyCam camera;

		ofxGuiGroup group;

		ofxPanel gui;
		ofParameter<int> thresh2D, minPoints, maxBlobs, trackedBlobs, hostport;
		ofParameter<float> thresholdNear, thresholdFar, minVol, maxVol, bg_tolerance;
		ofParameter<ofVec3f> boxMin, boxMax, thresh3D;
		ofParameter<string> hostip;
		
		ofxButton backCap;


		bool visible, save;

		ofxOscSender sender;

		ofFile record;
		
};
