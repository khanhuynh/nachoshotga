#include "syscall.h"


int main()
{

	char fileName[30];
	char buff[100];
	OpenFileId id;
  int iR;
  char fileCreate[30];
  Write("\nChuong trinh demo system call : ",32,ConsoleOutput);
  Write("SC_Create, SC_Open, SC_Read, SC_Write, SC_Close, SC_Exec, SC_Exit \n",80,ConsoleOutput);


  Write("\n1. SC_Create ",20,ConsoleOutput);
  Write("Nhap vao ten file :\n",30,ConsoleOutput);
  Read(fileCreate,MaxFileLength,ConsoleInput);
  Create(fileCreate);
  
  Write("\n2. SC_Open, SC_Read, SC_Write, SC_Close ",40,ConsoleOutput);
  Write("\nBan hay nhap ten file :",30,ConsoleOutput);
	 iR = Read(fileName,MaxFileLength,ConsoleInput);
	 if(iR < 0)
	 {
		Write("\nLoi ! ",10,ConsoleOutput);		
	 }
   else
   {
     
	    id = Open(fileName,RO);

	    if(id < 0)
	    {
		    Write("\nTen file nay ko co ",50,ConsoleOutput);
		
	    }
      else
      {
	      Write("\nNoi dung cua file ",50,ConsoleOutput);
	      Write(fileName ,30,ConsoleOutput);
	      Write(" la :",10,ConsoleOutput);
	      while(iR > 0)
	      {
		      iR  = Read(buff,10,id);//doc tu file
          if(iR < 10)
            break;
		      Write(buff,10,ConsoleOutput);//xuat ra man hinh
        }
        Close(id);
      }
   } 
     
  Write("\n3. SC_Exec",20,ConsoleOutput);
  Write("\nBan hay nhap ten file muon exec :",40,ConsoleOutput);
	iR = Read(fileName,MaxFileLength,ConsoleInput);
  if(iR < 0)
	 {
		Write("\nLoi ! ",50,ConsoleOutput);
		Exit(0);
	 }
  id = Exec(fileName);
  Join(id);


  Write("\n Het !",10,ConsoleOutput);
  Exit(0);  
    return 1;
} 
