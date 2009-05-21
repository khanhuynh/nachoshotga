#ifndef SCEXCEPTION_H
#define SCEXCEPTION_H

#include "../userprog/syscall.h"
#include "system.h"


char* User2System(int iVirtAddr,int iLimit);
int   System2User(int iVirtAddr,int iLen,char* buffer);
void IncreaseProgramCounter(); 

#endif
