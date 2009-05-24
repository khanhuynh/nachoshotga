
#include "FileTable.h"
#include "scexception.h"

CFileTable:: CFileTable(int size)
{
  ASSERT (size >= 2);

  m_bm = NULL;
  m_bm = new BitMap(size);
  ASSERT(m_bm != NULL);

  m_iSize = size;

  for (int i =0 ; i < size; i++)
  {
      m_pFile[i] = NULL;
  }
}

CFileTable::~CFileTable()
{
  for (int i = 0 ; i < m_iSize; i++)
  {
      if (m_bm->Test(i))
	    {
	      delete m_pFile[i];
	      m_pFile[i] = NULL;
	    }
  }
  delete m_bm;
  m_iSize = 0;
}



int CFileTable::fOpen(int iType, int fID, OpenFile *f)
{
  CFile file(f);

  switch(iType)
  {
    case RW:
      m_pFile[fID] = &file;
      m_pFile[fID]->PutType(RW);
      break;

    case R:
      m_pFile[fID] = &file;
      m_pFile[fID]->PutType(R);
      break;

    default:
      printf("\nError: Unknow file type %d", iType);
      break;
  }

  m_bm->Mark(fID);

  return fID;
}

int CFileTable::FindFreeSlot()
{
  return (m_bm->Find());
}

int CFileTable::GetSize()
{
  return (m_iSize);
}

bool   CFileTable::IsExist(int id)
{
  return m_bm->Test(id);
}

int   CFileTable::fRead(int iVirAddr, int iSize, int fID)
{
	int iRs = 0;
  	int m_iVirtAddr = iVirAddr;
  	int m_iSize = iSize;
  	int m_id = fID;
 
  	iRs = m_pFile[m_id]->fRead(m_iVirtAddr,m_iSize);
  	return iRs;
}
