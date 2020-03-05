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
			bool onWater = false;
			//void OnCollisionBegin(GameObject* otherObject)

			void OnCollisionBegin(GameObject* otherObject) override
			{
		

				if (otherObject->GetName()=="WATER")
				{
					this->onWater = true;
				}
				else if (otherObject->GetName() == "FLOOR") {
					this->onWater = false;
				}
				//if (otherObject->GetCollisitionType() == CollisionType::BRIDGE && !IsOnBridge)
				//{
				//	SetIsOnBridge(true);
				//}
				//if (otherObject->GetCollisitionType() == CollisionType::FLOOR && IsOnWater)
				//{
				//	SetIsOnWater(false);
				//	SetIsOnBridge(false);
				//}
			}
		};
	}
}                                                                                                                        