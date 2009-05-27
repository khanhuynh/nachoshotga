
#include "FileTable.h"
#include "scexception.h"

CFileTable::CFileTable()
{
  m_bm= new BitMap(MAX);
	m_pIO[CIN] = new CCin();
	m_bm->Mark(CIN);
	m_pIO[COUT] = new CCout();
	m_bm->Mark(COUT);
}

CFileTable:: CFileTable(int size)
{
  ASSERT (size >= 2 && size <= 10);

  m_bm = NULL;
  m_bm = new BitMap(size);
  ASSERT(m_bm != NULL);

  m_iSize = size;

  for (int i =0 ; i < size; i++)
  {
      m_pIO[i] = NULL;
  }
  
  m_pIO[CIN] = new CCin();
	m_bm->Mark(CIN);                      
	m_pIO[COUT] = new CCout();
	m_bm->Mark(COUT);
}

CFileTable::~CFileTable()
{
  for (int i = 0 ; i < m_iSize; i++)
  {
      if (m_bm->Test(i))
	    {
	      delete m_pIO[i];
	      m_pIO[i] = NULL;
	    }
  }                           
  delete m_bm;
  m_iSize = 0;
}



int CFileTable::fOpen(int iType, int fID, OpenFile *f)
{
  CRWFile  *rw;
  CROFile  *ro;

  switch(iType)
    {
    case RW:
      rw = new CRWFile(f);
      m_pIO[fID] = (CIOBase*) rw;
      break;
    case RO:
      ro = new CROFile(f);
      m_pIO[fID] = (CIOBase*) ro;
      break;

    default:
      printf("\n CFileTable: Ko xac dinh duoc loai file %d",iType);
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
    if(IsExist(fID) == FALSE)
      return -1;
 
  	int iRs = m_pIO[fID]->fRead(iVirAddr,iSize);
  	return iRs;
}

int   CFileTable::fWrite(int iVirAddr, int iSize, int fID)
{
    if(IsExist(fID) == FALSE)
      return -1;

  	int iRs = m_pIO[fID]->fWrite(iVirAddr,iSize);
  	return iRs;
}

int  CFileTable:: fClose(int fID)
{
  if(m_bm->Test(fID) == FALSE)
  {
    printf("\n CFileTable : fClose : Ko xac dinh duoc ID");
    return -1;
  }
  int iRs = m_pIO[fID]->fClose();
  if(fID > 1 && fID < m_iSize)
    m_bm->Clear(fID);
  return iRs;  
}
