#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"

namespace ofxKinectForWindows2 {
	namespace Source {

		struct bodyPart { ofVec3f pos; _JointType type; };

		struct bodyData { int id; vector<bodyPart> positions; };

		// -------
		class CustomBody : public Body {
		public:
			vector<bodyData> getBodyData();

		};
	}
}