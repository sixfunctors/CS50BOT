/*

CombatManager.cpp

- Controls attacking units

*/

#include "CombatManager.h"

using namespace CS50BOT;

CombatManager::CombatManager()
{

}

CombatManager& CombatManager::Instance()
{
	static CombatManager instance;
	return instance;
}

void CombatManager::initialize()
{
	//Original paradigm
	army.job = UnitInfo::DEFEND;
	
	//Your main base location
	main = BWAPI::Point<int, BWAPI::POSITION_SCALE >::Point(DataManager::selfStart);
	
	//Stores the spawn locations
	for (auto &i : DataManager::startingLocations)
	{
		//Don't add your own spawn location to the list
		if (i == DataManager::selfStart)
		{
			continue;
		}
		
		//Insert into the proper place in spawns (sorts by closest to furthest from our starting location)
		if (spawns.empty())
		{
			spawns.push_back(BWAPI::Point<int, BWAPI::POSITION_SCALE >::Point(i));

		}
		for (std::list<BWAPI::Position>::const_iterator &spawn = spawns.begin(); spawn != spawns.end(); ++spawn)
		{
			//Insert before the first spawn in the list which is further away than it
			if (i.getApproxDistance(DataManager::selfStart) <= spawn->getApproxDistance(BWAPI::Point<int, BWAPI::POSITION_SCALE>::Point(DataManager::selfStart)))
			{
				spawns.insert(spawn, BWAPI::Point<int, BWAPI::POSITION_SCALE >::Point(i));
			}
		}
	}
}

void CombatManager::update()
{
	//Print to Top Left
	if (army.job == UnitInfo::DEFEND)
	{
		BWAPI::Broodwar->registerEvent([](BWAPI::Game*){
			BWAPI::Broodwar->drawTextScreen(1, 1, "Current Army Paradigm: Defend");
		}, nullptr, BWAPI::Broodwar->getLatencyFrames(), 0);
	}
	else if (army.job == UnitInfo::ATTACK)
	{
		BWAPI::Position c = currentTarget;
		BWAPI::Broodwar->registerEvent([c](BWAPI::Game*){
			BWAPI::Broodwar->drawTextScreen(1, 1, "Current Army Paradigm: Attack \nCurrent Target Location: (%d, %d)", c.x, c.y);
		}, nullptr, BWAPI::Broodwar->getLatencyFrames(), 0);
	}

	//Iterate through army units
	if (DataManager::Army.empty())
	{
		return;
	}
	for (auto &current : DataManager::Army)
	{		
		//Ignore the worker if it cannot execute a command
		if (current.unit->isLockedDown() || current.unit->isMaelstrommed() || current.unit->isStasised() || current.unit->isLoaded())
			continue;
		
		// army paradigm
		if (army.job == UnitInfo::DEFEND)
		{

		}
		else if (army.job == UnitInfo::ATTACK)
		{
			if (current.unit->isIdle()) //Idle units attack towards currentTarget
			{
				current.unit->attack(currentTarget);
			}

			if (spawns.empty()) //If you have exhausted all spawn points
			{
				continue;
			}
			
			//If you are close enough to the target and there is no enemy visible
			if ((current.unit->getPosition().getApproxDistance(currentTarget) < (SENSE_RANGE)) &&
				(current.unit->getUnitsInRadius((SENSE_RANGE), BWAPI::Filter::IsEnemy).empty()))
			{
				//Move on to the next target
				currentTarget = spawns.front(); 
				spawns.pop_front();

				//Send all existing army units to the new target
				for (auto &unit : DataManager::Army)
				{
					unit.unit->attack(currentTarget);
				}
			}
		}
	}
}

void CombatManager::attack()
{
	army.job = UnitInfo::ATTACK;
	currentTarget = spawns.front();
	BWAPI::Broodwar << "The current target is " << currentTarget << std::endl;
	spawns.pop_front();
}