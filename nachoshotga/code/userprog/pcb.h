#ifndef PCB_H
#define PCB_H

#include "thread.h"
#include "synch.h"
#include "utility.h"
#include "../userprog/syscall.h"

class PCB{
 private:
  Semaphore *joinsem;
  Semaphore *exitsem;
  Semaphore *mutex;
  int        exitcode; 
  Thread    *thread;
  int        pid;
  int        numwait;

 public: 
  int        parentID;
  PCB();
  PCB(int id);
  ~PCB();
  int Exec(char *filename,int pid);
  int GetID() {return pid;}
  int GetNumWait();
  void JoinWait();
  void ExitWait();
  void JoinRelease();
  void ExitRelease();
  void IncNumWait();
  void DecNumWait();
  void SetExitCode(int ec){exitcode = ec;}
  int  GetExitCode(){return exitcode;}
  char* GetThreadName();
};
#endif
