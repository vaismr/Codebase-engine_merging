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
			bool collisionwater = false;
			//void OnCollisionBegin(GameObject* otherObject)

			void OnCollisionBegin(GameObject* otherObject) override
			{
		

				if (otherObject->GetName()=="WATER")
				{
					//SetIsOnWater(true);
					this->collisionwater = true;
					std:: cout << "colwater" << std::endl;
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