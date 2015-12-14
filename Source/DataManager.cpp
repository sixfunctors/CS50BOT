/*

DataManager.cpp

-Delegates commands
-Centralizes and organizes game data for access by other modules

*/

#include "DataManager.h"

#define ATTACK_SUPPLY 200
#define UNDERSPENDING 1000

//The starting nexus onComplete() gets called on frame 0 - causes it to be double-counted
#define STARTING_SUPPLY -18;

using namespace CS50BOT;

//Unit data
UnitList DataManager::Army;
UnitList DataManager::Workers;
UnitList DataManager::Producers;
UnitList DataManager::Researchers;

//Start of game data
BWAPI::Race DataManager::playerRace;
BWAPI::Race DataManager::enemyRace;
BWAPI::UnitType DataManager::supplyProvider;
BWAPI::TilePosition::list DataManager::startingLocations;
BWAPI::TilePosition DataManager::selfStart;


//Constructor
DataManager::DataManager()
{
	
}

//Keep persistent instance (only initializes once)
DataManager& DataManager::Instance()
{
	static DataManager instance;
	return instance;
}

//Takes starting information from game to initialize DataManager
void DataManager::initialize(BWAPI::Player self, BWAPI::Player e, BWAPI::Unitset units, BWAPI::TilePosition::list starts)
{
	//Data initialization
	player = self;
	enemy = e;

	playerRace = player->getRace();
	enemyRace = enemy->getRace();

	startingLocations = starts;
	selfStart = self->getStartLocation();

	supplyProvider = playerRace.getSupplyProvider();

	oldMineralsTotal = 0;
	oldGasTotal = 0;
	miningDataDelay = 0;

	usedSupply = 0;
	totalSupply = STARTING_SUPPLY;

	frames = 0;

	attackTrigger = false;

	//Initialize the DataManager for starting workers/buildings
	for (auto &unit : units)
	{
		add(unit); 
		usedSupply += unit->getType().supplyRequired();
	}

	BWAPI::Broodwar << "DataManager Initialized!" << std::endl;
	ProductionManager::Instance().initialize();
	BuildingManager::Instance().initialize();
	CombatManager::Instance().initialize();
}

//Frame update
void DataManager::update(DataObject frameData)
{
	//Store data on game state 
	if (miningDataDelay == 0)
	{
		mineralCheckbook += frameData.mineralsTotal - oldMineralsTotal;
		gasCheckbook += frameData.gasTotal - oldGasTotal;
		oldMineralsTotal = frameData.mineralsTotal;
		oldGasTotal = frameData.gasTotal;
		miningDataDelay = MININGDATADELAY;
	}
	else
	{
		miningDataDelay--;
	}

	//Pass it down
	WorkerManager::Instance().update();
	ProductionManager::Instance().update();
	BuildingManager::Instance().update(usedSupply, totalSupply);
	CombatManager::Instance().update();
	
	//For some reason frames internally run twice as fast
	frames += 2;

	/* Debugging Data 
	int x = mineralCheckbook;
	int y = gasCheckbook;
	int z = player->minerals();
	int a = usedSupply;
	int b = totalSupply;
	int c = player->supplyUsed();
	BWAPI::Broodwar->registerEvent(
		[x, y, z, a, b, c](BWAPI::Game*){ 
			BWAPI::Broodwar->drawTextScreen(1, 14, "Min: %d  Gas: %d \nSupply: %d/%d \nMineral Error: %d \nSupply Error: %d", x, y, a, b, z-x, c-a); 
		},
		nullptr,
		BWAPI::Broodwar->getLatencyFrames(),
		0);
	*/
	

	//Attack trigger
	if (((frames > ATTACK_FRAMES) || (usedSupply > ATTACK_SUPPLY)) && (!attackTrigger))
	{
		CombatManager::Instance().attack();
		attackTrigger = true;
	}

	//Excess money trigger
	if (mineralCheckbook > UNDERSPENDING)
	{
		BuildingManager::Instance().underspending();
	}
}


