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
