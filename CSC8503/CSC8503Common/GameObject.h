#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"

#include <vector>

using std::vector;
enum CollisionType
{

	DEFAULT,
	FLOOR,
	BRIDGE,
	WATER,
	HOME,
	

};

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;

		class GameObject	{
		public:
			GameObject(string name = "");
			~GameObject();

			string Getobjectname() {
				return this->name;

			}
			void SetBoundingVolume(CollisionVolume* vol) {
				boundingVolume = vol;
			}

			const CollisionVolume* GetBoundingVolume() const {
				return boundingVolume;
			}

			bool IsActive() const {
				return isActive;
			}

			void SetStatic(bool isStat) {
				isStatic = isStat;
			}

			bool IsStatic() const {
				return isStatic;
			}

			const Transform& GetConstTransform() const {
				return transform;
			}

			Transform& GetTransform() {
				return transform;
			}

			RenderObject* GetRenderObject() const {
				return renderObject;
			}

			PhysicsObject* GetPhysicsObject() const {
				return physicsObject;
			}

			NetworkObject* GetNetworkObject() const {
				return networkObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();
#pragma region 04/03 collisionType
			void SetCollisionType(CollisionType);
			CollisionType GetCollisitionType();

			void SetIsOnWater(bool b);
			bool GetIsOnWater();
			void SetIsOnBridge(bool b);
			bool GetIsOnBridge();
			//void SetIsBuoyancyAdded(bool b);
			bool GetIsBuoyancyAdded();

#pragma endregion 


			void SetStateDescription(const string description) { stateDescription = description; }
			string GetStateDescription() const { return stateDescription; }

		protected:
			Transform			transform;
			CollisionType collisionlayer;
			bool IsOnWater;
			bool IsOnBridge;
			bool isBuoyancyAdded;


			CollisionVolume*	boundingVolume;
			PhysicsObject*		physicsObject;
			RenderObject*		renderObject;
			NetworkObject*		networkObject;

			bool	isActive;
			bool	isStatic;
			string	name;
			string stateDescription = "N/A";

			Vector3 broadphaseAABB;
		};
	}
}

