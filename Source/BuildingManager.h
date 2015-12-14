/*

BuildingManager.h

*/

#pragma once

#include "Core.h"
#include "DataManager.h"
#include "WorkerManager.h"
#include "BuildOrder.h"

namespace CS50BOT
{
	
	class BuildingManager
	{
		BWAPI::UnitType moneyDump;

	private:
		BuildingManager();

		static BuildOrder currentBO;
		int pylonTimer;
		void print();

	public:		
		static BuildingManager& BuildingManager::Instance();
		void initialize();
		void update(int, int);
		void underspending();
	};
}