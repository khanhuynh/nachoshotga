#ifndef COUT_H
#define COUT_H


#include "iobase.h"


class CCout:public CIOBase
{
 public:
  CCout();
  virtual ~CCout();
  int   fWrite(int iVirtAddr,int iSize);
  int   fRead(int iVirtAddr,int iSize);
  int   fClose();

};
#endif
