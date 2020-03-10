#include "Lineup_Constraint.h"
using namespace NCL::CSC8503;

bool Lineup_Constraints::HasAdded(GameObject* obj) {
	vector<GameObject*>::iterator i;
	i = find(line->begin(), line->end(), obj);
	if (i != line->end())
		return true;
	else
		return false;
}

bool Lineup_Constraints::ifTouched(const GameObject* objA, const GameObject* objB) {
	float r = 3.0f;
	Vector3 delta = objA->GetConstTransform().GetWorldPosition() - objB->GetConstTransform().GetWorldPosition();
	float deltalengh = delta.Length();
	if (deltalengh < r)
		return true;
	else
		return false;
}

void Lineup_Constraints::PickUpItems(GameObject* obj0, GameObject* obj, Lineup_Constraints* constraint) {
	if (ifTouched(obj0, obj) && HasAdded(obj) && obj->GetID() != 0) {
		obj->GetTransform().SetWorldPosition(obj0->GetTransform().GetWorldPosition() - Vector3(0, 0, 4));
		constraint->AddObject(obj);
	}
}