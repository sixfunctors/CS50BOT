/*

CombatManager.h

- Controls attacking units

*/
#pragma once

#include "Core.h"
#include "DataManager.h"
#include "WorkerManager.h"

namespace CS50BOT
{
	class CombatManager
	{
		std::list<BWAPI::Position> spawns;
		UnitInfo army;
		BWAPI::Position main;
		BWAPI::Position currentTarget;
		
		CombatManager();
	public:
		static CombatManager& Instance();
		void initialize();
		void update();
		void attack();
	};
}