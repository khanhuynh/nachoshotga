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
	int iRs = 0;
	int m_iVirAddr = iVirAddr;
	int m_iSize = iSize;

	char *cBuf = new char[m_iSize +1];

	ASSERT(cBuf != NULL);

	int iBytesRead = m_pFile->Read(cBuf,m_iSize);

	if (iBytesRead < 0)
	{
		printf("\n Doc bi loi");
    
    		delete cBuf;
    		return -1;
  	}
  	else if (iBytesRead == 0)
    	{
     
     	 delete cBuf;
     	 return 0;
    	}

  	iRs = System2User(m_iVirAddr,iBytesRead,cBuf);

  
  	delete cBuf;
  	return iRs;
}	
