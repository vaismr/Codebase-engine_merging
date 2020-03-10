#pragma once
#include "Transform.h"
#include "CollisionVolume.h"

#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "../CSC8503Common/RigidBody.h"

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
			virtual ~GameObject();

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

			void SetActive(bool active) {
				this->isActive = active;

			}
			void deletethis() {
				this->isActive = false;
				this->GetRenderObject()->SetMesh(nullptr);
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

			RigidBody* GetBoundingDynamicObject() const{
				return dynamicObject;
			}

			void SetRenderObject(RenderObject* newObject) {
				renderObject = newObject;
			}

			void SetPhysicsObject(PhysicsObject* newObject) {
				physicsObject = newObject;
			}

			void setBoundingDynamicObject(RigidBody* newObject) {
				dynamicObject = newObject;
			}

			const string& GetName() const {
				return name;
			}

			void SetName(string newname) {
				name = newname;
			}

			void SetID(int id) {
				ID = id;
			}

			int GetID() {
				return ID;
			}

			virtual void OnCollisionBegin(GameObject* otherObject) {
				//std::cout << "OnCollisionBegin event occured!\n";
			}

			virtual void OnCollisionEnd(GameObject* otherObject) {
				//std::cout << "OnCollisionEnd event occured!\n";
			}

			bool GetBroadphaseAABB(Vector3&outsize) const;

			void UpdateBroadphaseAABB();

			bool operator==(const GameObject& obj)const {
				return ID == obj.ID;
			}
#pragma region 04/03 collisionType

			//void SetCollisionType(CollisionType);
			//CollisionType GetCollisitionType();

			//void SetIsOnWater(bool b);
			//bool GetIsOnWater();
			//void SetIsOnBridge(bool b);
			//bool GetIsOnBridge();
			//void SetIsBuoyancyAdded(bool b);
			//bool GetIsBuoyancyAdded();

#pragma endregion 

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
			RigidBody*          dynamicObject;

			bool	isActive;
			bool	isStatic;
			string	name;
			int ID;

			Vector3 broadphaseAABB;
		};
	}
}

