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

	// @TODO stop pushing player once collided
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
		enemyObject->GetPhysicsObject()->AddForce(dir * enemyObject->GetSpeedMultiplier());
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

	EnemyFunc waitFunc = [](void* enemy, void* data) {
		EnemyAIPatrol* enemyObject = (EnemyAIPatrol*)enemy;
		enemyObject->SetStateDescription("waiting");
		if (!enemyObject->GetIsWaiting()) {
			enemyObject->SetIsWaiting(true);
			enemyObject->SwitchDestinations();
		}
	};

	EnemyFunc patrolToPoint = [](void* enemy, void* data) {
		EnemyAIPatrol* enemyObject = (EnemyAIPatrol*)enemy;
		if (enemyObject->GetIsWaiting())
			enemyObject->SetIsWaiting(false);
		Vector3 dest = enemyObject->GetCurrentDestination();
		Vector3 enemyPos = enemyObject->GetTransform().GetWorldPosition();
		enemyObject->SetStateDescription("patrolling");
		Vector3 dir = dest - enemyPos;
		dir.Normalise();
		float dirAngle = atan2(dir.x, dir.z);
		Quaternion orientation = Quaternion(0.0f, sin(dirAngle * 0.5f), 0.0f, cos(dirAngle * 0.5f));
		enemyObject->GetTransform().SetLocalOrientation(orientation);
		// @TODO pathfind?
		enemyObject->GetPhysicsObject()->AddForce(dir * enemyObject->GetSpeedMultiplier());
	};

	EnemyState* waitState = new EnemyState(waitFunc, this);
	EnemyState* patrolToPointState = new EnemyState(patrolToPoint, this);

	sM->AddState(waitState);
	sM->AddState(patrolToPointState);

	GenericTransition<float&, float>* patrolToWait = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::LessThanTransition, distToCurrentDest, 5.0, patrolToPointState, waitState);
	
	GenericTransition<float&, float>* waitToPatrol = new GenericTransition<float&, float>(
		GenericTransition<float&, float>::GreaterThanTransition, waitTime, 1.0, waitState, patrolToPointState);

	sM->AddTransition(waitToPatrol);
	sM->AddTransition(patrolToWait);
}