#include "LevelBase.h"

class Level1 : public LevelBase {
public:
	virtual void init(TutorialGame* game) {
		game->InitMixedGridWorld(10, 10, 3.5f, 3.5f);
		game->AddCharacterToWorld(Vector3(45, 2, 0));
		game->AddFloorToWorld(Vector3(0, -2, 0));

	}
};
