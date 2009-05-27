#include "syscall.h"


int main()
{

	char filename[30];
	char buff[100];
	OpenFileId id;
	int kq;
  char filecreate[30];
  Write("Create File ...\n",20,ConsoleOutput);
  
  Read(filecreate,MaxFileLength,ConsoleInput);
  Create(filecreate);
	Write("\nChuong trinh \"cat\" \n",20,ConsoleOutput);
	Write("\nBan hay nhap ten file :",30,ConsoleOutput);
	kq = Read(filename,MaxFileLength,ConsoleInput);
	if(kq < 0)
	{
		Write("\nLoi : Chuoi ban vua nhap khong hop le ",50,ConsoleOutput);		
		Exit(0);
	}
  
	id = Open(filename,0);
  
	if(id < 0)
	{
		Write("\nTen file nay ko co ",50,ConsoleOutput);
		Exit(0);
	}
	
	Write("\nNoi dung cua file ",50,ConsoleOutput);
	Write(filename ,30,ConsoleOutput);
	Write(" la :",10,ConsoleOutput);
	kq = 1;  
	while(kq > 0)
	{
		kq  = Read(buff,10,id);//doc tu file
		Write(buff,10,ConsoleOutput);//xuat ra man hinh
    if(kq < 10)
      break;
  
  }
	Close(id);
	Exit(0);
    
    return 1;
} 
