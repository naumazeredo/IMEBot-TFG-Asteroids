#include "IMEBot.h"

#include <string>
#include <cmath>

//const float eps = 0.005f;

IMEBot::IMEBot()
{}

IMEBot::~IMEBot()
{}

void IMEBot::stabilize() {
  float angvel = myShip->velAng;

  if (fabs(angvel) > 30.0f) {
    thrust = 0;
    sideThrustFront = angvel / 100;
    sideThrustBack = -angvel / 100;
  } else {
    vec2 refVel = rotate(myShip->vel, -degtorad(myShip->ang));

    thrust = -clamp(0.3f * refVel.y);
    sideThrustFront = -clamp(0.2f * refVel.x / 2);
    sideThrustBack = -clamp(0.2f * refVel.x / 2);
  }
}

float IMEBot::lookAt(vec2 target) {
  vec2 deltaPos = target - myShip->pos;
  vec2 refTarget = rotate(deltaPos, -degtorad(myShip->ang));

  float theta = atan2(refTarget.y, refTarget.x) - M_PI/2.0f;
  if (theta < -M_PI) theta += 2 * M_PI;

  return 3.0f * theta;
}

void IMEBot::Process()
{
  //float t = gamestate->timeStep * gamestate->tick;

  float shipAngle = degtorad(myShip->ang);

  vec2 resForce {0, 0};

  // TODO(naum): Avoid walls?

  // Rock avoidance
  for (auto rock : gamestate->rocks) {
    vec2 deltaPos = myShip->pos - rock.second->pos;
    if (mag(deltaPos) < 15.0f) {
      vec2 relVel = rock.second->vel - myShip->vel;
      float k = dot(norm(deltaPos), relVel);
      if (k > 0.0f)
        resForce += norm(deltaPos) * (10 * k / mag(deltaPos));
    }

    // TODO(naum): Improve using laser avoidance
  }

  // Laser avoidance
  for (auto laser : gamestate->lasers) {
    vec2 dir = laser.second->vel - myShip->vel;
    vec2 deltaPos = myShip->pos - laser.second->pos;

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


  // Get closer enemy
  float closerDist = 999999.0f;
  Ship* closer = nullptr;

  for (auto ship : gamestate->ships) {
    if (ship.second->uid == myShip->uid) continue;

    if (mag(ship.second->pos - myShip->pos) < closerDist) {
      closer = ship.second;
      closerDist = mag(ship.second->pos - myShip->pos);
    }
  }

  // Shoot logic
  shoot = 0;

  if (closer) {
    vec2 deltaPos = closer->pos - myShip->pos;
    bool laserCanReach = closerDist < 25.0f * ((int)myShip->charge) * 2;

    if (laserCanReach) {
      bool preciseAngle = (fabs(dot(norm(deltaPos), { -1 * sin(degtorad(myShip->ang)), cos(degtorad(myShip->ang)) })) >= cos(degtorad(1)));
      if (preciseAngle) {
        shoot = (int)myShip->charge;
      }
    }
  }

  // Set thrusters force
  vec2 refForce = rotate(resForce, -shipAngle);
  if (mag(refForce) > 0.4f) {
    thrust = clamp(refForce.y);
    sideThrustFront = clamp(refForce.x / 2);
    sideThrustBack = clamp(refForce.x / 2);
  } else {
    stabilize();

    if (closer) {
      float force = lookAt(closer->pos);
      sideThrustBack += force;
      sideThrustFront -= force;
    }
  }

  // Victory!!!
  if (!closer) {
    thrust = 1.0f;
    sideThrustFront = 1.0f;
    sideThrustBack = -1.0f;
    shoot = 1;
  }
}
