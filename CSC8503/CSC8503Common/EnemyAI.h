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

			virtual void Update() {}

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

			void Update() {
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
			EnemyAIPatrol(Vector3& pointA, Vector3& pointB, string name = "") : EnemyAI(name), A(pointA), B(pointB) {
				distToA = (A - GetTransform().GetWorldPosition()).Length();
				distToB = (B - GetTransform().GetWorldPosition()).Length();
				SetupStateMachine();
			}

			~EnemyAIPatrol() {}

			void Update() {
				distToA = (A - GetTransform().GetWorldPosition()).Length();
				distToB = (B - GetTransform().GetWorldPosition()).Length();
				sM->Update();
			}

			void SetPointA(const Vector3& pointA) { A = pointA; }
			void SetPointB(const Vector3& pointB) { B = pointB; }

		private:
			Vector3 A;
			Vector3 B;

			float distToA;
			float distToB;

			void SetupStateMachine();
		};
	}
}