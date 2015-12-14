/*

Core.cpp

- Defines methods
*/

#include "Core.h"

using namespace CS50BOT;

//Prints the current units in a given list
void UnitList::dump()
{
	BWAPI::Broodwar << "Production Buildings:" << std::endl;
	for (std::list<UnitInfo>::const_iterator i = this->begin(); i != this->end(); ++i)
	{
		BWAPI::Broodwar << i->type << ", " << i->job << std::endl;
	}
}