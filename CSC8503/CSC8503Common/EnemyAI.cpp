#include "EnemyAI.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"

using namespace NCL;
using namespace CSC8503;

void EnemyAIChase::SetupStateMachine() {
	EnemyFunc idleFunc = [](void* enemy, void* player) {
		EnemyAIChase* enemyObject = (EnemyAIChase*)enemy;
		enemyObject->SetStateDescription("idle");
	};

	// @TODO raycast for this?
	EnemyFunc spottedPlayer = [](void* enemy, void* player) {
		EnemyAIChase* enemyObject = (EnemyAIChase*)enemy;
		GameObject* playerObject = (GameObject*)player;
		enemyObject->SetStateDescription("spotted player");
		Vector3 dir = playerObject->GetTransform().GetWorldPosition() - enemyObject->GetTransform().GetWorldPosition();
		dir.Normalise();
		float dirAngle = atan2(dir.x, dir.z);
		Quaternion orientation = Quaternion(0.0f, sin(dirAngle * 0.5f), 0.0f, cos(dirAngle * 0.5f));
		enemyObject->GetTransform().SetLocalOrientation(orientation);
	};

	EnemyFunc chaseFunc = [](void* enemy, void* player) {
		EnemyAIChase* enemyObject = (EnemyAIChase*)enemy;
		GameObject* playerObject = (GameObject*)player;
		enemyObject->SetStateDescription("chasing");
		Vector3 dir = playerObject->GetTransform().GetWorldPosition() - enemyObject->GetTransform().GetWorldPosition();
		dir.Normalise();
		float dirAngle = atan2(dir.x, dir.z);
		Quaternion orientation = Quaternion(0.0f, sin(dirAngle * 0.5f), 0.0f, cos(dirAngle * 0.5f));
		enemyObject->GetTransform().SetLocalOrientation(orientation);
		// @TODO pathfind?
		enemyObject->GetPhysicsObject()->AddForce(dir * 100.0f);
	};

	EnemyState* idleState = new EnemyState(idleFunc, this);
	EnemyState* spottedState = new EnemyState(spottedPlayer, this, chaseObject);
	EnemyState* chaseState = new EnemyState(chaseFunc, this, chaseObject);

	sM->AddState(idleState);
	sM->AddState(spottedState);
	sM->AddState(chaseState);

	GenericTransition<float&, float>* idleToSpotted = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::LessThanTransition, distToObject, 30.0f, idleState, spottedState);

	GenericTransition<float&, float>* spottedToIdle = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::GreaterThanTransition, distToObject, 40.0f, spottedState, idleState);


	GenericTransition<float&, float>* spottedToChase = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::LessThanTransition, distToObject, 15.0f, spottedState, chaseState);

	GenericTransition<float&, float>* chaseToSpotted = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::GreaterThanTransition, distToObject, 25.0f, chaseState, spottedState);

	sM->AddTransition(idleToSpotted);
	sM->AddTransition(spottedToIdle);
	sM->AddTransition(spottedToChase);
	sM->AddTransition(chaseToSpotted);
}

void EnemyAIPatrol::SetupStateMachine() {
	// @TODO idlefunc for this, i.e. when reach point, stop for 1 sec before moving to other point
	
	EnemyFunc patrolToA = [](void* enemy, void* A) {
		EnemyAIPatrol* enemyObject = (EnemyAIPatrol*)enemy;
		Vector3* dest = (Vector3*)A;
		Vector3 enemyPos = enemyObject->GetTransform().GetWorldPosition();
		enemyObject->SetStateDescription("patrolling to A");
		Vector3 dir = *dest - enemyPos;
		dir.Normalise();
		float dirAngle = atan2(dir.x, dir.z);
		Quaternion orientation = Quaternion(0.0f, sin(dirAngle * 0.5f), 0.0f, cos(dirAngle * 0.5f));
		enemyObject->GetTransform().SetLocalOrientation(orientation);
		// @TODO pathfind?
		enemyObject->GetPhysicsObject()->AddForce(dir * 100.0);
	};

	EnemyFunc patrolToB = [](void* enemy, void* B) {
		EnemyAIPatrol* enemyObject = (EnemyAIPatrol*)enemy;
		Vector3* dest = (Vector3*)B;
		Vector3 enemyPos = enemyObject->GetTransform().GetWorldPosition();
		enemyObject->SetStateDescription("patrolling to B");
		Vector3 dir = *dest - enemyPos;
		dir.Normalise();
		float dirAngle = atan2(dir.x, dir.z);
		Quaternion orientation = Quaternion(0.0f, sin(dirAngle * 0.5f), 0.0f, cos(dirAngle * 0.5f));
		enemyObject->GetTransform().SetLocalOrientation(orientation);
		// @TODO pathfind?
		enemyObject->GetPhysicsObject()->AddForce(dir * 100.0);
	};

	EnemyState* patrolToAState = new EnemyState(patrolToA, this, (void*)&A);
	EnemyState* patrolToBState = new EnemyState(patrolToB, this, (void*)&B);

	sM->AddState(patrolToAState);
	sM->AddState(patrolToBState);

	GenericTransition<float&, float>* AtoB = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::LessThanTransition, distToA, 5.0, patrolToAState, patrolToBState);

	GenericTransition<float&, float>* BtoA = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::LessThanTransition, distToB, 5.0, patrolToBState, patrolToAState);

	sM->AddTransition(AtoB);
	sM->AddTransition(BtoA);
}