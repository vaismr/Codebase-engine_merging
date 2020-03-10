#include "RigidBody.h"

void RigidBody::setMass() {
	switch (RType)
	{
	case physx::PxActorType::eRIGID_STATIC:
		RMass = 0;
		break;
	case physx::PxActorType::eRIGID_DYNAMIC:
		RMass = 100.0;
		break;
	case physx::PxActorType::eARTICULATION_LINK:
		break;
	case physx::PxActorType::eACTOR_COUNT:
		break;
	case physx::PxActorType::eACTOR_FORCE_DWORD:
		break;
	default:
		break;
	}
}

void RigidBody::BroadPhaseDetection() {
	if (!boundingVolume)
		return;
	/*switch (RType) {
	case PxActorType::eRIGID_STATIC:
		
	}*/
}