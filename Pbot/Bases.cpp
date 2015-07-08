#include "Bases.h"

using namespace Filter;

Bases::Bases()
{
}


Bases::~Bases()
{
}

void Bases::onFrame()
{
	baseTableUpkeep();
	serviceTableUpkeep();
}

void Bases::baseTableUpkeep()
{
	int offset;
	for (auto &br : baseTable)
	{
		baseRecordUpkeep();
		offset = optimalWorkerCount(br) - currentWorkerCount(br);
		if (offset > 0)
			removeExcess();
		else if (offset < 0)
		{
			while ((freeWorkers.size() != 0) && (offset != 0))
			{
				reallocateWorkers(); //remove um freeWorker;
				offset++;
			}
		}
		if (offset != 0)
			trainWorker();
	}
}

void Bases::serviceTableUpkeep()
{
	map<int, int> baseVespeneWorker;
	for (auto &sr : serviceTable)
	{
		if ((sr.second.task == Task::idle) || (sr.second.worker->isIdle()))
		{
			sr.second.task = Task::mineral;
			sr.second.worker->rightClick(baseTable[sr.second.nexus_ID].nexus->getClosestUnit(IsMineralField, 32 * 10));
		}
		else if (sr.second.task == Task::vespene)
			baseVespeneWorker[sr.second.nexus_ID]++;
	}
	for (auto &bvw : baseVespeneWorker)
	{
		if ((bvw.second < 3) && (baseTable[bvw.first].assimilator->exists()))
		{
			moveWorkersToVespene();
		}
	}
}