#include "IMEBot.h"

#include <string>
#include <cmath>
#include "util.h"

IMEBot::IMEBot() : controly(0.18f, 0.125f), controlx(0.18f, 0.125f), controla(0.18f, 0.125f)
{}

IMEBot::~IMEBot()
{}

void IMEBot::Process()
{
  float t = gamestate->timeStep * gamestate->tick;
  float alpha = degtorad(myShip->ang);

  float destX = 5, destY = 0, destAlpha = degtorad(0);
  float deltaX = destX - myShip->posx, deltaY = destY - myShip->posy;
  float refY = deltaY * cos(alpha) - deltaX * sin(alpha),
        refX = deltaX * cos(alpha) + deltaY * sin(alpha),
        refA = minangle(destAlpha - alpha);

  float pdy = controly.pd(refY, t);
  float pdx = controlx.pd(refX, t);
  float pda = controla.pd(refA, t);

  // XY
  /*
  thrust = clamp(pdy);
  sideThrustFront = clamp(pdx);
  sideThrustBack = clamp(pdx);
  */

  // Angle
  thrust = 0;
  sideThrustFront = -clamp(pda);
  sideThrustBack = clamp(pda);


  shoot = 0;
}
