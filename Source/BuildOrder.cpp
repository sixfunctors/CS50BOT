/*

BuildOrder.cpp

-Defines functions for working with BuildOrders

*/

#include "BuildOrder.h"

using namespace CS50BOT;

//Adds a new item to a BuildOrder
//Ensures the BO is always sorted by priority
void BuildOrder::add(BWAPI::UnitType building)
{
	this->push_back(building);
	return;
}

void BuildOrder::dump(std::string path)
{
	//If no path given, dump to terminal
	if (path == "")
	{
		BWAPI::Broodwar << "The current build order is:" << std::endl;
		for (BuildOrder::const_iterator i = this->begin(); i != this->end(); ++i)
		{
			BWAPI::Broodwar << (*i) << std::endl;
		}
	}
	else //Dump to file path
	{
		std::ofstream output;
		output.open(path);
		for (BuildOrder::const_iterator i = this->begin(); i != this->end(); ++i)
		{
			BWAPI::Broodwar << (*i) << std::endl;
		}
		output.close();
	}
}