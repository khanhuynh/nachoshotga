#include "rofile.h"
#include "scexception.h"

CROFile::CROFile(OpenFile *f)
{
  file = f;
}
  
CROFile::~CROFile()
{
  if (file != NULL)
    delete file;
}

int CROFile::fClose()
{
  delete file;
  file = NULL;
  return 0;
}

int CROFile::fWrite(int virtAddr, int size)
{
  printf("\n Can't write to read-only file");
  return -1;
}  


int CROFile::fRead(int virtAddr,int size)
{
  int irs = 0;
  int ivA = virtAddr;
  int isz = size;
  
  char *buf = new char[isz +1];

  ASSERT(buf != NULL);

  int ibytesread = file->Read(buf,isz);
  if (ibytesread < 0){
    printf("\n Error- read fail.");
    
    delete buf;
    return -1;
  }  
  else if (ibytesread == 0)
    {
     
      delete buf;
      return 0;
    }
  
  irs = System2User(ivA,ibytesread,buf);

  if(buf[ibytesread - 1] == EOF)
  {
    delete buf;
    return -2;
  }

  delete buf;
  return irs;
}

