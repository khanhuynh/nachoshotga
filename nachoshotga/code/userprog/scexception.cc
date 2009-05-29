
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
    machine->WriteRegister(2, -1);
    return -1;
  }

  int fID = currentThread->fTable->FindFreeSlot();

  if(fID < 0)
  {
    printf("No free slot\n");
    machine->WriteRegister(2, -1);
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
    printf("\nCannot open file %s", FileName);
    machine->WriteRegister(2, -1);
    delete FileName;
    return -1;
  }

  int iFileID = currentThread->fTable->fOpen(iType, fID, f);
  machine->WriteRegister(2, iFileID);

  return iFileID;
}


/*
Input: User space address = reg4, buffer size = reg5, OpenfileID = reg6
Output: -1: loi
Purpose: doc tu file hay console
*/

int doSC_Read()
{
	int iVirAddr = machine->ReadRegister(4);

  int iSize = machine->ReadRegister(5);
  int id = machine->ReadRegister(6);
  
  if (iSize <= 0)
    {
      printf("\nError: unexpected buffer size: %d",iSize);
      machine->WriteRegister(2, -1);
      return -1;
    }

  if (id < 0 || id >= currentThread->fTable->GetSize())
    {
      printf("\n ReadError: Unexpected file id: %d",id);
      machine->WriteRegister(2, -1);
      return -1;
    }
  if (!currentThread->fTable->IsExist(id)){
    printf("\n ReadError: reading file id %d is not opened",id);
    machine->WriteRegister(2, -1);
    return -1;
  }

  int rs = currentThread->fTable->fRead(iVirAddr,iSize,id);
  
  machine->WriteRegister(2,rs);

  return rs;
}

/*
Input: User space address = reg4, buffer size= reg5, OpenFileID = reg6
Output: = -1 - loi hay so bytes da duoc viet
        
Purpose: do write to file or console
*/

int doSC_Write()
{
  int iVirAddr = machine->ReadRegister(4);
  int iSize = machine->ReadRegister(5);
  int id = machine->ReadRegister(6);

  if (iSize < 0)
    {
      printf("\nSC_Write: Loi size < 0: %d",iSize);
      machine->WriteRegister(2, -1);
      return -1;
    }
  else if (iSize == 0)
  {
    machine->WriteRegister(2, 0);
    return 0;
  }

  if (id < 0 || id >= currentThread->fTable->GetSize())
    {
      printf("\n WriteError: Ko xac dinh duoc id: %d",id);
      machine->WriteRegister(2, -1);
      return -1;
    }
  if (!currentThread->fTable->IsExist(id)){
    printf("\n WriteError: file id %d chua mo",id);
    machine->WriteRegister(2, -1);
    return -1;
  }

  int rs = currentThread->fTable->fWrite(iVirAddr,iSize,id);

  machine->WriteRegister(2,rs);

  return rs;
}


/*
Input: OpenfileID = reg4
Output: 0- thanh cong , -1 - that bai
Purpose: dong file
*/

int doSC_Close()
{
  int id = machine->ReadRegister(4);
  if (id < 0 || id >= currentThread->fTable->GetSize())
    {
      printf("\n Close: Khong xac dinh duoc id: %d",id);
      machine->WriteRegister(2, -1);
      return -1;
    }
  if (!currentThread->fTable->IsExist(id)){
    printf("\n Close: File id %d chua duoc mo",id);
    machine->WriteRegister(2, -1);
    return -1;
  }

  currentThread->fTable->fClose(id);
  machine->WriteRegister(2, 0);
  return 0;
}

/*
Input: reg4 - ten file
Output:
Purpose: su ly su kien SC_Create khi System goi   
*/

int doSC_Create()
{
	int virtAddr;
  char* filename;

  virtAddr = machine->ReadRegister(4);
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

/*
input : name : ten cua bien semaphore
	iSemval: gia tri ban dau 
output : 0 : thanh cong
	 -1 : loi
		+ name = NULL
*/
int doSC_CreateSemaphore()
{
	int iAddr = machine->ReadRegister(4);
	int iSemval = machine->ReadRegister(5);
	char *name = User2System(iAddr,32);
	if(name == NULL)
	{
		printf("\nSC_Semaphore :: Name = NULL");
		machine->WriteRegister(2,-1);
		return -1;
	}
	int kq = sem->create(name,iSemval);
	if(kq < 0)
	{
		printf("\nSC_Semaphore :: Khong the tao semaphore : %s",name);
		machine->WriteRegister(2,-1);
		return -1;
	}
	machine->WriteRegister(2,0);
  return 0;
}

/*
 Thuc thi 1 chuong trinh moi trong system thread
input :   ten cua file chuong trinh
output :    loi   -1 
          thanh cong : SpaceID 
*/
int doSC_Exec()
{
	int iAddr = machine->ReadRegister(4);
	char* fName = User2System(iAddr,MaxFileLength+1);
	
	int ID = pTab->ExecUpdate(fName); //tra ve id cua tien trinh con hoac -1 neu loi

	machine->WriteRegister(2,ID);
	return ID;
}

/*
------------------------------------------------------------
  Join vao tien trinh cha co id la pID
input : id cua tien trinh cha

output : 0 neu thanh cong
          
------------------------------------------------------------
*/
int doSC_Join()
{
	int pID = machine->ReadRegister(4);

	int iExitcode = pTab->JoinUpdate(pID);

	machine->WriteRegister(2,iExitcode);

  return iExitcode;

}

/*
------------------------------------------------------------
void Exit(int exitCode)
intput : exitcode = 0 : chuong trinh hoan thanh thanh cong
		 exitcode != 0 : nguoc lai
------------------------------------------------------------
*/
int doSC_Exit()
{
  	int iExitStat = machine->ReadRegister(4);
 
  	// neu tien trinh thoat ra vi loi
	if (iExitStat != 0)
  {
    		printf("\nTien trinh %s thoat bi loi : %d",currentThread->getName(),iExitStat);
        return -1;
      }

	int iRs = pTab->ExitUpdate(iExitStat);
  // giai phong tai nguyen
	currentThread->FreeSpace();
	currentThread->Finish();
  return iRs;
}

/*
  -----------------------------------------------------
  Doi
input :  ten cua semaphore can wait
output : 0 : thanh cong
	        -1 : loi
  -----------------------------------------------------
*/

int doSC_Wait()
{
	int iAddr = machine->ReadRegister(4);
	char *Name = User2System(iAddr,32);
	if(Name == NULL)
	{
		printf("\nSC_Wait :: name = NULL");
		machine->WriteRegister(2,-1);
		return -1;
	}
	int iRs = sem->wait(Name);
	if(iRs < 0)
	{
		printf("\nSC_Wait :: Can't wait semaphore : %s",Name);
		machine->WriteRegister(2,-1);
		return -1;
	}
	machine->WriteRegister(2,0);
  return 0;
}


/*
  -----------------------------------------------------
int signal(char* name)
input : name : ten cua semaphore can signal
output : 0 : thanh cong
	-1 : loi
  -----------------------------------------------------
*/
int doSC_Signal()
{
	int iVirAddr = machine->ReadRegister(4);
	char *Name = User2System(iVirAddr,32);
  
	if(Name == NULL)
	{
		printf("\nSC_Signal :: Ten NULL");
		machine->WriteRegister(2,-1);
		return -1;
	}
	int iRs = sem->signal(Name);
	if(iRs != 0)
	{
		printf("\nSC_Signal :: Ko the signal : %s",Name);
		machine->WriteRegister(2,-1);
		return -1;
	}
	machine->WriteRegister(2,0);
  return 0;
}
