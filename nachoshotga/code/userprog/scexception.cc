// Syscall exception file: process all syscall exception except SC_Halt


#include "scexception.h"


////////////////////////////////////////////////////////////////////////////////////
/*
Input: - Dia chi khong gian nguoi dung (int)
       - Gioi han cua buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer tu khong gian vung nho nguoi dung sang khong gian vung nho he thong 
*/

char* User2System(int iVirtAddr,int iLimit)
{
  int i;  // chi so
  int iOneChar;
  char* kernelBuf = NULL;
  
  kernelBuf = new char[iLimit +1];  //them ky tu ket thuc chuoi
  if (kernelBuf == NULL)            // Kiem tra viec cap phat cho kernelBuf
    return kernelBuf;               // return NULL

  memset(kernelBuf, 0, iLimit+1);   // Set cho tat ca cac phan tu trong kernelBuf ve 0
  
  //printf("\n Filename u2s:");
  for (i = 0 ; i < iLimit ;i++)
    {
      // Thuc hien doc tu vung nho dia chi iVirtAddr vao kernelBuf tung ki tu
      machine->ReadMem(iVirtAddr+i,1,&iOneChar);
      kernelBuf[i] = (char)iOneChar;
      //printf("%c",kernelBuf[i]);
      if (iOneChar == 0)
	      break;
    }

    return kernelBuf;
}



/*
Input: - Dia chi khong gian nguoi dung (int)
       - gioi han buffer (int)
       - Buffer (char[])
Output:- So byte da copy (int)
Purpose: Copy buffer tu khong gian nho he thong sang khong gian nho nguoi dung
*/
int System2User(int iVirtAddr,int iLen,char* buffer)
{
  if (iLen < 0)
    {
      return -1;
    }
  if (iLen == 0)
    {
      return iLen;
    }
  int i = 0;
  int iOneChar = 0 ;
  do{
    // Thuc hien viec doc buffer va ghi len vung nho user
    iOneChar= (int) buffer[i];
    machine->WriteMem(iVirtAddr+i, 1, iOneChar);
    i ++;
  }while(i < iLen && iOneChar != 0);

  return i;  // Tra ve so ky tu doc duoc
}

// Ham tang thanh ghi PC
void IncreaseProgramCounter()  
{
  int iPc;
  iPc = machine->ReadRegister(PCReg);
  machine->WriteRegister(PrevPCReg, iPc);
  iPc = machine->ReadRegister(NextPCReg);
  machine->WriteRegister(PCReg, iPc);
  iPc += 4;
  machine->WriteRegister(NextPCReg, iPc);
}

/*
Input:  - Ten file
        - Loai file
          0: file doc ghi
          1: file chi doc
Output: - fail: -1
          OK: OpenFileID
*/

int doSC_Open()
{
  int iVirtAddr = machine->ReadRegister(4);
  int iType = machine->ReadRegister(5);

  if(iType < 0 || iType > 1)
  {
    printf("\nUnknow file type %d", iType);
    return -1;
  }

  int fID = currentThread->fTable->FindFreeSlot();

  if(fID < 0)
  {
    printf("No free slot\n");
    return -1;
  }

  char *FileName = User2System(iVirtAddr, MaxFileLength + 1);

  if((FileName == NULL) || (strlen(FileName) == 0) || (strlen(FileName) >= MaxFileLength + 1))
  {
    printf("\nNot enough memory of too many characters in filename");
    machine->WriteRegister(2, -1);
    delete FileName;
    return -1;
  }

  OpenFile *f = fileSystem->Open(FileName);

  if(f == NULL)
  {
    printf("Cannot open file %s", FileName);
    machine->WriteRegister(2, -1);
    delete FileName;
    return -1;
  }

  int iFileID = currentThread->fTable->fOpen(iType, fID, f);
  machine->WriteRegister(2, iFileID);

  return iFileID;
}

int doSC_Read()
{
	int virtAddr = machine->ReadRegister(4);

  int size = machine->ReadRegister(5);
  int id = machine->ReadRegister(6);
char* name = User2System(virtAddr,size);
  
  if (size <= 0)
    {
      printf("\nError: unexpected buffer size: %d",size);
      return -1;
    }

  if (id < 0 || id >= currentThread->fTable->GetSize())
    {
      printf("\n ReadError: Unexpected file id: %d",id);
      return -1;
    }
  if (!currentThread->fTable->IsExist(id)){
    printf("\n ReadError: reading file id %d is not opened",id);
    return -1;
  }

  int rs = currentThread->fTable->fRead(virtAddr,size,id);
  
  machine->WriteRegister(2,rs);

  return rs;
}

int doSC_Create()
{
	int virtAddr;
  	char* filename;

    
  DEBUG(dbgFile,"\n SC_Create call ...");
  DEBUG(dbgFile,"\n Doc tap tin tu dia chi ao");
  
  // check for exception
  virtAddr = machine->ReadRegister(4);
  DEBUG (dbgFile,"\n Dang doc file.");
  filename = User2System(virtAddr,MaxFileLength+1);
  if (filename == NULL)
    {
      printf("\n Not enough memory in system");
      DEBUG(dbgFile,"\n Not enough memory in system");
      machine->WriteRegister(2,-1);
      delete filename;
      return -1;
    }
  
  if (strlen(filename) == 0 || (strlen(filename) >= MaxFileLength+1))
    {
      printf("\n Qua nhieu ki tu trong ten tap tin: %s",filename);
      DEBUG(dbgFile,"\n Qua nhieu ki tu trong ten tap tin");
      machine->WriteRegister(2,-1);
      delete filename;
      return -1;
    }

  //Hoan thanh viec doc ten tap tin
  
  DEBUG(dbgFile,"\n Doc ten tap xong!.");
  

  // Tao tap tin voi cai size = 0;

  if (!fileSystem->Create(filename,0))
    {
      printf("\n Loi tao file: '%s'",filename);
      delete filename;
      machine->WriteRegister(2,-1);
      delete filename;
      return -1;
    }
 

  machine->WriteRegister(2,0);

  delete filename;
  return 0;
}

void doSC_Semaphore()
{
	int iAddr = machine->ReadRegister(4);
	int iSemval = machine->ReadRegister(5);
	char *name = User2System(iAddr,32);
	if(name == NULL)
	{
		printf("\nSC_Semaphore :: name = NULL");
		machine->WriteRegister(2,-1);
		return;
	}
	int kq = sem->create(name,iSemval);
	if(kq < 0)
	{
		printf("\nSC_Semaphore :: khong the tao semaphore : %s",name);
		machine->WriteRegister(2,-1);
		return;
	}
	machine->WriteRegister(2,0);
}
