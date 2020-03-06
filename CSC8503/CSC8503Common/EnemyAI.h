#pragma once
#include "GameObject.h"
#include "../CSC8503Common/StateMachine.h"
#include <vector>

namespace NCL {
	namespace CSC8503 {
		class NetworkObject;

		class EnemyAI : public GameObject {
		public:
			EnemyAI(string name = "") : GameObject(name), sM(new StateMachine()) {}

			virtual ~EnemyAI() { delete sM; }

			virtual void Update(float dt) {}

			void SetSpeedMultiplier(const float multiplier) { speedMultiplier = multiplier; }
			float GetSpeedMultiplier() const { return speedMultiplier; }

			void SetStateDescription(const string description) { stateDescription = description; }
			string GetStateDescription() const { return stateDescription; }

		protected:
			StateMachine* sM;
			string stateDescription = "";
			float speedMultiplier = 100.0f;
			
			virtual void SetupStateMachine() {}
		};



		class EnemyAIChase : public EnemyAI {
		public:
			EnemyAIChase(GameObject* chaseObject, string name = "") : EnemyAI(name), chaseObject(chaseObject), distToObject(0) {
				SetupStateMachine();
			}

			~EnemyAIChase() {}

			void Update(float dt) {
				distToObject = (chaseObject->GetTransform().GetWorldPosition() - GetTransform().GetWorldPosition()).Length();
				sM->Update();
			}

		private:
			GameObject* chaseObject;
			float distToObject;

			void SetupStateMachine();
		};



		class EnemyAIPatrol : public EnemyAI {
		public:
			EnemyAIPatrol(std::vector<Vector3> points, GameObject* chaseObject = nullptr, string name = "") : EnemyAI(name), chaseObject(chaseObject), points(points) {
				isWaiting = true;
				hasReachedEnd = false;
				waitTime = 0.0f;
				index = 0;
				currentDest = points[index];
				distToCurrentDest = (currentDest - GetTransform().GetWorldPosition()).Length();
				distToObject = 30.0f;
				SetupStateMachine();
			}

			~EnemyAIPatrol() {}

			void Update(float dt) {
				if (isWaiting)
					waitTime += dt;
				distToCurrentDest = (currentDest - GetTransform().GetWorldPosition()).Length();
				distToObject = (chaseObject->GetTransform().GetWorldPosition() - GetTransform().GetWorldPosition()).Length();
				sM->Update();
			}

			void SetPoints(const std::vector<Vector3>& points) { this->points = points; }

		protected:
			Vector3 GetCurrentDestination() const { return currentDest; }

			void NextDestination() {
				if (index == 0)
					hasReachedEnd = false;
				if (index < (points.size() - 1) && !hasReachedEnd) {
					currentDest = points[index++];
				}
				else if (index > 0) {
					hasReachedEnd = true;
					currentDest = points[index--];
				}
			}

			bool GetIsWaiting() const { return isWaiting; }
			void SetIsWaiting(const bool isWaiting) { 
				this->isWaiting = isWaiting;
				if (!isWaiting)
					waitTime = 0.0f;
			}

		private:
			Vector3 currentDest;
			GameObject* chaseObject;

			std::vector<Vector3> points;
			int index;
			bool hasReachedEnd;

			float distToCurrentDest;
			float distToObject;

			bool isWaiting;
			float waitTime;

			void SetupStateMachine();
		};
	}
}