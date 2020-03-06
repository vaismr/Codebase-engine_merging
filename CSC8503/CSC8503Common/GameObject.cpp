#include "GameObject.h"
#include "CollisionDetection.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
	collisionlayer = CollisionType::DEFAULT;
	IsOnWater = false;
	IsOnBridge = false;
	isBuoyancyAdded = false;

}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

//These would be better as a virtual 'ToAABB' type function, really...
void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetWorldOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
}
//void GameObject::SetCollisionType(CollisionType b)
//{
//	this->collisionlayer = b;
//
//}
//CollisionType GameObject::GetCollisitionType(){
//	return collisionlayer;
//
//}
//void GameObject::SetIsOnWater(bool b)
//{
//	IsOnWater = b;
//}
//
//bool GameObject::GetIsOnWater()
//{
//	return IsOnWater;
//}
//
//void GameObject::SetIsOnBridge(bool b)
//{
//	IsOnBridge = b;
//}
//
//bool GameObject::GetIsOnBridge()
//{
//	return IsOnBridge;
//
//}
/*
void GameObject::SetIsBuoyancyAdded(bool b)
{
	isBuoyancyAdded = b;
}

bool GameObject::GetIsBuoyancyAdded()
{
	return isBuoyancyAdded;
}*/