#pragma once

#include "TutorialGame.h"

class LevelBase {
public:
	virtual void init(TutorialGame* game) = 0;
};