//Inserts the unit's information into the DataManager
void DataManager::add(BWAPI::Unit unit)
{
	//Build a UnitInfo type for unit
	UnitInfo info;
	info.unit = unit;
	info.type = unit->getType();
	info.delay = 0;

	//If supply provider
	if (info.type.supplyProvided() > 0)
	{
		//Keep supply tabs
		totalSupply += info.type.supplyProvided();
	}

	//If gas refinery, send workers
	if (info.type.isRefinery())
	{
		WorkerManager::Instance().setGasProduction(unit);
	}

	//Stores the unit's data
	if (info.type.isWorker()) //If worker
	{
		info.job = UnitInfo::MINE;
		
		//Add it to workers
		Workers.push_back(info);
		return;
	}
	else if (!info.type.isBuilding()) //All other units go in army 
	{
		info.job = UnitInfo::DEFEND;
		info.unit->move(BWAPI::Point<int, BWAPI::POSITION_SCALE >::Point(DataManager::selfStart));

		//Add it to army
		Army.push_back(info);
		return;
	}
	else if (info.type.canProduce()) //If production building
	{
		info.job = UnitInfo::PRODUCE;

		//Add it to buildings
		Producers.push_back(info);
		return;
	}
	else if (!info.type.researchesWhat().empty()) //If upgrade building
	{
		info.job = UnitInfo::RESEARCH;

		//Add it to researchers
		Researchers.push_back(info);
		return;
	}
	else { return; } //Useless buildings
}

/*
Manually delete elements from a UnitList
Necessary because compiler can't figure out how to compare two UnitInfo types
But it can compare two Unit types
*/
void DataManager::remove(UnitInfo info)
{
	usedSupply -= info.type.supplyRequired();
	totalSupply -= info.type.supplyProvided();
	
	//Finds and removes unit data
	if (info.type.isWorker()) //If worker
	{
		UnitList list = Workers;
		for (std::list<UnitInfo>::iterator &current = list.begin(); current != list.end(); ++current)
		{
			if (current->unit == info.unit)
			{
				list.erase(current);
			}
		}
		return;
	}
	else if (!info.type.isBuilding()) //All other units go in army 
	{
		UnitList list = Army;
		for (std::list<UnitInfo>::iterator &current = list.begin(); current != list.end(); ++current)
		{
			if (current->unit == info.unit)
			{
				list.erase(current);
			}
		}
		return;
	}
	else if (info.type.canProduce()) //If production building
	{
		UnitList list = Producers;
		for (std::list<UnitInfo>::iterator &current = list.begin(); current != list.end(); ++current)
		{
			if (current->unit == info.unit)
			{
				list.erase(current);
			}
		}
		return;
	}
	else if (!info.type.researchesWhat().empty()) //If upgrade building (Currently useless -- for future development)
	{
		UnitList list = Researchers;
		for (std::list<UnitInfo>::iterator &current = list.begin(); current != list.end(); ++current)
		{
			if (current->unit == info.unit)
			{
				list.erase(current);
			}
		}
		return;
	}
	else { return; } //Useless buildings

}

//Checks and enacts the funding of a desired unit or building
bool DataManager::purchaseRequest(BWAPI::UnitType toBuild)
{
	if (!toBuild)
	{
		return false;
	}
	if ( (toBuild.mineralPrice() > mineralCheckbook) || (toBuild.gasPrice() > gasCheckbook) )
	{
		return false;
	}

	if (totalSupply - usedSupply < toBuild.supplyRequired())
	{
		return false;
	}
	
	BWAPI::Broodwar << "Purchase: " << toBuild << std::endl;
	mineralCheckbook -= toBuild.mineralPrice();
	gasCheckbook -= toBuild.gasPrice();
	usedSupply += toBuild.supplyRequired();
	return true;
}

//Undoes a purchase in the checkbook (in case the build attempt fails)
void DataManager::purchaseUndo(BWAPI::UnitType toBuild)
{
	BWAPI::Broodwar << "Undo: " << toBuild << std::endl;
	mineralCheckbook += toBuild.mineralPrice();
	gasCheckbook += toBuild.gasPrice();
	usedSupply -= toBuild.supplyRequired();
}

//Checks whether a unit can be built 
//To check building, only use one argument
//To check unit, make the second argument the production building
bool DataManager::techCheck(BWAPI::UnitType toBeBuilt)
{
	//For future development: encode the tech tree into DataManager in order to perform technology checks
	return true;
}