
#ifndef FILETABLE_H
#define FILETABLE_H

#include "bitmap.h"
#include "File.h"



#define MAX 10   // So file quan ly



class CFileTable
{
 public:
  CFileTable(int size);
  ~CFileTable();

  int   fWrite(int iVirAddr, int iSize, int fID);
  int   fRead(int iVirAddr, int iSize, int fID);
  int   fOpen(int iType, int fID, OpenFile *f);
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


