#include "Bases.h"

using namespace Filter;

Bases::Bases()
{
}

Bases::~Bases()
{
}

void Bases::onStart(Unitset us)
{
	for (auto u : us)
	{
		if (u->getType().isResourceDepot())
			addNexus(u->getID());

		if (u->getType().isWorker())
			addWorker(u->getID());
	}
}

void Bases::onFrame()
{
	Unit u, vespeneSource;
	map<int, int> BaseToWorkerOnVespene;

	// serviceUpkeep;
	for (auto sr : svRec)
	{
		u = Broodwar->getUnit(sr.workerID);
		if (sr.currentTask == vespene)
		{
			BaseToWorkerOnVespene[sr.NexusID]++;
		}
		if (u->isIdle())
		{
			if (sr.currentTask == task::mineral)
				u->rightClick(Broodwar->getClosestUnit(Broodwar->getUnit(sr.NexusID)->getPosition(), Filter::IsMineralField));
			else
			{
				for (auto br : bRec)
				{
					if (sr.NexusID = br.NexusID)
					{
						vespeneSource = Broodwar->getUnit(br.AssimilatorID);
						u->rightClick(vespeneSource);
					}
				}
			}
		}
	}

	// Se precisa de mais trabalhadores no vespene...
	for (auto bwv : BaseToWorkerOnVespene)
	{
		int b = bwv.first,
			t = bwv.second;
		if ((t != 0) && (t != 3))
		{
			while (t < 3)
			{
				for (auto &sr : svRec)
				{
					if ((sr.NexusID = b) && (sr.currentTask == task::mineral))
					{
						
						sr.currentTask = task::vespene;
						u = Broodwar->getUnit(sr.workerID);
						for (auto br : bRec)
						{
							if (sr.NexusID = br.NexusID)
							{
								vespeneSource = Broodwar->getUnit(br.AssimilatorID);
								u->rightClick(vespeneSource);
							}
						}
						t++;

						//Isso aqui é feio... Mas necessário para sair do loop do for sem realocar todo mundo para vespene.
						if (t == 3)
							break;
					}
				}
			}
		}
	}

	removeExcess();
	reallocateWorker();
}

void Bases::onUnitCreate(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Nexus)
		addNexus(unit->getID());

	if (unit->getType() == UnitTypes::Protoss_Probe)
		addWorker(unit->getID());
}

void Bases::onUnitDestroy(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Nexus)
		removeNexus(unit->getID());

	if (unit->getType() == UnitTypes::Protoss_Probe)
		removeWorker(unit->getID());
}

void Bases::onUnitMorph(Unit unit)
{
	bool add = true;

	if (unit->getType() == UnitTypes::Resource_Vespene_Geyser)
		for (auto& br : bRec)
			if (br.AssimilatorID == unit->getID())
				add = false;

	if (add)
		addAssimilator(unit->getID());
	else
		removeAssimilator(unit->getID());
}

bool Bases::mayExpand()
{
	bool b = true;
	int worker, mineral, maxWorker;
	for (auto br : bRec)
	{
		worker = 0;
		maxWorker = 0;

		mineral = Broodwar->getUnitsInRadius(Broodwar->getUnit(br.NexusID)->getPosition(), 10 * 48, Filter::IsMineralField).size();
		
		maxWorker += mineral*WorkerMineral;
		
		if (br.AssimilatorID != -1)
		{
			maxWorker += WorkerVespene;
		}

		for (auto sr : svRec)
		{
			if (sr.NexusID == br.NexusID)
				worker++;
		}
		if (worker < maxWorker)
			b = false;
	}
	return b;
}

// Ignora trabalhadores em vespene
void Bases::removeExcess()
{
	map<int,int> wcount;
	for (auto br : bRec)
	{
		wcount[br.NexusID] = 0;
		// Conta o total de trabalhadores
		for (auto sr : svRec)
		{
			if ((sr.currentTask == task::mineral) && (sr.NexusID == br.NexusID))
				wcount[br.NexusID]++;
		}

		// Reduz o pelo total de minerais * 2
		wcount[br.NexusID] -= (Broodwar->getUnitsInRadius(Broodwar->getUnit(br.NexusID)->getPosition(), 10 * 48,Filter::IsMineralField).size()) * WorkerMineral;


		if (wcount[br.NexusID] > 0)
		{
			for (auto sr : svRec)
			{
				if ((sr.currentTask == task::mineral) && (sr.NexusID == br.NexusID))
				{
					sr.NexusID = -1;
					wcount[br.NexusID]--;
					if (wcount[br.NexusID] == 0)
						break;
				}
			}
		}
	}
}

