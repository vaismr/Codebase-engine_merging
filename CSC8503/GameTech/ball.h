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
					if (this->GetName() == "FIRE") {
						this->SetName("BALL");
						this->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
					}
					if (this->GetName() == "ICE") {
						otherObject->GetRenderObject()->SetColour(Vector4(0.0f, 0.8f, 0.8f, 1.0f));
						otherObject->SetName("ICE");
					}
				}
				else if (otherObject->GetName() == "FIREPOWERUP") {
					this->SetName("FIRE");
					this->GetRenderObject()->SetColour(Vector4(0.5f, 0.1f, 0.1f, 1.0f));
				}
				else if (otherObject->GetName() == "ICEPOWERUP") {
					this->SetName("ICE");
					this->GetRenderObject()->SetColour(Vector4(0.0f, 0.8f, 0.8f, 1.0f));
				}
				else if (this->GetName() == "FIRE" && otherObject->GetName() == "ICE") {
					this->hitIce = true;
					otherObject->SetActive(false);
				}
			}
		};
	}
}                                                                                                                        