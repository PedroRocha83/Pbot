#include "Production.h"

Production::Production()
{
}


Production::~Production()
{
}

void Production::operate(Unit building)
{
	UnitType ut = building->getType();

	if ((ut == UnitTypes::Protoss_Nexus) 
		&& (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) < 60)	
		&& (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Probe) / Broodwar->self()->allUnitCount(UnitTypes::Protoss_Nexus) < 20))
		building->train(UnitTypes::Protoss_Probe);

	if (ut == UnitTypes::Protoss_Gateway)
		building->train(UnitTypes::Protoss_Zealot);

	if (ut == UnitTypes::Protoss_Forge)
	{
		if (!(building->upgrade(UpgradeTypes::Protoss_Ground_Weapons)))
		{
			if (Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Ground_Armor) <= 
				(Broodwar->self()->getUpgradeLevel(UpgradeTypes::Protoss_Plasma_Shields)))
			{
				building->upgrade(UpgradeTypes::Protoss_Ground_Armor);
			}
			else
			{
				building->upgrade(UpgradeTypes::Protoss_Plasma_Shields);
			}
		}
	}

	if (ut == UnitTypes::Protoss_Cybernetics_Core)
	{
		if (!(building->upgrade(UpgradeTypes::Protoss_Air_Weapons)))
			building->upgrade(UpgradeTypes::Protoss_Air_Armor);
	}

	if (ut == UnitTypes::Protoss_Robotics_Facility)
	{
		if (Broodwar->self()->allUnitCount(UnitTypes::Protoss_Observer) < 3)
			building->train(UnitTypes::Protoss_Observer);

		building->train(UnitTypes::Protoss_Reaver);
	}

	if (ut == UnitTypes::Protoss_Robotics_Support_Bay)
	{
		if (!(building->upgrade(UpgradeTypes::Scarab_Damage)))
			building->upgrade(UpgradeTypes::Reaver_Capacity);
	}

	if (ut == UnitTypes::Protoss_Observatory)
	{
		if (!(building->upgrade(UpgradeTypes::Sensor_Array)))
		{
			building->upgrade(UpgradeTypes::Gravitic_Boosters);
		}
	}

	if (ut == UnitTypes::Protoss_Stargate)
	{
		if (!building->train(UnitTypes::Protoss_Carrier))
			building->train(UnitTypes::Protoss_Corsair);
	}

	if (ut == UnitTypes::Protoss_Fleet_Beacon)
		building->upgrade(UpgradeTypes::Carrier_Capacity);		

	if (ut == UnitTypes::Protoss_Arbiter_Tribunal)
	{
		//nada mesmo!
	}
	
	if (ut == UnitTypes::Protoss_Citadel_of_Adun)
		building->upgrade(UpgradeTypes::Leg_Enhancements);

	if (ut == UnitTypes::Protoss_Templar_Archives)
	{
		//nada mesmo!
	}
}