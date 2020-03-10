#pragma once
#include "PxActor.h"
#include "PxRigidDynamic.h"
#include "CollisionVolume.h"
#include "Transform.h"
#include "../../Common/Keyboard.h"
#include "RenderObject.h"
#include "PhysicsObject.h"

using namespace physx;
using namespace NCL::CSC8503;
using namespace NCL;
using namespace std;

class RigidBody: PxRigidDynamic
{
public:
	RigidBody();
	~RigidBody() {};

	//void GenerateMass();

	void setType(PxActorType::Enum type) { RType = type; };
	PxActorType::Enum getType() { return RType; };

	/*void setBoundingVolume(CollisionVolume* vType) { boundingVolume = vType; };
	CollisionVolume* getBoundingVolume()const { return boundingVolume; };
	
    void setTransform(Transform T) { transform = T; }
	const Transform& GetConstTransform() const { return transform; };
	Transform& GetTransform() { return transform; };*/

	void setMass();
	float getMass() { return RMass; };
	float getInvMass() { 
		if (RMass != 0)
			return 1 / RMass;
		else
			return 0;
	};

	void setID(int i) { id = i; };
	int getID() { return id; };


	bool operator==(const RigidBody& obj)const {
		return id == obj.id;
	};

	bool IsActive() { return isActive; }

	/*RenderObject* GetRenderObject() { return renderObject; }

	PhysicsObject* GetPhysicsObject() { return physicsObject;}*/

	void BroadPhaseDetection();

	void setKinematicTarget(const PxTransform& destination);
	bool const getKinematicTarget(PxTransform& target);

	bool isSleeping() { return sleep; };
	void setSleepThreshold(PxReal threshold);

	PxReal const getSleepThreshold() { return alert_radius; };
	void setStabilizationThreshold(PxReal threshold) { alert_radius = threshold; };

	void setWakeCounter(PxReal wakeCounterValue) { count_down = wakeCounterValue; };
	PxReal getWakeCounter() { return count_down; };

	void wakeUp();
	void putToSleep();

	void setSolverIterationCounts(PxU32 minPositionIters, PxU32 minVelocityIters = 1);
	void const getSolverIterationCounts(PxU32& minPositionIters, PxU32& minVelocityIters);

	PxReal const getContactReportThreshold();
	void setContactReportThreshold(PxReal threshold);
	const char* getConcreteTypeName() const { return "PxRigidDynamic"; }

protected:
	Transform transform;
	PxActorType::Enum RType;
	CollisionVolume* boundingVolume;
	RenderObject* renderObject;
	PhysicsObject* physicsObject;
	bool	isActive;
	float RMass;
	int id;
	float radius;
	bool sleep=true;
	PxReal alert_radius;
	PxReal count_down;
};





class DynamicCharacter :public RigidBody {
public:
	DynamicCharacter();
	~DynamicCharacter();

	void SetBoundingVolume(CollisionVolume* vol) {
		boundingVolume = vol;
	}

	const CollisionVolume* GetBoundingVolume() const {
		return boundingVolume;
	}

	bool IsActive() const {
		return isActive;
	}

	const string& GetName() const {
		return name;
	}

protected:
	Transform		 transform;

	CollisionVolume* boundingVolume;

	bool	isActive;
	string	name;
	int ID;

};