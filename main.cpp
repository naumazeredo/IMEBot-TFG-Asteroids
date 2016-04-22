#include <iostream>
#include "bot_interface.h"

#include "IMEBot.h"

using namespace std;

int main (int argc, char *argv[])
{
  GameState * gamestate = new GameState();

  IMEBot* bot = new IMEBot();

  bot->gamestate = gamestate;
  bot->myShip = gamestate->myShip;

  gamestate->Log("Loaded");

  while(bot->myShip != NULL)
  {
    gamestate->Update();
    bot->Update();
    gamestate->WriteData();
  }

  delete gamestate;
  delete bot;

  return 0;
}
