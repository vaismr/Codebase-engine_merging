#include "LevelBase.h"

class Level1 : public LevelBase {
public:
	virtual void init(TutorialGame* game) {
	
		game->AddCharacterToWorld(Vector3(45, 2, 0));
		game->AddFloorToWorld(Vector3(0, -2, 0));

	}
};
