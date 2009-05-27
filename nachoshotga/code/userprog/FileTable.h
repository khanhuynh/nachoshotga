
#ifndef FILETABLE_H
#define FILETABLE_H

#include "bitmap.h"
#include "iobase.h"
#include "rofile.h"
#include "rwfile.h"
#include "cin.h"
#include "cout.h"

#define CIN 0
#define COUT 1

#define MAX 10   // So doi tuong quan ly
#define  RW 0
#define  RO 1


class CFileTable
{
 public:
  CFileTable();
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
  CIOBase *m_pIO[MAX];
};

#endif


