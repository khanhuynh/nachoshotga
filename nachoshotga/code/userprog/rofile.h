#ifndef ROFILE_H
#define ROFILE_H

#include "syscall.h"
#include "iobase.h"
#include "openfile.h"

class CROFile:public CIOBase{
 public:
  CROFile(OpenFile *f);
  virtual ~CROFile();

  int   fWrite(int virtAddr,int size);
  int   fRead(int virtAddr,int size);
  int   fClose();

 private:
  OpenFile* file;
};
#endif

