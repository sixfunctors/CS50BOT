/*

BuildingManager.cpp

- Receives buildorder queue from StrategyManager
- Sends build commands to WorkerManager

*/

#include "BuildingManager.h"

using namespace CS50BOT;

BuildOrder BuildingManager::currentBO;

//Constructor
BuildingManager::BuildingManager()
{
	
}

//Keep persistent instance (only initializes once)
BuildingManager& BuildingManager::Instance()
{
	static BuildingManager instance;
	return instance;
}

//Takes starting information from game to initialize BuildingManager
void BuildingManager::initialize()
{
	//Hardcoded build order
	currentBO.add(BWAPI::UnitTypes::Enum::Protoss_Gateway);
	currentBO.add(BWAPI::UnitTypes::Enum::Protoss_Gateway);
	currentBO.add(BWAPI::UnitTypes::Enum::Protoss_Cybernetics_Core);
	currentBO.add(BWAPI::UnitTypes::Enum::Protoss_Assimilator);
	currentBO.add(BWAPI::UnitTypes::Enum::Protoss_Citadel_of_Adun);
	currentBO.add(BWAPI::UnitTypes::Enum::Protoss_Templar_Archives);

	//Inform ProductionManager what unit types to build
	std::array<BWAPI::UnitType, COMP_MAX> units = { BWAPI::UnitTypes::Enum::Protoss_Dark_Templar, BWAPI::UnitTypes::Enum::Protoss_Dragoon, BWAPI::UnitTypes::Enum::Protoss_Zealot };
	ProductionManager::Instance().setUnitProduction(BWAPI::UnitTypes::Enum::Protoss_Gateway, units);
	moneyDump = BWAPI::UnitTypes::Enum::Protoss_Gateway;

	BWAPI::Broodwar << "Build Order seeded!" << std::endl;
	BWAPI::UnitType x = currentBO.front();
	print();
}

//Frame update
void BuildingManager::update(int used, int total)
{
	//What should BuildingManager do each frame?
	if ((used + 8 >= total) && (currentBO.front() != DataManager::supplyProvider) && (pylonTimer <= 0))
	{
		BWAPI::Broodwar << "Supply provider added to front of BO" << std::endl;
		currentBO.push_front(DataManager::supplyProvider);
		print();
	}

	//Requests to purchase the first thing in the BO
	BWAPI::UnitType building = currentBO.front();
	if (currentBO.empty())
	{
		return;
	}
	if (DataManager::Instance().techCheck(building)) //If it passes the check
	{
		if (DataManager::Instance().purchaseRequest(building))//If enough money for that thing
		{
			if (WorkerManager::Instance().build(building)) //If it builds successfully
			{
				BWAPI::Broodwar << "Building a " << building << "!" << std::endl;
				currentBO.pop_front();
				print();
				if (building == DataManager::supplyProvider)
				{
					pylonTimer = DataManager::supplyProvider.buildTime(); //Keep a timer of how long between pylons
				}
			}
			else //Undo the transaction
			{
				DataManager::Instance().purchaseUndo(building);
			}
		}
	}

	pylonTimer--;
}

//Keeps updated the next item in the BO printed to top left
void BuildingManager::print()
{
	BWAPI::UnitType x = currentBO.front();
	BWAPI::Broodwar->registerEvent(
		[x](BWAPI::Game*){
		BWAPI::Broodwar->drawTextScreen(1, 28, "Next in BO: %s", x.c_str());
	}, [x](BWAPI::Game*)->bool{ return (x == currentBO.front()); });
}

//If you have excess money build another gateway
void BuildingManager::underspending()
{
	currentBO.push_front(moneyDump);
}