#ifndef IOBASE_H
#define IOBASE_H


class CIOBase{
 public:
  virtual int  fWrite(int iVirAddr, int iSize) = 0;
  virtual int   fRead(int iVirAddr, int iSize) = 0;
  virtual int   fClose() = 0;

};
#endif
