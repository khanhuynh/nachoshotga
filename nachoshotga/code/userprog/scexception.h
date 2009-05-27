#ifndef SCEXCEPTION_H
#define SCEXCEPTION_H

#include "../userprog/syscall.h"
#include "system.h"


char* User2System(int iVirtAddr,int iLimit);
int   System2User(int iVirtAddr,int iLen,char* buffer);
void IncreaseProgramCounter();
int doSC_Create();
int doSC_Open(); 
int doSC_Read();
int doSC_Write();
int doSC_Close();

int doSC_CreateSemaphore();
int doSC_Exec();
int doSC_Join();
int doSC_Exit();
int doSC_Wait();
int doSC_Signal();

#endif
