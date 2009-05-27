#ifndef RWFILE_H
#define RWFILE_H


#include "openfile.h"
#include "iobase.h"


class CRWFile:public CIOBase{
 public:
  CRWFile(OpenFile *f);
  virtual ~CRWFile();

  int   fWrite(int virtAddr,int size);
  int   fRead(int virtAddr,int size);
  int   fClose();
 private:
  OpenFile* m_pFile;
};
#endif
