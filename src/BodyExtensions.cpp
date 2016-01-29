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
				vector<bodyPart> positions;
				for (auto & j : body.joints) {
					bodyPart p;
					p.type = j.second.getType();
					p.pos = j.second.getPosition();
					positions.push_back(p);
					//print the joint location (3d)
					/*
					if (j.second.getType() == JointType_SpineShoulder) {
						bodyPart p;
						p.pos = j.second.getPosition()
						positions.push_back();
					}
					if (j.second.getType() == JointType_HandRight) {
						positions.push_back(j.second.getPosition());
					}
					*/
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
