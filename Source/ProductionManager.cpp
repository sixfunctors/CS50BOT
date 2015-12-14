/*

ProductionManager.cpp

-Keeps Queues producing units

*/

#include "ProductionManager.h"

#define WORKER_MAX 22

using namespace CS50BOT;

//Constructor
ProductionManager::ProductionManager()
{
	
}

//Keep persistent instance (only initializes once)
ProductionManager& ProductionManager::Instance()
{
	static ProductionManager instance;
	return instance;
}

void ProductionManager::initialize()
{
	//Stores the correct worker to produce inside UnitComp (at the beginning of the game)
	std::array<BWAPI::UnitType, COMP_MAX> units = { DataManager::playerRace.getWorker() };
	setUnitProduction(DataManager::playerRace.getCenter(), units);
}


void ProductionManager::update()
{
	//Iterate through production buildings
	for (auto &current : DataManager::Producers)
	{
		//Those buildings which should be producing
		if (current.job == UnitInfo::PRODUCE)
		{
			//If you have built enough workers
			if ((current.type == DataManager::playerRace.getCenter()) && (DataManager::Workers.size() > WORKER_MAX))
			{
				current.job = UnitInfo::IDLE;
			}
			else if (current.unit->isIdle())
			{
				produce(&current);
			}
		}
	}//End iteration
}


//Produces one of the designated unit from a building
bool ProductionManager::produce(UnitInfo* building)
{
	if (!UnitComp.count(building->type)) //If no unit assigned to that building
	{
		BWAPI::Broodwar << "ERROR! ERROR! No unit type assigned to that building type!!" << std::endl;
		return false;
	}

	//The unit to be produced
	std::array<BWAPI::UnitType, COMP_MAX> units = UnitComp[building->type];
	
	//Iterate through the units for that building
	for (const BWAPI::UnitType &unit : units)
	{
		if (!unit)
		{
			continue;
		}

		//If you can't build that unit yet (tech failure)
		if (!DataManager::Instance().techCheck(unit))
		{
			continue;
		}
		

		if (!building->unit->canTrain(unit))
		{
			continue;
		}

		//If you dont have enough money
		if (!DataManager::Instance().purchaseRequest(unit))
		{
			continue; 
		}

		//Builds one of whatever unit that building should be producing
		building->unit->train(unit);
		return true;
	}
	return false;
}

//Informs ProductionManager what unit should be produced from what building
void ProductionManager::setUnitProduction(BWAPI::UnitType building, std::array<BWAPI::UnitType, COMP_MAX> units)
{
	UnitComp[building] = units;
}