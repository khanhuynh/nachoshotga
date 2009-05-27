#include "cout.h"
#include "scexception.h"
#include "system.h"

CCout::CCout()
{
  return; 
}

CCout::~CCout()
{
  return ;
}
int CCout::fWrite(int iVirtAddr,int iSize)
{
  int iAddr = iVirtAddr;
  int iSz = iSize;

  if (iSz < 0){
    printf("\n CCout:Loi fWrite ! So bytes khong xac dinh %d", iSize);
    return -1;
  }
  else if (iSz == 0)
    return 0;

  char *buf = User2System(iAddr,iSz);
  if (buf == NULL || buf == (char*)0)
    {
      printf("\n CCout:Loi fWrite - buffer rong !");
      if (buf != NULL)
      {
        delete buf;
      }  
      return -1;
    }
  int iResult = gSynchConsole->Write(buf,strlen(buf));
  if (iResult < 0)
    printf("CCout:Loi fWrite - Loi ghi bang Synchcons!..");

  delete buf;
  return iResult;
}

int CCout::fRead(int iVirtAddr,int iSize)
{
  printf("\n Khong the doc bang cout...");
  return -1;
}

int CCout::fClose()
{
  printf("\n Cout khong co gi de dong..."); 
  return -1; 
}


