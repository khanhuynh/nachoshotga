
#ifndef FILETABLE_H
#define FILETABLE_H

#include "bitmap.h"
#include "File.h"
#include "scexception.h"


#define  MAX  10   // So file quan ly
#define  W 0
#define  R 1


class CFileTable
{
 public:
  CFileTable(int size);
  ~CFileTable();

  int   fWrite(char* FileName, int iSize, int fID);
  int   fRead(char* FileName, int iSize, int fID);
  int   fOpen(char* FileName, int iType, int fID, OpenFile *f);
  int   fClose(int fID);
  int   GetSize();
  int   FindFreeSlot();
  bool  IsExist(int fID);
 private:
  int m_iSize;
  BitMap *m_bm;
  CFile *m_pFile[MAX];
};

#endif


