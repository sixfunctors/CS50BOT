/*

Core.h

Contains data structures, libraries, and functions needed by multiple Modules

*/

#pragma once

#include "BWAPI.h"
#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <array>


#define MININGDATADELAY 10
#define COMP_MAX 5
#define SENSE_RANGE 112 //Minimum vision range in Points of any unit
#define ATTACK_FRAMES 7500 //Frames before attackTrigger fires


namespace CS50BOT
{
	//Stores a unit's type and job
	struct UnitInfo {
		enum Job { ATTACK, DEFEND, BUILD, MINE, GAS, PRODUCE, IDLE, RESEARCH };
		BWAPI::Unit unit;
		BWAPI::UnitType type;
		Job job = IDLE;
		int delay;

	};

	class UnitList : public std::list < UnitInfo >
	{
	public:
		void dump();
	};
}