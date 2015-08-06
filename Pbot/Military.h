#pragma once

#include "Targets.h"
#include "Squads.h"

class Military
{
public:
	Military();
	~Military();

	void onStart();
	void onFrame();
	void onEnd();
	
private:
	Targets Targets;
	Squads Squads;
};

