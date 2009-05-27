
#include "rwfile.h"
#include "scexception.h"

CRWFile::CRWFile(OpenFile *f)
{
  m_pFile = f;
}

CRWFile::~CRWFile()
{
  if (m_pFile != NULL)
    delete m_pFile;
}

int CRWFile::fClose()
{
  delete m_pFile;
  m_pFile = NULL;
  return 0;
}

int CRWFile::fWrite(int virtAddr, int size)
{
  int irs = 0;
  int ivA = virtAddr;
  int isz = size;

  if (isz < 0){
      printf("\n CRWFile:fWrite Error unexpected bytes to write %d",size);
      return -1;
    }
  else if (isz == 0)
    return 0;

  char *buf = User2System(ivA,isz);
  if (buf == NULL || buf == (char*)0)
    {
      printf("\n CRWFile:fWrite Error- user memory space empty");
      if (buf != NULL) delete buf;
      return -1;
    }
  
  irs = m_pFile->Write(buf,strlen(buf));
  if (irs <= 0)
    printf("\n fail to write");

  delete buf;
  return irs;
}


int CRWFile::fRead(int virtAddr,int size)
{
  int irs = 0;
  int ivA = virtAddr;
  int isz = size;

  char *buf = new char[isz +1];

  ASSERT(buf != NULL);

  int ibytesread = m_pFile->Read(buf,isz);
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
