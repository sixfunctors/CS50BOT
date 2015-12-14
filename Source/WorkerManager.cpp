/*

WorkerManager.cpp

-manages worker mining
-calls for protection for workers

*/

#include "WorkerManager.h"


using namespace CS50BOT;

UnitInfo WorkerManager::nullWorker;

//Constructor 
WorkerManager::WorkerManager()
{
	//Initialize nullWorker
	nullWorker.unit = 0;
}

//Keep persistent instance (only initializes once)
WorkerManager& WorkerManager::Instance()
{
	static WorkerManager instance;
	return instance;
}

void WorkerManager::initialize()
{
	workerType = DataManager::playerRace.getWorker();
}

void WorkerManager::update()
{	
	//Worker checkup
	for (auto &current : DataManager::Workers)
	{
		// Remove the worker if it no longer exists
		if (!current.unit->exists())
		{
			//If it was on way to construct something try again
			build(current.type);
			continue;
		}

		//Ignore the worker if it cannot execute a command
		if (current.unit->isLockedDown() || current.unit->isMaelstrommed() || current.unit->isStasised() || current.unit->isLoaded())
			continue;

		if (current.job == UnitInfo::MINE)
		{
			if (current.unit->isIdle())
			{
				current.unit->gather(current.unit->getClosestUnit(BWAPI::Filter::IsMineralField));
				continue;
			}
		}

		if (current.job == UnitInfo::GAS)
		{
			if (current.unit->isIdle())
			{
				current.unit->gather(current.unit->getClosestUnit(BWAPI::Filter::IsRefinery));
				continue;
			}
		}

		if (current.job == UnitInfo::BUILD)
		{
			if (current.unit->isIdle())
			{
				if (!current.unit->build(current.type, current.unit->getTilePosition()))//If build fails
				{
					build(current.type);
				}
				current.job = UnitInfo::MINE;
				return;
			}
		}
	}
}

//Logs the request to build a building and stores its information
bool WorkerManager::build(BWAPI::UnitType building, UnitInfo* b)
{
	UnitInfo* builder;
	if (b->unit == nullWorker.unit)
	{
		builder = getWorker(UnitInfo::MINE);
	}
	else
	{
		builder = b;
	}

	if (!builder->unit) //If no builder found
	{
		BWAPI::Broodwar << "No Builder!" << building << std::endl;
		return false;
	}

	using namespace BWAPI;
	
	//get a location
	TilePosition targetBuildLocation = Broodwar->getBuildLocation(building, builder->unit->getTilePosition());
	Position a = Position(targetBuildLocation);

	//For whatever reason, you have to deal with refineries differently
	if (building.isRefinery())
	{
		return builder->unit->build(building, targetBuildLocation);
	}

	//Otherwise send a move command and store the building information
	if (!builder->unit->move(Position(a.x + building.width()/2, a.y + building.height()/2))) //Move to center of build location
	{
		return false;
	} 
	
	//else
	builder->job = UnitInfo::BUILD;
	builder->type = building;
	return true;
}

//Gets a worker with the given job
UnitInfo* WorkerManager::getWorker(UnitInfo::Job j)
{
	//Iterates through workers
	for (auto &current : DataManager::Workers)
	{
		if (current.job == j)
		{
			return &current;
		}
	} //If no worker found

	return &nullWorker; //Glorified return 0
}

//Sends three workers to mine gas
void WorkerManager::setGasProduction(BWAPI::Unit refinery)
{
	UnitInfo* worker = getWorker(UnitInfo::MINE);
	worker->job = UnitInfo::GAS;
	worker->unit->move(refinery->getPosition());

	worker = getWorker(UnitInfo::MINE);
	worker->job = UnitInfo::GAS;
	worker->unit->move(refinery->getPosition());
	
	worker = getWorker(UnitInfo::MINE);
	worker->job = UnitInfo::GAS;
	worker->unit->move(refinery->getPosition());
}
