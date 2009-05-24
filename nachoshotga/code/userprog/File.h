#ifndef FILE_H
#define FILE_H

#include "openfile.h"
#include "../userprog/syscall.h"
#include "bitmap.h"

#define  RW 0
#define  R 1

class CFile
{
 public:
  CFile();
  CFile(OpenFile *file);
  ~CFile();
  int GetType();
  void PutType(int iType);
  int   fWrite(int iVirAddr, int iSize);
  int   fRead(int iVirAddr, int iSize);
  int   fClose(int fID);
 private:
  OpenFile *m_pFile;
  int m_iType;  // Loai khi mo file, tuy theo no loai gi ma thuc hien doc ghi cho phu hop
	              //Loai R thi ko cho ghi, loai W thi khong cho doc
};
#endif
