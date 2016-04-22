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
  float shipAngle = degtorad(myShip->ang);
  vec2 shipPos { myShip->posx, myShip->posy };

  vec2 resForce {0, 0};

  for (auto rock : gamestate->rocks) {
    vec2 deltaPos { rock.second->posx, rock.second->posy };
    deltaPos = shipPos - deltaPos;
    if (mag(deltaPos) < 16.0f) {
      resForce += norm(deltaPos) * (50.0f / squaremag(deltaPos));
    }
  }

  for (auto laser : gamestate->lasers) {
    vec2 dir { laser.second->velx, laser.second->vely };

    vec2 deltaPos { laser.second->posx, laser.second->posy };
    deltaPos = shipPos - deltaPos;

    float force = squaremag(dir) / squaremag(deltaPos);

    float projmag = mag(deltaPos) * dot(norm(dir), norm(deltaPos));
    float projx = (deltaPos - norm(dir) * projmag).x;
    if (projx < 4.0f) {
      gamestate->Log("steering!");
      vec2 perp { -dir.y, dir.x };
      if (dot(perp, deltaPos) < 0)
        perp = -perp;

      resForce += norm(perp) * force;
    }
  }

  vec2 refForce;
  refForce.x = resForce.x * cos(shipAngle) - resForce.y * sin(shipAngle);
  refForce.y = resForce.x * sin(shipAngle) + resForce.y * cos(shipAngle);

  //if (mag(refForce) < 0.

  //gamestate->Log(to_string(refForce.x) + " " + to_string(refForce.y));

  thrust = refForce.y;
  sideThrustFront = refForce.x / 2;
  sideThrustBack = refForce.x / 2;
  shoot = 0;
}
