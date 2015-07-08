#pragma once

#include <BWAPI.h>
#include <queue>

using namespace BWAPI;
using namespace Filter;
using namespace std;

struct baseRecord
{
	Unit nexus, assimilator;
	bool hasGeyser;
	int crystals, mineralWorkers, vespeneWorkers;

	baseRecord(Unit nex)
	{
		nexus = nex;
		assimilator = NULL;
		hasGeyser = (!Broodwar->getUnitsInRadius(nex->getPosition(), 32 * 10, (IsResourceContainer&&!IsMineralField)).empty());
		crystals = Broodwar->getUnitsInRadius(nex->getPosition(), 32 * 10, IsMineralField).size();
		mineralWorkers = 0;
		vespeneWorkers = 0;
	}
};

enum Task{idle, mineral, vespene, building};

struct serviceRecord
{
	int nexus_ID;
	Unit worker;
	Task task;
};

using BaseTable = vector<baseRecord>;
using ServiceTable = map<int,serviceRecord>; //O primeiro campo é o workerID!

class Bases
{
public:

	Bases();
	~Bases();

	void onStart();
	void onFrame();
	void onEnd();

private:

	const int workerCountLimit = 75;
	const int workerPerMineral = 2;
	const int workerPerVespene = 3;

	BaseTable baseTable;
	ServiceTable serviceTable;

	int currentTotalWorkerCount;
	queue<Unit> freeWorkers;

	int optimalWorkerCount(baseRecord bt);
	int currentWorkerCount(baseRecord bt);
	
	void baseRecordUpkeep();
	void baseTableUpkeep();
	void serviceTableUpkeep();

	void removeExcess();
	void reallocateWorkers();
	void trainWorker();

	void moveWorkersToVespene();
};

