#pragma once

#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace Filter;
using namespace std;

#define WorkerMineral 2;
#define WorkerVespene 3;

enum task{ mineral, vespene };

struct serviceRecord
{
	int workerID = -1, NexusID =-1;
	task currentTask;
};

struct baseRecord
{
	int NexusID = -1, AssimilatorID =-1;
};

class Bases
{
public:

	Bases();
	~Bases();

	void onStart(Unitset us);
	void onFrame();

	void onUnitCreate(Unit unit);
	void onUnitDestroy(Unit unit);
	void onUnitMorph(Unit unit);

	// Nexus
	bool mayExpand();
	
	// Assimilator
	bool needsAssimilator();

private:

	vector<serviceRecord> svRec;
	vector<baseRecord> bRec;

	void addNexus(int nID);
	void removeNexus(int nID);

	void addAssimilator(int aID);
	void removeAssimilator(int aID);

	void addWorker(int wID);
	void removeWorker(int wID);

	// Retira trabalhadores a medida que o mineral acaba.
	void removeExcess();
	// Realoca trabalhadores para outras bases.
	void reallocateWorker();
	
};