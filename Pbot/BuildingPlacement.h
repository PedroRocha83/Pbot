#pragma once

#include <vector>
#include <BWAPI.h>

using namespace std;
using namespace BWAPI;

#define influenceMap vector<vector<float>>

class BuildingPlacement
{
public:
	
	BuildingPlacement();
	~BuildingPlacement();

	void place(UnitType ut);

private:

	influenceMap pylons, resources, buildings;
	
	float pylonInfluence(Unit pylon, TilePosition target);
	float resourceInfluence(Unit resource, TilePosition target);
	float buildingInfluence(Unit building, TilePosition target);

	TilePosition returnPylonPosition();       //Usa o mapa de prédios.
	TilePosition returnNexusPosition();       //Usa o mapa de recursos.
	TilePosition returnAssimilatorPosition(); //Não requer mapa de influência.
	TilePosition returnBuildingPosition();    //Usa o mapa de pylons.

	Unit getNearestFreeWorker(TilePosition selectedTile);
	void build(UnitType ut, TilePosition selectedTile, Unit worker);

};

