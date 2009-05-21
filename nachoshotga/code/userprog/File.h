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
  int m_iType; // Loai khi mo file, tuy theo no loai gi ma thuc hien doc ghi cho phu hop
	//Loai R thi ko cho ghi, loai W thi khong cho doc
};
#endif
