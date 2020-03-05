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
			bool onBridge = false;
			bool onButton = false;


			//void OnCollisionBegin(GameObject* otherObject)

			void OnCollisionBegin(GameObject* otherObject) override
			{
		

				if (otherObject->GetName()=="WATER")
				{
					this->onWater = true;
					this->onBridge = false;
				}
				if (otherObject->GetName() == "BRIDGE")
				{
					this->onBridge = true;
				}
				if (otherObject->GetName() == "DOOR")
				{
					this->onButton = true;
					this->onBridge = false;
				}
				else if(otherObject->GetName()=="FLOOR"){
					this->onWater = false;
					this->onBridge = false;
					this->onButton = false;

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
			
			
			void clearcol()
				{
				onBridge = false;

				}
		};
	}
}                                                                                                                        