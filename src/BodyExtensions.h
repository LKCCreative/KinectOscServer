#pragma once

#include "ofMain.h"
#include "ofxKinectForWindows2.h"

namespace ofxKinectForWindows2 {
	namespace Source {
		// -------
		class CustomBody : public Body {
		public:
			
			ofVec3f getHeadPositions();

		};
	}
}