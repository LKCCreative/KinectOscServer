#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"

namespace ofxKinectForWindows2 {
	namespace Source {

		struct bodyData { int id; vector<ofVec3f> positions; };

		// -------
		class CustomBody : public Body {
		public:
			vector<bodyData> getBodyData();

		};
	}
}