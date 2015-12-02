#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "BodyExtensions.h"

namespace ofxKinectForWindows2 {
	namespace Source {

		//----------
		ofVec3f CustomBody::getHeadPositions() {

			const auto & bonesAtlas = Data::Body::getBonesAtlas();

			ofVec3f pos = ofVec3f();

			for (auto & body : bodies) {
				if (!body.tracked) continue;

				map<JointType, ofVec2f> jntsProj;

				for (auto & j : body.joints) {

					//print the joint location (3d)
					if (j.second.getType() == JointType_Head) {
						pos = j.second.getPosition();
					}

				}
			}

			return pos;
		}
	}
}
