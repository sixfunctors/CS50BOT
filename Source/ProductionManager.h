/*

ProductionManager.h

-Keeps Queues producing units at all times

*/

#pragma once

#include "Core.h"
#include "DataManager.h"
#include "WorkerManager.h"
#include "BuildingManager.h"


namespace CS50BOT
{
	class ProductionManager
	{
		//Creates a set of rules for what buildings should be building what units
		//Does this by associating to every building type an array of unit types, building the first available type
		std::map<BWAPI::UnitType, std::array<BWAPI::UnitType, COMP_MAX>> UnitComp;

	public:
		ProductionManager();
		static ProductionManager& Instance();
		void initialize();

		void update();
		bool produce(UnitInfo*);
		void setUnitProduction(BWAPI::UnitType building, std::array<BWAPI::UnitType, COMP_MAX>);
		

	};
}