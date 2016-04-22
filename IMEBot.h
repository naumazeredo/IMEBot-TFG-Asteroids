#ifndef IMEBot_H
#define IMEBot_H

#include "BotBase.h"
#include "control.h"

class IMEBot : public BotBase
{
private:
  Control controly, controlx, controla;

public:
  IMEBot();
  virtual ~IMEBot();
  virtual void Process();
};

#endif
