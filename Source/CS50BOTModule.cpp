/*

CS50BOTModule.cpp
by Connor Halleck-Dube

-Initializes the AI
-Detects and reads the game state
-Delegates information to other modules 

*/

#include "CS50BOTModule.h"

using namespace CS50BOT;

void CS50BOTModule::onStart()
{
	BWAPI::Broodwar->sendText("CS50BOT Initialized!");

	//Initialize DataManager
	DataManager::Instance().initialize(
		BWAPI::Broodwar->self(), 
		BWAPI::Broodwar->enemy(),
		BWAPI::Broodwar->self()->getUnits(),
		BWAPI::Broodwar->getStartLocations());

	// Print the map name.
	// BWAPI returns std::string when retrieving a string, don't forget to add .c_str() when printing!
	BWAPI::Broodwar << "The map is " << BWAPI::Broodwar->mapName() << "!" << std::endl;

	// Enable the UserInput BWAPI::Flag, which allows us to control the bot and type messages.
	//BWAPI::Broodwar->enableFlag(BWAPI::Flag::UserInput);

	// Set the command optimization level so that common commands can be grouped
	// and reduce the bot's APM.
	BWAPI::Broodwar->setCommandOptimizationLevel(2);

	// Check if this is a replay
	if ( BWAPI::Broodwar->isReplay() )
	{
		// Announce the players in the replay
		BWAPI::Broodwar << "The following players are in this replay:" << std::endl;
    
		// Iterate all the players in the game using a std:: iterator
		BWAPI::Playerset players = BWAPI::Broodwar->getPlayers();
		for(auto p : players)
		{
			// Only print the player if they are not an observer
			if ( !p->isObserver() )
			BWAPI::Broodwar << p->getName() << ", playing as " << p->getRace() << std::endl;
		}
	}
	else // if this is not a replay
	{
		// Retrieve you and your enemy's races. enemy() will just return the first enemy.
		// If you wish to deal with multiple enemies then you must use enemies().
		if ( BWAPI::Broodwar->enemy() ) // First make sure there is an enemy
			BWAPI::Broodwar << "The matchup is " << BWAPI::Broodwar->self()->getRace() << " vs " << BWAPI::Broodwar->enemy()->getRace() << std::endl;
	}

	//Initialize the StrategyManager
	//StrategyManager::Instance().initialize();
}

void CS50BOTModule::onEnd(bool isWinner)
{
  // Called when the game ends
  if ( isWinner )
  {
    // Log your win here!
  }
}

// Called once every game frame

void CS50BOTModule::onFrame()
{
	
	// Return if the game is a replay or is paused
	if ( BWAPI::Broodwar->isReplay() || BWAPI::Broodwar->isPaused() || !BWAPI::Broodwar->self() )
		return;

	// Prevent spamming by only running our onFrame once every number of latency frames.
	// Latency frames are the number of frames before commands are processed.
	if ( BWAPI::Broodwar->getFrameCount() % BWAPI::Broodwar->getLatencyFrames() != 0 )
		return;

	//Update DataManager
	DataObject frameData;
	frameData.mineralsTotal = BWAPI::Broodwar->self()->gatheredMinerals();
	frameData.gasTotal = BWAPI::Broodwar->self()->gatheredGas();
	DataManager::Instance().update(frameData);

	//Update StrategyManager
	//StrategyManager::Instance().update();
  
}

void CS50BOTModule::onSendText(std::string text)
{

  // Send the text to the game if it is not being processed.
  BWAPI::Broodwar->sendText("%s", text.c_str());


  // Make sure to use %s and pass the text as a parameter,
  // otherwise you may run into problems when you use the %(percent) character!

}

void CS50BOTModule::onReceiveText(BWAPI::Player player, std::string text)
{
	// Parse the received text
	BWAPI::Broodwar << player->getName() << " said \"" << text << "\"" << std::endl;
}

void CS50BOTModule::onPlayerLeft(BWAPI::Player player)
{
	// Interact verbally with the other players in the game by
	// announcing that the other player has left.
	BWAPI::Broodwar->sendText("Goodbye %s!", player->getName().c_str());
}

void CS50BOTModule::onNukeDetect(BWAPI::Position target)
{

	// Check if the target is a valid position
	if ( target )
	{
		// if so, print the location of the nuclear strike target
		BWAPI::Broodwar << "Nuclear Launch Detected at " << target << std::endl;
	}
	else 
	{
		// Otherwise, ask other players where the nuke is!
		BWAPI::Broodwar->sendText("Where's the nuke?");
	}

	// You can also retrieve all the nuclear missile targets using BWAPI::Broodwar->getNukeDots()!
}

void CS50BOTModule::onUnitDiscover(BWAPI::Unit unit)
{
}

void CS50BOTModule::onUnitEvade(BWAPI::Unit unit)
{
}

void CS50BOTModule::onUnitShow(BWAPI::Unit unit)
{
}

void CS50BOTModule::onUnitHide(BWAPI::Unit unit)
{
}

void CS50BOTModule::onUnitCreate(BWAPI::Unit unit)
{
	if ( BWAPI::Broodwar->isReplay() )
	{
		// if we are in a replay, then we will print out the build order of the structures
		if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
		{
			int seconds = BWAPI::Broodwar->getFrameCount()/24;
			int minutes = seconds/60;
			seconds %= 60;
			BWAPI::Broodwar->sendText("%.2d:%.2d: %s creates a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void CS50BOTModule::onUnitDestroy(BWAPI::Unit unit)
{
	if (unit->getPlayer() != BWAPI::Broodwar->self())
	{
		return;
	}

	UnitInfo info;
	info.unit = unit;
	info.type = unit->getType();
	DataManager::Instance().remove(info);
}

void CS50BOTModule::onUnitMorph(BWAPI::Unit unit)
{
	if ( BWAPI::Broodwar->isReplay() )
	{
		// if we are in a replay, then we will print out the build order of the structures
		if ( unit->getType().isBuilding() && !unit->getPlayer()->isNeutral() )
		{
			int seconds = BWAPI::Broodwar->getFrameCount()/24;
			int minutes = seconds/60;
			seconds %= 60;
			BWAPI::Broodwar->sendText("%.2d:%.2d: %s morphs a %s", minutes, seconds, unit->getPlayer()->getName().c_str(), unit->getType().c_str());
		}
	}
}

void CS50BOTModule::onUnitRenegade(BWAPI::Unit unit)
{
}

void CS50BOTModule::onSaveGame(std::string gameName)
{
	BWAPI::Broodwar << "The game was saved to \"" << gameName << "\"" << std::endl;
}

void CS50BOTModule::onUnitComplete(BWAPI::Unit unit)
{ 
	//Send new unit to DataManager
	DataManager::Instance().add(unit);
}
