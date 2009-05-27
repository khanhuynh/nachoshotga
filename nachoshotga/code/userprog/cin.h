#ifndef CIN_H
#define CIN_H

#include "iobase.h"


class CCin:public CIOBase
{
 public:
  CCin();
  virtual ~CCin();
  int   fWrite(int iVirtAddr,int iSize);
  int   fRead(int iVirtAddr,int iSize);
  int   fClose();
};
#endif
