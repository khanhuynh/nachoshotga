#include "cin.h"
#include "system.h"
#include "scexception.h"

CCin::CCin()
{
  return;
}

CCin::~CCin()
{
  return;
}  


int CCin::fWrite(int iVirtAddr,int iSize)
{
  printf("\n Khong the doc bang cin...");
  return -1; 
}

int CCin::fRead(int iVirtAddr,int iSize)
{
  int iAddr = iVirtAddr;
  int iSz = iSize;
  char *buf = new char[iSz + 1];

  ASSERT(buf != NULL);

  int iResult = gSynchConsole->Read(buf,iSz);

  // xoa phan thua cua input
  if (iResult == iSz && buf[iResult] != 10)   // 10 la ky tu ket thuc dong
  {
    char temp[11];
    int iLen = 0;
    // Xoa het input bang cach thuc hien doc lien tuc cho du dong
    do{
      iLen = gSynchConsole->Read(temp,10);
    }  while (iLen == 10 && temp[iLen-1] != 10);
  }

  if (iResult < 0)
  {
    printf("CCin:Loi fRead - Khong the doc tu SynchConsole!..");
    delete buf;
    return -1;
  }

  int rs = System2User(iAddr,iResult,buf);
  if (rs < 0)
      printf("\n CCin:Read:Loi khi chuyen sang khong gian nguoi dung");

  //  machine->WriteRegister(2,rsl);


  delete buf;
  return rs;
}

int   CCin::fClose()
{
  printf("\n Loi khong the dong Cin...");
  return -1;
}




