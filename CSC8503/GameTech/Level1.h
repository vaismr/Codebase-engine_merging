#include "LevelBase.h"

class Level1 : public LevelBase {
public:
	virtual void init(TutorialGame* game) {
	
		game->AddCharacterToWorld(Vector3(45, 2, 0));
		game->AddFloorToWorld(Vector3(300, -2, 0));
	}

	char* getName() const override
	{
		return "level1";
	};
};
