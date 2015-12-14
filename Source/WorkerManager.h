/*

WorkerManager.h

-manages worker mining
-calls for protection for workers

*/

#pragma once

#include "Core.h"
#include "DataManager.h"
#include "ProductionManager.h"

namespace CS50BOT
{
	class WorkerManager
	{
		//"Couldnt find any desired worker"
		static UnitInfo nullWorker;

		UnitInfo* getWorker(UnitInfo::Job);
		WorkerManager();
		BWAPI::UnitType workerType;

	public:
		static WorkerManager& Instance();
		void initialize();
		
		void update();
		bool build(BWAPI::UnitType, UnitInfo* b = &nullWorker);

		void setGasProduction(BWAPI::Unit refinery);
	};

}


