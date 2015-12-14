/*

DataManager.h

-Manages the army units

*/

#pragma once

#include "Core.h"
#include "WorkerManager.h" 
#include "ProductionManager.h"
#include "CombatManager.h"



namespace CS50BOT
{
	struct DataObject
	{
		//Total minerals/gas mined to this point in the game
		int mineralsTotal; 
		int gasTotal;
	};


	class DataManager
	{	
		DataManager();

		//Frame Data
		int oldMineralsTotal;
		int oldGasTotal;
		int miningDataDelay;

		//Internal register for funds
		int mineralCheckbook;
		int gasCheckbook;

		//Internal register for supply
		int usedSupply;
		int totalSupply;

		//Player pointer
		BWAPI::Player player;
		BWAPI::Player enemy;

		int frames;

		//Triggers
		bool attackTrigger;
	public:
		/* Data */

		//Unit data
		static UnitList Army;
		static UnitList Workers;
		static UnitList Producers;
		static UnitList Researchers;

		//Start of game data
		static BWAPI::Race playerRace;
		static BWAPI::Race enemyRace;
		static BWAPI::UnitType supplyProvider;
		static BWAPI::TilePosition::list startingLocations;
		static BWAPI::TilePosition selfStart;

		/* Functions */
		static DataManager & Instance();
		void initialize(BWAPI::Player self, BWAPI::Player enemy, BWAPI::Unitset units, BWAPI::TilePosition::list starts);
		void update(DataObject);

		//Unit Manipuation
		void add(BWAPI::Unit);
		void remove(UnitInfo);

		//Build&produce requests
		bool purchaseRequest(BWAPI::UnitType toBuild);
		void purchaseUndo(BWAPI::UnitType toBuild);
		bool techCheck(BWAPI::UnitType toBeBuilt);
	};
}
