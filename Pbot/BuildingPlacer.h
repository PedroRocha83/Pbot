#pragma once

#include <BWAPI.h>

using namespace BWAPI;
using namespace std;

class BuildingPlacer
{
public:
	BuildingPlacer();
	~BuildingPlacer();

	TilePosition place(UnitType ut);
	
	void onUnitCreate(Unit unit);
	void onUnitDestroy(Unit unit);

private:

	// A cada vez que um pylon � usado no sistema, some 1 ao segundo valor.
	// O primeiro valor � ID do pylon.
	map<int, int> pylonUses;

	TilePosition getNexusTile();
	TilePosition getAssimilatorTile();
	TilePosition getPylonTile();
	TilePosition getBuildingTile(UnitType ut);
};

