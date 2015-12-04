#include "ofMain.h"
#include "ofxKinectForWindows2.h"
#include "BodyExtensions.h"

namespace ofxKinectForWindows2 {
	namespace Source {

		//----------
		vector<bodyData> CustomBody::getBodyData() {

			const auto & bonesAtlas = Data::Body::getBonesAtlas();

			vector<bodyData> bodiesData;

			

			for (auto & body : bodies) {
				if (!body.tracked) continue;
				

				//populate positions
				vector<ofVec3f> positions;
				for (auto & j : body.joints) {
					//print the joint location (3d)
					if (j.second.getType() == JointType_Head) {
						positions.push_back(j.second.getPosition());
					}
				}

				bodyData data;
				data.id = body.trackingId;
				data.positions = positions;

				bodiesData.push_back(data);
			}


			return bodiesData;
		}

	}
}
