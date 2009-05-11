// Syscall handle file: process all syscall exception except SC_Halt
// Begin DU~NG TRUNG TRAN

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
  iPc+ = 4;
  machine->WriteRegister(NextPCReg, iPc);
}



