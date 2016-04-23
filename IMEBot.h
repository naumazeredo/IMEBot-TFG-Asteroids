#ifndef IMEBot_H
#define IMEBot_H

#include "BotBase.h"
#include "control.h"
#include "util.h"

typedef enum State { STABLE, STABILIZING, PRE_DODGE, DODGING, TURN_AND_LOOK } State;

class IMEBot : public BotBase
{
private:
  State state;
  float timeZero;
  vec2 repForces;

  vec2 calculateRepulsiveForces();
  void startCounter();
  void stabilize();
  float lookAt(vec2);

  void nextState();
  string getState();
public:
  IMEBot();
  virtual ~IMEBot();
  virtual void Process();
};

#endif
