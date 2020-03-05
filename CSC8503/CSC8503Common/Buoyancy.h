#pragma once
#include "GameObject.h"
#include"Constraint.h"
using namespace NCL::CSC8503;

#include "GameObject.h"
#include "Constraint.h"

using namespace NCL::CSC8503;

class Buoyancy : public Constraint
{
public:
	Buoyancy(GameObject* obj);
	~Buoyancy();


	void UpdateConstraint(float dt) override;

	GameObject* GetBuoyancyObject();
private:
	float forceIntensity;
	GameObject* object;
	float time;
};

