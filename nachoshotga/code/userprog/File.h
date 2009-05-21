#ifndef FILE_H
#define FILE_H

#include "system.h"
#include "../userprog/syscall.h"
#include "bitmap.h"

class CFile
{
 public:
  CFile();
  CFile(OpenFile *file);
  ~CFile();
  int   fWrite(char* FileName, int iSize);
  int   fRead(char* FileName, int iSize);
  int   fClose(int fID);
 private:
  OpenFile *m_pFile;
};
#endif
