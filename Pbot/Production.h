#pragma once

#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

class Production
{
public:
	Production();
	~Production();

	void onStart();
	void onFrame();
	void onEnd();

private:

	vector<int> resources;
	vector<Unit> buildings;

	vector<int> checkAvailableResources();
	vector<int> cost(UnitType unit);
	vector<int> deductCost(vector<int> a, vector<int> b);
	UnitType makeBestChoice(Unit building);
	void executeBestChoice(Unit building, UnitType choice);
};
