#include "LevelBase.h"
#include  "GameTechRenderer.h"
#include  "TutorialGame.h"
class LevelTest: public LevelBase {
private:

public:
	virtual void init(TutorialGame* game) {
	
		/*game->InitMixedGridWorld(10, 10, 3.5f, 3.5f);
		game->AddGooseToWorld(Vector3(30, 2, 0));
		game->AddAppleToWorld(Vector3(35, 2, 0));*/

		// game->AddParkKeeperToWorld(Vector3(40, 2, 0));
		// game->AddCharacterToWorld(Vector3(45, 2, 0));

		/*game->AddFloorToWorld(Vector3(0, -2, 0));
		game->AddParticleToWorld(Vector3(40, 20, 0), game->GetBasicTex(), 0.5);*/

		// game->AddFloorToWorld(Vector3(0, -2, 0));
	}

	char* getName() const override
	{
		return "test_level";
	};
};
