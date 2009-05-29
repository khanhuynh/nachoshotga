#ifndef SCEXCEPTION_H
#define SCEXCEPTION_H

#include "../userprog/syscall.h"
#include "system.h"

//Copy buffer tu khong gian vung nho nguoi dung sang khong gian vung nho he thong 
char* User2System(int iVirtAddr,int iLimit);

//Copy buffer tu khong gian nho he thong sang khong gian nho nguoi dung
int   System2User(int iVirtAddr,int iLen,char* buffer);

// Ham tang thanh ghi PC
void IncreaseProgramCounter();

//su ly su kien SC_Create khi System goi 
int doSC_Create();

// mo file
int doSC_Open(); 

//doc tu file hay console
int doSC_Read();

//write to file or console
int doSC_Write();

//dong file
int doSC_Close();

//tao semaphore
int doSC_CreateSemaphore();

//Thuc thi 1 chuong trinh moi trong system thread
int doSC_Exec();

// tham gia tien trinh
int doSC_Join();

//kiem tra chuong trinh co hoan thanh hay ko ( = 0 la hoan thanh va nguoc lai)
int doSC_Exit();

//semaphore wait
int doSC_Wait();

//semaphore can signal
int doSC_Signal();

#endif
