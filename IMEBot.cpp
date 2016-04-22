#include "IMEBot.h"

#include <string>
#include <cmath>
#include "util.h"

const float eps = 0.005f;

IMEBot::IMEBot()
{}

IMEBot::~IMEBot()
{}

void IMEBot::Process()
{
  float t = gamestate->timeStep * gamestate->tick;
  float alpha = degtorad(myShip->ang);
  vec2 shipPos { myShip->posx, myShip->posy };

  vec2 resForce {0, 0};

  for (auto rock : gamestate->rocks) {
    vec2 pos { rock.second->posx, rock.second->posy };
    pos = shipPos - pos;
    if (mag(pos) < 16.0f) {
      resForce += norm(pos) * (10.0f / squaremag(pos));
    }
  }

  vec2 refForce;
  refForce.x = resForce.x * cos(alpha) - resForce.y * sin(alpha);
  refForce.y = resForce.x * sin(alpha) + resForce.y * cos(alpha);

  thrust = refForce.y;
  sideThrustFront = refForce.x / 2;
  sideThrustBack = refForce.x / 2;
  shoot = 0;
}
