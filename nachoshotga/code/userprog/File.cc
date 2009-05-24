#include "File.h"
#include "scexception.h"

CFile::CFile(OpenFile *file)
{
  m_pFile = file;
}

CFile::~CFile()
{
  if(m_pFile != NULL)
  {
    delete m_pFile;
  }
}

int CFile::GetType()
{
  return m_iType;
}

void CFile::PutType(int iType)
{
  m_iType = iType;
}
int   CFile::fRead(int iVirAddr, int iSize)
{
	int addr = iVirAddr;
  	int si = iSize;
  	char *buf = new char[si + 1];

  	ASSERT(buf != NULL);

 	 int rs = gSynchConsole->Read(buf,si);
  
 	
 	 if (rs == si && buf[rs] != 10)
	{
   	 	char temp[11];
  	 	 int len =  0;
  	  	do
		{
   		   len = gSynchConsole->Read(temp,10);
    		}  while (len == 10 && temp[len-1] != 10);
  	}

  	if (rs < 0)
	{
  	  printf("fRead Error- SynchConsole read fail!..");
   	 delete buf;
   	 return -1;
  	}

  	int rsl = System2User(addr,rs,buf);
 	 if (rsl < 0)
    	  printf("\n Read:Error write to user space");

  
 	 delete buf;
 	 return rsl; 
}
