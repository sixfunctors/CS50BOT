/*

BuildOrder.h

-Defines functions on BuildOrder data type

*/

#include "Core.h"

namespace CS50BOT
{

	class BuildOrder : std::list < BWAPI::UnitType >
	{
		friend class BuildingManager;
		
	public:
		void add(BWAPI::UnitType);
		void dump(std::string path = ""); //Dumps the entire BO into that file path

	};

}