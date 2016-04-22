#ifndef IMEBot_H
#define IMEBot_H

#include "BotBase.h"
#include "control.h"
#include "util.h"

class IMEBot : public BotBase
{
private:
  void stabilize();

  float lookAt(vec2);
public:
  IMEBot();
  virtual ~IMEBot();
  virtual void Process();
};

#endif
