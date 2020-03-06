#include "Buoyancy.h"

Buoyancy::Buoyancy(GameObject* obj)
{
	forceIntensity = 0.f;
	object = obj;
	time = 0;
}

Buoyancy::~Buoyancy()
{
}

void Buoyancy::UpdateConstraint(float dt)
{
	time += dt;
	std::cout << "forceIntensity:" << forceIntensity << std::endl;
	forceIntensity = sin(time * 10);
	if (forceIntensity > 0)
	{
		object->GetPhysicsObject()->AddForce(Vector3(0,15, 0) * forceIntensity);
	}
}

GameObject* Buoyancy::GetBuoyancyObject()
{
	return object;
}