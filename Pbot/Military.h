#pragma once

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
	Squads Squads;
};