void Bases::reallocateWorker()
{
	int min, wcount;
	Unit worker;
	for (auto br : bRec)
	{
		min = Broodwar->getUnitsInRadius(Broodwar->getUnit(br.NexusID)->getPosition(), 480, Filter::IsMineralField).size();
		wcount = 0;

		for (auto sr : svRec)
		{
			if ((sr.NexusID == br.NexusID) && (sr.currentTask ==task::mineral))
			{
				wcount++;
			}
		}

		wcount = min - wcount;

		if (wcount > 0)
		{
			for (auto &sr : svRec)
			{
				if (sr.NexusID == -1)
				{
					sr.currentTask = task::mineral;
					sr.NexusID = br.NexusID;
					worker = Broodwar->getUnit(sr.workerID);
					worker->rightClick(Broodwar->getClosestUnit(Broodwar->getUnit(sr.NexusID)->getPosition(), Filter::IsMineralField));

					wcount--;
				}
				if (wcount == 0)
				{
					break;
				}
			}
		}
	}
}

void Bases::addWorker(int wID)
{
	serviceRecord sr;
	sr.currentTask = task::mineral;
	sr.workerID = wID;
	sr.NexusID = Broodwar->getClosestUnit(Broodwar->getUnit(wID)->getPosition(), Filter::IsResourceDepot, 320)->getID();
	svRec.push_back(sr);
}

void Bases::removeWorker(int wID)
{
	int removeThis;
	for (int i = 0; i < svRec.size(); i++)
	{
		if (svRec[i].workerID == wID)
			removeThis = i;
	}
	svRec.erase(svRec.begin() + removeThis);
}

void Bases::addNexus(int nID)
{
	baseRecord br;
	br.NexusID = nID;

	Unit u = (Broodwar->getClosestUnit(Broodwar->getUnit(nID)->getPosition(), Filter::IsRefinery, 320));
	if (u != nullptr)
		br.AssimilatorID = u->getID();

	bRec.push_back(br);
}

void Bases::removeNexus(int nID)
{
	int removeThis;
	for (int i = 0; i < bRec.size(); i++)
	{
		if (bRec[i].NexusID == nID)
			removeThis = i;
	}
	bRec.erase(bRec.begin() + removeThis);
}

void Bases::addAssimilator(int aID)
{
	Unit nexus = Broodwar->getClosestUnit(Broodwar->getUnit(aID)->getPosition(), Filter::IsResourceDepot, 320);
	int count = 0;

	if (nexus != nullptr)
		for (auto& br : bRec)
		{
			if (nexus->getID() == br.NexusID)
			{
				br.AssimilatorID = aID;
				for (auto& sr : svRec)
				{
					if ((sr.NexusID == br.NexusID) && (sr.currentTask == task::mineral) && count < 3)
					{
						count++;
						sr.currentTask = task::vespene;
						Broodwar->getUnit(sr.workerID)->rightClick(Broodwar->getUnit(aID));
					}
				}
			}
		}
}

void Bases::removeAssimilator(int aID)
{
	for (auto& br : bRec)
	{
		if (br.AssimilatorID == aID)
			br.AssimilatorID = -1;

		for (auto& sr : svRec)
		{
			if ((sr.NexusID == br.NexusID) && (sr.currentTask == task::vespene))
			{
				sr.currentTask = task::mineral;
				Broodwar->getUnit(sr.workerID)->rightClick(Broodwar->getClosestUnit(Broodwar->getUnit(sr.NexusID)->getPosition(), Filter::IsMineralField));
			}
		}
	}
}

bool Bases::needsAssimilator()
{
	Unitset us;
	for (auto br : bRec)
	{
		if (br.AssimilatorID == -1)
		{
			us = Broodwar->getUnitsInRadius(Broodwar->getUnit(br.NexusID)->getPosition(), 480, Filter::IsWorker);
			if (us.size() > 10)
				return true;
		}
	}
	return false;
}