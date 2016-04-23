#include "IMEBot.h"

#include <string>
#include <cmath>

//const float eps = 0.005f;

IMEBot::IMEBot() : state(STABLE) {
}

IMEBot::~IMEBot()
{}

void IMEBot::nextState() {
  vec2 repForce = calculateRepulsiveForces();
  if (state == STABLE) {
    if (mag(repForce) >= 0.4f) {
      state = DODGING;
    }
    else if (gamestate->ships.size() != 1) {
      state = TURN_AND_LOOK;
    }
    else {
      state = STABLE;
    }
  }
  else if (state == DODGING) {
    if (mag(repForce) >= 0.4f) {
      state = DODGING;
    }
    else {
      state = STABILIZING;
    }
  }
  else if (state == TURN_AND_LOOK) {
    if (mag(repForce) >= 0.4f) {
      state = PRE_DODGE;
      startCounter();
    }
    else {
      state = TURN_AND_LOOK;
    }
  }
  else if (state == STABILIZING) {
    if (mag(repForce) >= 0.4f) {
      state = PRE_DODGE;
    }
    else if ((fabs(myShip->velAng) < 10.0f) && (fabs(mag(myShip->vel)) < 0.1f)) { //values to be tested
      state = STABLE;
    }
    else {
      state = STABILIZING;
    }
  }
  else {  //if (state == PRE_DODGE) {
    if ((gamestate->timeStep * gamestate->tick - timeZero) > 0.1) {   //100 ms trying to stabilize
      if (mag(repForce) >= 0.4f) {
        state = DODGING;
      }
      else {
        state = STABILIZING;
      }
    }
    else {
      state = PRE_DODGE;
    }
  }
}

vec2 IMEBot::calculateRepulsiveForces() {
  //TODO
  return vec2{0.0f, 0.0f};
}

void IMEBot::startCounter() {
  timeZero = gamestate->timeStep * gamestate->tick;
}

void IMEBot::stabilize() {
  float angvel = myShip->velAng;

  if (fabs(angvel) > 10.0f) {
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
    // TODO(naum): relative velocity as potential weight
    vec2 deltaPos = myShip->pos - rock.second->pos;
    if (mag(deltaPos) < 10.0f) {
      resForce += norm(deltaPos) * (10.0f / mag(deltaPos));
    }

    // TODO(naum): Improve using laser avoidance
  }

  // Laser avoidance
  for (auto laser : gamestate->lasers) {
    // TODO(naum): Use referencial velocity
    vec2 dir = laser.second->vel;
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

  // TODO(naum): Go near enemy if closer enemy is close enough
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
}
