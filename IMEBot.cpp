#include "IMEBot.h"

#include <string>
#include <cmath>

const float eps = 0.005f;

IMEBot::IMEBot()
{}

IMEBot::~IMEBot()
{}

void IMEBot::stabilize() {
  float angvel = myShip->velAng;

  if (fabs(angvel) > 10.0f) {
    thrust = 0;
    sideThrustFront = angvel / 100;
    sideThrustBack = -angvel / 100;
  } else {
    vec2 vel { myShip->velx, myShip->vely };
    vec2 refVel = rotate(vel, -degtorad(myShip->ang));

    thrust = -clamp(0.3f * refVel.y);
    sideThrustFront = -clamp(0.2f * refVel.x / 2);
    sideThrustBack = -clamp(0.2f * refVel.x / 2);
  }
}

float IMEBot::lookAt(vec2 target) {
  vec2 deltaPos { myShip->posx, myShip->posy };
  deltaPos = target - deltaPos;
  vec2 refTarget = rotate(deltaPos, -degtorad(myShip->ang));

  float theta = atan2(refTarget.y, refTarget.x) - M_PI/2.0f;

  return 3.0f * theta;
}

void IMEBot::Process()
{
  float t = gamestate->timeStep * gamestate->tick;

  float shipAngle = degtorad(myShip->ang);
  vec2 shipPos { myShip->posx, myShip->posy };

  vec2 resForce {0, 0};

  // TODO(naum): Avoid walls?

  for (auto rock : gamestate->rocks) {
    // TODO(naum): relative velocity as potential weight
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

  vec2 refForce = rotate(resForce, -shipAngle);

  float closerDist = 999999.0f;

  if (mag(refForce) > 0.4f) {
    thrust = clamp(refForce.y);
    sideThrustFront = clamp(refForce.x / 2);
    sideThrustBack = clamp(refForce.x / 2);
  } else {
    stabilize();

    vec2 vel { myShip->velx, myShip->vely };

    Ship* closer = nullptr;
    for (auto ship : gamestate->ships) {
      if (ship.second->uid == myShip->uid) continue;

      vec2 pos { ship.second->posx, ship.second->posy };
      if (mag(pos - shipPos) < closerDist) {
        closer = ship.second;
        closerDist = mag(pos - shipPos);
      }
    }

    if (closer) {
      float force = lookAt({closer->posx, closer->posy});
      sideThrustBack += force;
      sideThrustFront -= force;
    }
  }

  shoot = 0;

  if (closerDist < 25.0f * ((int)myShip->charge) * 3)
    shoot = (int)myShip->charge;
}
