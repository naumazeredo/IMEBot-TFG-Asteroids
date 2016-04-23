#include "IMEBot.h"

#include <string>
#include <cmath>
#include <ctime>

//const float eps = 0.005f;

IMEBot::IMEBot() : state(STABLE) {
}

IMEBot::~IMEBot()
{}

void IMEBot::nextState() {
  repForces = calculateRepulsiveForces();
  if (state == STABLE) {
    if (mag(repForces) >= 0.4f) {
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
    if (mag(repForces) >= 0.4f) {
      state = DODGING;
    }
    else {
      state = STABILIZING;
    }
  }
  else if (state == TURN_AND_LOOK) {
    if (mag(repForces) >= 0.4f) {
      state = PRE_DODGE;
      startCounter();
    }
    else {
      state = TURN_AND_LOOK;
    }
  }
  else if (state == STABILIZING) {
    if (mag(repForces) >= 0.4f) {
      state = PRE_DODGE;
      startCounter();
    }
    else if ((fabs(myShip->velAng) < 10.0f)) { //values to be tested
      state = STABLE;
    }
    else {
      state = STABILIZING;
    }
  }
  else {  //if (state == PRE_DODGE) {
    if ((gamestate->timeStep * gamestate->tick - timeZero) > 0.1) {   //100 ms trying to stabilize
      if (mag(repForces) >= 0.4f) {
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

string IMEBot::getState() {
  if (state == STABLE) {
    return "STABLE";
  }
  else if (state == STABILIZING) {
    return "STABILIZING";
  }
  else if (state == PRE_DODGE) {
    return "PRE_DODGE";
  }
  else if (state == DODGING) {
    return "DODGING";
  }
  else {
    return "TURN_AND_LOOK";
  }
}

vec2 IMEBot::calculateRepulsiveForces() {
  vec2 repForces {0.0f, 0.0f};
  //calculate wall forces
  /*
  float distanceCenter = mag(myShip->pos);
  if (fabs(gamestate->arenaRadius - distanceCenter) <= (2 * myShip->radius)) {
    repForces += norm(myShip->pos) * (-1 * (distanceCenter));
  }
  */
  //calculate rock forces
  for (pair<int, Rock*> rock : gamestate->rocks) {
    // TODO(naum): relative velocity as potential weight
    /*
    vec2 deltaPos = myShip->pos - rock.second->pos;
    if (mag(deltaPos) < 10.0f) {
      repForces += norm(deltaPos) * (10.0f / mag(deltaPos));
    }
    // TODO(naum): Improve using laser avoidance
    // */
    vec2 deltaPos = rock.second->pos - myShip->pos;
    vec2 relVel;
    float magnitude;
    if (mag(deltaPos) < 15.0f) {
      relVel = rock.second->vel - myShip->vel;
      magnitude = 50 * dot(norm(deltaPos), relVel) / squaremag(deltaPos);
      if (magnitude <= 0) {
        repForces += norm(deltaPos) * magnitude;
      }
    }
  }

  //calculate laser forces
  for (pair<int, Laser*> laser : gamestate->lasers) {
    // TODO(naum): Use referencial velocity
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

      repForces += norm(perp) * force;
    }
  }

  return repForces;
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
  nextState();
  gamestate->Log(getState());
  gamestate->Log(to_string(repForces.x) + ", " + to_string(repForces.y));

  shoot = 0;

  if (state == STABLE) {
    stabilize();
  }
  else if (state == STABILIZING) {
    stabilize();
  }
  else if (state == PRE_DODGE) {
    stabilize();
  }
  else if (state == DODGING) {
    float shipAngle = degtorad(myShip->ang);
    vec2 refForce = rotate(repForces, -shipAngle);
    thrust = clamp(refForce.y);
    sideThrustFront = clamp(refForce.x / 2);
    sideThrustBack = clamp(refForce.x / 2);

    float closerDist = 999999.0f;
    Ship* closer = nullptr;

    for (auto ship : gamestate->ships) {
      if (ship.second->uid == myShip->uid) continue;

      if (mag(ship.second->pos - myShip->pos) < closerDist) {
        closer = ship.second;
        closerDist = mag(ship.second->pos - myShip->pos);
      }
    }

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
      

      float force = lookAt(closer->pos);
      sideThrustBack += force;
      sideThrustFront -= force;
    }
  }
  else {
    stabilize();
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
      

      float force = lookAt(closer->pos);
      sideThrustBack += force;
      sideThrustFront -= force;
    }
  }
  /*
  //float t = gamestate->timeStep * gamestate->tick;


  vec2 resForce = calculateRepulsiveForces();

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
  */
}
