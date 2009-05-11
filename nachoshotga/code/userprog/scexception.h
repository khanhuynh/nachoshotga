#ifndef SCEXCEPTION_H
#define SCEXCEPTION_H

#include "../userprog/syscall.h"
#include "system.h"
#define MaxFileLength 32

char* User2System(int iVirtAddr,int iMaxFileLen);
int   System2User(int iVirtAddr,int iLen,char* buffer);


#endif
