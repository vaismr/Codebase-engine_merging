#pragma once
#include "GameObject.h"
#include "../CSC8503Common/StateMachine.h"

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
			EnemyAIPatrol(Vector3& pointA, Vector3& pointB, string name = "") : EnemyAI(name), A(pointA), B(pointB), isWaiting(true), waitTime(0.0f) {
				(A - GetTransform().GetWorldPosition()).Length() < 
					(B - GetTransform().GetWorldPosition()).Length() ? currentDest = A : currentDest = B;
				SetupStateMachine();
			}

			~EnemyAIPatrol() {}

			void Update(float dt) {
				if (isWaiting)
					waitTime += dt;
				distToCurrentDest = (currentDest - GetTransform().GetWorldPosition()).Length();
				sM->Update();
			}

			void SetPointA(const Vector3& pointA) { A = pointA; }
			void SetPointB(const Vector3& pointB) { B = pointB; }

		protected:
			Vector3 GetCurrentDestination() const { return currentDest; }
			// could expand to move through array of destinations
			void SwitchDestinations() { currentDest == A ? currentDest = B : currentDest = A; }

			bool GetIsWaiting() const { return isWaiting; }
			void SetIsWaiting(const bool isWaiting) { 
				this->isWaiting = isWaiting;
				if (!isWaiting)
					waitTime = 0.0f;
			}

		private:
			Vector3 currentDest;
			Vector3 A;
			Vector3 B;

			float distToCurrentDest;

			bool isWaiting;
			float waitTime;

			void SetupStateMachine();
		};
	}
}