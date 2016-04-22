#include "IMEBot.h"

#include <string>
#include <cmath>
#include "util.h"

IMEBot::IMEBot() : controly(0.15f, 0.12f), controlx(0.15f, 0.12f), controla(0.1f, 0.1f)
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

  // All
  /*
  float f = -pda, b = pda;
  if (fabs(f) > 1.0f || fabs(b) > 1.0f) {
    float m = max(fabs(f), fabs(b));
    f /= m;
    b /= m;
  }

  thrust = clamp(pdy);
  sideThrustFront = f;
  sideThrustBack = b;

  gamestate->Log(to_string(f) + ", " + to_string(b));
  */

  // XY
  thrust = clamp(pdy);
  sideThrustFront = clamp(pdx);
  sideThrustBack = clamp(pdx);

  // Angle
  /*
  thrust = 0;
  sideThrustFront = -clamp(pda);
  sideThrustBack = clamp(pda);
  */


  shoot = 0;
}
