#include "GameWorld.h"
#include "GameObject.h"
#include "../GameTech/ball.h"
#include "Constraint.h"
#include"Buoyancy.h"
#include "CollisionDetection.h"
#include "../../Common/Camera.h"
#include <algorithm>

using namespace NCL;
using namespace NCL::CSC8503;

GameWorld::GameWorld()	{
	mainCamera = new Camera();
	
	quadTree = nullptr;

	shuffleConstraints	= false;
	shuffleObjects		= false;
}

GameWorld::~GameWorld()	{
}

void GameWorld::Clear() {
	gameObjects.clear();
	constraints.clear();
	//buoyancies.clear();
}

void GameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	for (auto& i : constraints) {
		delete i;
	}
/*	for (auto& i : buoyancies)
	{
		delete i;
	}*/
	Clear();
}

void GameWorld::AddGameObject(GameObject* o) {
	gameObjects.emplace_back(o);
}

void GameWorld::RemoveGameObject(GameObject* o) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
}

void GameWorld::GetObjectIterators(
	GameObjectIterator& first,
	GameObjectIterator& last) const {

	first	= gameObjects.begin();
	last	= gameObjects.end();
}

void GameWorld::OperateOnContents(GameObjectFunc f) {
	for (GameObject* g : gameObjects) {
		f(g);
	}
}

void GameWorld::UpdateWorld(float dt) {
	UpdateTransforms();
	UpdateConstraint();

	if (shuffleObjects) {
		std::random_shuffle(gameObjects.begin(), gameObjects.end());
	}

	if (shuffleConstraints) {
		std::random_shuffle(constraints.begin(), constraints.end());
	}
}

void GameWorld::UpdateTransforms() {
	for (auto& i : gameObjects) {
		i->GetTransform().UpdateMatrices();
	}
}
void GameWorld::UpdateConstraint() {
	/*for (auto& i : gameObjects)
	{
		std::cout << i->GetIsBuoyancyAdded() << " " << i->GetIsOnWater() << std::endl;
		if (i->GetCollisitionType() == CollisionType::DEFAULT && !i->GetIsBuoyancyAdded() && i->GetIsOnWater())
		{
			Buoyancy* buoyancy = new Buoyancy(i);
			i->SetIsBuoyancyAdded(true);
			AddBuoyancy(buoyancy);
		}
		if (i->GetCollisitionType() == CollisionType::DEFAULT && i->GetIsBuoyancyAdded() && !i->GetIsOnWater())
		{

			ClearBuoyancies();
			i->SetIsBuoyancyAdded(false);
		}
	
	}*/
}

void GameWorld::UpdateQuadTree() {
	delete quadTree;

	//quadTree = new QuadTree<GameObject*>(Vector2(512, 512), 6);

	//for (auto& i : gameObjects) {
	//	quadTree->Insert(i);
	//}
}

bool GameWorld::Raycast(Ray& r, RayCollision& closestCollision, bool closestObject) const {
	//The simplest raycast just goes through each object and sees if there's a collision
	RayCollision collision;

	for (auto& i : gameObjects) {
		if (!i->GetBoundingVolume()) { //objects might not be collideable etc...
			continue;
		}
		RayCollision thisCollision;
		if (CollisionDetection::RayIntersection(r, *i, thisCollision)) {
				
			if (!closestObject) {	
				closestCollision		= collision;
				closestCollision.node = i;
				return true;
			}
			else {
				if (thisCollision.rayDistance < collision.rayDistance) {
					thisCollision.node = i;
					collision = thisCollision;
				}
			}
		}
	}
	if (collision.node) {
		closestCollision		= collision;
		closestCollision.node	= collision.node;
		return true;
	}
	return false;
}


/*
Constraint Tutorial Stuff
*/

void GameWorld::AddConstraint(Constraint* c) {
	constraints.emplace_back(c);
}

void GameWorld::RemoveConstraint(Constraint* c) {
	std::remove(constraints.begin(), constraints.end(), c);
}

bool GameWorld::GetConstraintIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const {
	if (constraints.empty())
	{
		return false;
	}
	first	= constraints.begin();
	last	= constraints.end();
	return true;
}
void GameWorld::AddBuoyancy(Constraint* c)
{
	buoyancies.emplace_back(c);
}
void GameWorld::ClearBuoyancies()
{
	//std::vector<Constraint*>::const_iterator it;
	//it = std::find(buoyancies.begin(), buoyancies.end(), c);
	//if (it != buoyancies.end())
	//{
	//	buoyancies.erase(it);
	//}
	//buoyancies.erase(std::remove(buoyancies.begin(), buoyancies.end(), c));
	std::vector<Constraint*>::iterator it = buoyancies.begin();
	for (; it != buoyancies.end();)
	{
		it = buoyancies.erase(it);
	}
}

bool GameWorld::GetBuoyancyIterators(
	std::vector<Constraint*>::const_iterator& first,
	std::vector<Constraint*>::const_iterator& last) const
{
	if (buoyancies.empty())
	{
		return false;
	}
	first = buoyancies.begin();
	last = buoyancies.end();
	return true;
}
