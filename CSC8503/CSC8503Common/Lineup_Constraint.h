#pragma once
#include "PxConstraint.h"
#include "GameObject.h"
#include "GameObject.h"
#include <vector>
using namespace std;
using namespace physx;
//using namespace NCL::CSC8503;

namespace NCL {
	namespace CSC8503 {
		class Lineup_Constraints : PxConstraint {
		public:
			Lineup_Constraints();
			~Lineup_Constraints() {};

			bool HasAdded(GameObject* obj);
			void initiateLine(vector<GameObject*>* L) { line = L; };
			void release() {
				line->pop_back();
			};

			void getForce(Vector3& linear, Vector3& angular);
			bool isValid();
			float getBreakForce() { return break_force; };
			void setBreakForce(float F) { break_force = F; };

			void AddObject(GameObject* obj) {
				line->emplace_back(obj);
			};

			void PickUpItems(GameObject* obj0, GameObject* obj, Lineup_Constraints* constraint);
			bool ifTouched(const GameObject* objA, const GameObject* objB);


			vector<GameObject*>* line;
			float force;
			float break_force;

			//protected:

		};
	}
}