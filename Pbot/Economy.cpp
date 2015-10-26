#include "Economy.h"

Economy::Economy()
{
}

Economy::~Economy()
{
}

void Economy::onStart(Unitset us)
{
	BOql.onStart();
	//Bases.onStart(us);

	buildingStatus bs;
	for (auto u : us)
	{
		if (u->getType().isResourceDepot())
		{
			bs.buildingID = u->getID();
			bs.buildingType = Broodwar->self()->getRace().getCenter();
			bs.place = u->getTilePosition();
			bs.builderID = -1;
			currentBuildings.push_back(bs);
		}
	}
}

void Economy::onFrame()
{
	int currPop = Broodwar->self()->supplyUsed(),
		totalPop = Broodwar->self()->supplyTotal(),
		popGap = 24,
		futurePop = 0;

	// Todo o loop da base fica contido aqui.	
	Bases.onFrame();

	// Mantém o estado atual ou busca o próximo passo na "build order".
	bool nextStep = true;

	// O prédio é para a produção, o trabalhador é para construção.
	Unit building = nullptr,
		worker = nullptr;

	/*	Encontra o estado atual do prédio sendo observado.
		Se o prédio está funcionando e livre, a produção tenta fazer uso dele.
		Se todos os prédios estão feitos ou em construção, o próximo passo é dado.*/
	for (auto &bs : currentBuildings)
	{
			// Limitar a construção de pylons
		if ((bs.buildingType == Broodwar->self()->getRace().getSupplyProvider())
			&& (bs.buildingID == -1 || Broodwar->getUnit(bs.buildingID)->isBeingConstructed()))
		{
			futurePop += bs.buildingType.supplyProvided();
		}

		// Não foi colocado em jogo ainda
		if (bs.buildingID == -1)
		{
			// Restringir o BOql caso...
			if ((bs.buildingType == UnitTypes::Protoss_Gateway)
				|| (bs.buildingType == UnitTypes::Protoss_Forge)
				|| (bs.buildingType == UnitTypes::Protoss_Cybernetics_Core)
				|| (bs.buildingType == UnitTypes::Protoss_Stargate)
				|| (bs.buildingType == UnitTypes::Protoss_Robotics_Facility))
			{
				nextStep = false;
			}
			// Está demorando muito para entrar em jogo
			if (bs.frame < Broodwar->getFrameCount() + 120)
			{
				// Pode ser feito
				if (Broodwar->canMake(bs.buildingType))
				{
					// Precisa de um lugar válido
					if ((bs.place == TilePositions::Invalid) || !(Broodwar->canBuildHere(bs.place, bs.buildingType)))
						bs.place = Bp.place(bs.buildingType);

					worker = Broodwar->getClosestUnit((Position)bs.place, Filter::IsWorker);
					// Se a função acima retornou um trabalhador, mande-o construir e registre o momento.
					if (worker != NULL)
					{
						bs.builderID = worker->getID();
						bs.frame = Broodwar->getFrameCount();
						worker->build(bs.buildingType, bs.place);
					}
				}
			}
			// Ainda está no tempo certo!
			else
			{
				worker = Broodwar->getUnit(bs.builderID);
				if (worker->exists())
				{
					bs.frame = Broodwar->getFrameCount();
					worker->build(bs.buildingType, bs.place);
				}
				// O trabalhador foi morto durante a execução da ordem de construção
				else
				{
					bs.builderID = -1;
					bs.frame = -1;
					bs.place = TilePositions::Invalid;
				}
			}
		}
		else
		{
			building = Broodwar->getUnit(bs.buildingID);
			//Production!
			if ((building->exists()) && (building->isIdle()))
			{
				Production.operate(building);
			}
			else
			{
				bs.buildingID = -1;
				bs.frame = -1;
				bs.builderID = -1;
			}
		}
	}

	// Se for a hora de expandir, BOQl não deve ser chamado.
	if (mayExpand())
	{
		addBuilding(UnitTypes::Protoss_Nexus);
	}

	// Chama o BOQl...
	if (nextStep)
		addBuilding(BOql.chooseAction());

	// Verifica a necessidade de adicionar outros prédios que diferentes do NEXUS!
	// Pylons
	if (currPop + popGap > totalPop + futurePop)
		addBuilding(UnitTypes::Protoss_Pylon);

	// Assimilators
	if (needsAssimilator())
		addBuilding(UnitTypes::Protoss_Assimilator);

	// Citadel of Adun
	addBuildingCheckReq(UnitTypes::Protoss_Citadel_of_Adun, UnitTypes::Protoss_Gateway, 2);

	// Robotics Facility
	addBuildingCheckReq(UnitTypes::Protoss_Robotics_Support_Bay, UnitTypes::Protoss_Robotics_Facility, 0);

	// Observatory
	addBuildingCheckReq(UnitTypes::Protoss_Observatory, UnitTypes::Protoss_Robotics_Facility, 0);

	// FleetBeacon
	addBuildingCheckReq(UnitTypes::Protoss_Fleet_Beacon, UnitTypes::Protoss_Stargate, 1);

	// Templar Archive
	addBuildingCheckReq(UnitTypes::Protoss_Templar_Archives, UnitTypes::Protoss_Citadel_of_Adun, 0);

	// Arbiter Tribunal
	addBuildingCheckReq(UnitTypes::Protoss_Arbiter_Tribunal, UnitTypes::Protoss_Fleet_Beacon, 0);
}

void Economy::onEnd(bool isWinner)
{
	BOql.onEnd(isWinner);
}

void Economy::onUnitCreate(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Probe)
		Bases.onUnitCreate(unit);

	if (unit->getType() == UnitTypes::Protoss_Nexus)
	{
		Bases.onUnitCreate(unit);
		nexusUnderConstruction = false;
	}

	for (auto &cb : currentBuildings)
	{
		if ((cb.buildingType == unit->getType()) && (cb.buildingID == -1))
		{
			cb.buildingID = unit->getID();
			break;
		}
	}
}

void Economy::onUnitDestroy(Unit unit)
{
	if (unit->getType() == UnitTypes::Protoss_Probe || UnitTypes::Protoss_Nexus)
		Bases.onUnitDestroy(unit);

	if (unit->getType() == UnitTypes::Protoss_Pylon)
		Bp.onUnitDestroy(unit);
}

void Economy::onUnitMorph(Unit unit)
{
	if (unit->getType().isRefinery())
		assimilatorUnderConstruction = false;
	
	Bases.onUnitMorph(unit);
}

void Economy::onUnitComplete(Unit unit)
{}

bool Economy::needsAssimilator()
{
	return (Bases.needsAssimilator() && !assimilatorUnderConstruction);
}

void Economy::addBuilding(UnitType ut)
{
	buildingStatus bs;
	if (ut != UnitTypes::None){
		bs.buildingType = ut;
		currentBuildings.push_back(bs);
	}
}

void Economy::addBuildingCheckReq(UnitType add, UnitType req, int moreThan)
{
	if (Broodwar->self()->allUnitCount(add) == 0
		&& Broodwar->self()->deadUnitCount(add) == 0
		&& Broodwar->self()->allUnitCount(req) > moreThan)
	{
		buildingStatus bs;
		bs.buildingType = add;
		currentBuildings.push_back(bs);
	}
}

bool Economy::mayExpand()
{
	return (Bases.mayExpand() && !nexusUnderConstruction);
}