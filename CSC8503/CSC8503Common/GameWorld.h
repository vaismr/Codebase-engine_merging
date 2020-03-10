#pragma once
#include <vector>
#include "Ray.h"
#include "CollisionDetection.h"
#include "QuadTree.h"
#include "RigidBody.h"
using namespace physx;
namespace NCL {
		class Camera;
		using Maths::Ray;
	namespace CSC8503 {
		class GameObject;
		class Constraint;

		typedef std::function<void(GameObject*)> GameObjectFunc;
		typedef std::vector<GameObject*>::const_iterator GameObjectIterator;

		class GameWorld	{
		public:
			GameWorld();
			~GameWorld();

			void Clear();
			void ClearAndErase();

			void AddGameObject(GameObject* o);
			void AddRigidBody(RigidBody* o);
			void RemoveGameObject(GameObject* o);
			void RemoveRigidBody(RigidBody* o);

			void AddConstraint(Constraint* c);
			void RemoveConstraint(Constraint* c);
#pragma region floating on water
			void AddBuoyancy(Constraint* c);
			void ClearBuoyancies();

#pragma endregion
			Camera* GetMainCamera() const {
				return mainCamera;
			}

			void ShuffleConstraints(bool state) {
				shuffleConstraints = state;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			bool Raycast(Ray& r, RayCollision& closestCollision, bool closestObject = false) const;

			virtual void UpdateWorld(float dt);

			void OperateOnContents(GameObjectFunc f);

			void GetObjectIterators(
				GameObjectIterator& first,
				GameObjectIterator& last) const;

			bool GetConstraintIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;
			bool GetBuoyancyIterators(
				std::vector<Constraint*>::const_iterator& first,
				std::vector<Constraint*>::const_iterator& last) const;

		protected:
			void UpdateTransforms();
			void UpdateQuadTree();
			void UpdateConstraint();


			std::vector<GameObject*> gameObjects;
			std::vector<RigidBody*>rigidbodies;

			std::vector<Constraint*> constraints;
			std::vector<Constraint*> buoyancies;


			QuadTree<GameObject*>* quadTree;

			Camera* mainCamera;

			bool shuffleConstraints;
			bool shuffleObjects;
		};
	}
}

