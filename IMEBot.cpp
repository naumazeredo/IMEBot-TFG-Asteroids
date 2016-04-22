#include "IMEBot.h"

#include <string>
#include <cmath>
#include "util.h"

const float eps = 0.005f;

IMEBot::IMEBot()
{}

IMEBot::~IMEBot()
{}

void IMEBot::stabilize() {
  vec2 vel { myShip->velx, myShip->vely };
  vec2 refVel = rotate(vel, myShip->ang);

  thrust = -clamp(refVel.y);
  sideThrustFront = -clamp(refVel.x / 2);
  sideThrustBack = -clamp(refVel.x / 2);
}

void IMEBot::Process()
{
  float t = gamestate->timeStep * gamestate->tick;
  float shipAngle = degtorad(myShip->ang);
  vec2 shipPos { myShip->posx, myShip->posy };

  vec2 resForce {0, 0};

  for (auto rock : gamestate->rocks) {
    vec2 deltaPos { rock.second->posx, rock.second->posy };
    deltaPos = shipPos - deltaPos;
    if (mag(deltaPos) < 10.0f) {
      resForce += norm(deltaPos) * (10.0f / mag(deltaPos));
    }

    // TODO(naum): Improve using laser avoidance
  }

  for (auto laser : gamestate->lasers) {
    // TODO(naum): Use referencial velocity
    vec2 dir { laser.second->velx, laser.second->vely };

    vec2 deltaPos { laser.second->posx, laser.second->posy };
    deltaPos = shipPos - deltaPos;

    // Don't consider past lasers
    if (dot(deltaPos, dir) <= 0.0f) continue;

    float force = squaremag(dir) / squaremag(deltaPos);

    float projmag = mag(deltaPos) * dot(norm(dir), norm(deltaPos));
    float projx = (deltaPos - norm(dir) * projmag).x;
    if (fabs(projx) < 3.0f && projmag <= mag(dir) * laser.second->lifetime) {
      vec2 perp { -dir.y, dir.x };
      if (dot(perp, deltaPos) < 0)
        perp = -perp;

      resForce += norm(perp) * force;
    }
  }

  vec2 refForce = rotate(resForce, shipAngle);

  if (mag(refForce) > 0.4f) {
    thrust = clamp(refForce.y);
    sideThrustFront = clamp(refForce.x / 2);
    sideThrustBack = clamp(refForce.x / 2);
  } else {
    stabilize();
  }

  shoot = 0;
}
