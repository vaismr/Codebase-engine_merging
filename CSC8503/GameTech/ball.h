#pragma once
#include "../CSC8503Common/GameObject.h"
#include"..///CSC8503Common/PositionConstraint.h"

namespace NCL {
	namespace CSC8503
	{
		class Ball :public GameObject
		{

		public:
			Ball(string name = "");
			bool isOnFloor = false;

			//ball on water will slow down
			bool isOnWater = false;
			bool isSlowDown = false;
			bool hitIce = false;

			void OnCollisionBegin(GameObject* otherObject) override

			{
				if (otherObject->GetName()=="FLOOR")
				{
					this->isOnFloor = true;
					this->isOnWater = false;
				}
				else if (otherObject->GetName() == "WATER") {
					this->isOnFloor = false;
					this->isOnWater = true;
				}
				else if (otherObject->GetName() == "ICE") {
					this->hitIce = true;
					otherObject->SetActive(false);
				}
			}
		};
	}
}                                                                                                                        