#pragma once

#include <BWAPI.h>

#include "rlMain.h"
#include "BuildingPlacement.h"
#include "Bases.h"
#include "Production.h"

class Economy
{
public:
	Economy();
	~Economy();

	void onStart();
	void onFrame();
	void onEnd();

private:

	rlMain BOql;
	BuildingPlacement BP;
	Bases Bases;
	Production Production;

	BWAPI::Unitset buildings, workers;
	vector<int> resources;
};

