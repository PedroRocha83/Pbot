#pragma once

#include <BWAPI.h>
#include <vector>

using namespace BWAPI;
using namespace std;

struct baseTuple
{
	int id;
	Unit nexus, assimilator;
	int mineral, vespene;
};

struct serviceTuple
{
	int baseId;
	Unit worker;
	string task;
};

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
	const float workerPerMineral = 2.5;
	const float workerPerVespene = 3;

	int BaseCounter; //Conta o número de bases, é usado para gerar a ID da tupla de bases.

	vector<baseTuple> BaseTable;
	vector<serviceTuple> ServiceTable;

	void trainWorker();
	UnitType requestPylon();
	UnitType requestNexus();
	UnitType requestAssimilator();
	void allocateWorker();          // Emprega um trabalho na base na qual já está alocado.
	void redistributeWorker();      // Move trabalhadores entre as bases, retirando de bases super lotadas ou da base#0 para bases que precisem de trabalhadores.

};

