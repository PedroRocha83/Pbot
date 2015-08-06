#include "BuildingPlacer.h"


BuildingPlacer::BuildingPlacer()
{
}


BuildingPlacer::~BuildingPlacer()
{
}

void BuildingPlacer::onUnitCreate(Unit unit)
{
	pylonUses.insert(make_pair(unit->getID(), 0));
}

void BuildingPlacer::onUnitDestroy(Unit unit)
{
	pylonUses.erase(unit->getID());
}

TilePosition BuildingPlacer::place(UnitType ut)
{
	if (ut == UnitTypes::Protoss_Nexus)
		return getNexusTile();

	if (ut == UnitTypes::Protoss_Assimilator)
		return getAssimilatorTile();

	if (ut == UnitTypes::Protoss_Pylon)
		return getPylonTile();

	return getBuildingTile(ut);
}

TilePosition BuildingPlacer::getNexusTile()
{
	int distance = 0;
	TilePosition tp,
		startPosition = Broodwar->self()->getStartLocation();

	Unitset vespeneGeyser = Broodwar->getStaticGeysers();

	for (auto u : vespeneGeyser)
	{
		if (Broodwar->getUnitsInRadius(u->getPosition(), 320, Filter::IsResourceDepot).size() == 0)
		{
			if ((u->getPlayer() != Broodwar->self()) 
				|| (distance < startPosition.getApproxDistance(u->getTilePosition())) 
				|| (distance = 0))
			{
				tp = u->getTilePosition();
				distance = startPosition.getApproxDistance(tp);
			}
		}
	}
	return Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Nexus, tp);
}

TilePosition BuildingPlacer::getAssimilatorTile()
{
	Unitset units = Broodwar->getAllUnits();
	
	for (auto u : units)
	{
		if (u->getType().isResourceDepot())
		{
			if (Broodwar->getClosestUnit(u->getPosition(), Filter::IsRefinery, 20) == nullptr)
				return Broodwar->getBuildLocation(BWAPI::UnitTypes::Protoss_Assimilator, u->getTilePosition());
		}
	}
}

TilePosition BuildingPlacer::getPylonTile()
{
	TilePosition tp;

	if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Pylon) == 0)
		return Broodwar->getBuildLocation(UnitTypes::Protoss_Pylon, Broodwar->self()->getStartLocation(),10);
	else
		for (auto pu : pylonUses)
		{
			tp = Broodwar->getUnit(pu.first)->getTilePosition();
			tp = Broodwar->getBuildLocation(UnitTypes::Protoss_Pylon, tp);
			if (tp.isValid())
				return tp;
		}
}

TilePosition BuildingPlacer::getBuildingTile(UnitType ut)
{
	TilePosition tp;
	for (auto & pu : pylonUses)
	{
		tp = Broodwar->getUnit(pu.first)->getTilePosition();
		tp = Broodwar->getBuildLocation(ut, tp, 5);
		pu.second++;
		if (tp.isValid())
			return tp;
	}
}