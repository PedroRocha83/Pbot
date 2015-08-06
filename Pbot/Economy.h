#pragma once

#include <BWAPI.h>

#include "rlMain.h"
#include "Bases.h"
#include "Production.h"
#include "BuildingPlacer.h"

enum bState{planned, underConstruction, built, destroyed};

// Funciona como um espelho do estado usado no RL, contém informações extras.
struct buildingStatus
{
	int buildingID = -1;
	UnitType buildingType;
	TilePosition place =  TilePositions::Invalid;
	int builderID = -1;
	int frame = -1;
};

class Economy
{
public:

	Economy();
	~Economy();

	void onStart(Unitset us);
	void onFrame();
	void onEnd(bool isWinner);
	
	void onUnitCreate(Unit unit);
	void onUnitDestroy(Unit unit);
	void onUnitMorph(Unit unit);
	void onUnitComplete(Unit unit);

private:

	rlMain BOql;
	Bases Bases;
	Production Production;
	BuildingPlacer Bp;

	vector<buildingStatus> currentBuildings;
	
	void addBuilding(UnitType ut);
	void addBuildingCheckReq(UnitType add, UnitType req, int moreThan);

	//assimilator e bases novas.
	bool needsAssimilator();
	bool assimilatorUnderConstruction = false;

	bool mayExpand();
	bool nexusUnderConstruction = false;
};