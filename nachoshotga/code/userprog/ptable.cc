#include "ptable.h"
#include "system.h"


PTable::PTable(int size){
  
  if (size > MAXPROCESS)
    size = MAXPROCESS;

  bm = new BitMap(size );
  ASSERT(bm != NULL);

  //do not use index 0
  bm->Mark(0);

  psize = size;
  for (int i = 0; i < psize ; i++)
    pcb[i] = NULL;
 
  bmsem = new Semaphore("bitmapAccess",1);
}

PTable::~PTable()
{
  for (int i =0 ; i < psize; i++)
    {
      if (bm->Test(i))
        {
          delete pcb[i];
          pcb[i] = NULL;
        }
    }
  delete bm;
  psize = 0 ;
  delete bmsem;
}

int PTable::ExecUpdate(char* filename)
{
  // Read process name
  //int virtAddr = machine->ReadRegister(4);

  //char *filename = User2System(virtAddr,MaxFileLength+1);

  if (filename == NULL)
    {
      printf("\n Not enough memory in system");
      DEBUG(dbgFile,"\n Not enough memory in system");
      delete filename;
      return -1;
    }

  //debug
  //  printf("\nPtable:exec process : %s",filename);

  //Check name too long!
  if (strlen(filename) == 0 || (strlen(filename) >= MaxFileLength+1))
    {
      printf("\n Too many characters in filename: %s",filename);
      DEBUG(dbgFile,"\n Too many characters in filename");
      delete filename;
      return -1;
    }

  //Check filename exist
  OpenFile* executable = fileSystem->Open(filename);

  if (executable == NULL){
    printf("\nPtable::Error opening file:  %s",filename);
    DEBUG(dbgFile,"\n Error opening file.");
    delete filename;
    return -1;
  }

  delete executable;

  //Check if call itself
  //  printf("\n current pro '%s'",currentThread->getName());
  
  if (strcmp(currentThread->getName(),filename) == 0){
    printf("\nPTable:Error do NOT support a process execute itself");
    delete filename;
    return -1;
  }
  

  // Find free slot on ptable
  int pid = this->GetFreeSlot();    // Tim slot trong

  if (pid == -1){// no empty slot
    printf("\nPTableError:: No empty slot.");
    return -1;
  }

  // Allocate new pcb
  pcb[pid] = new PCB(pid);
  pcb[pid]->parentID = currentThread->processID;
  //debug
  //printf("\nPtable: process ID: %d",pid);

  int rs = pcb[pid]->Exec(filename,pid);
  
  //  delete filename;

  if (rs == -1)// error
    pid = -1;

  return pid;
}


int PTable::ExitUpdate(int exitcode)
{
  // get process ID
  int pid = currentThread->processID;
  // check it exist
  if(!IsExist(pid)){
    printf("\nPTable::ExitUpdate unexpected process id %d",pid);
    return -1;
  }

  //
  if(pid == 0)// the very parent process.
  {
    interrupt->Halt();
    return 0;
  }

  // printf("\nPTable:ExitUpdate exit process id %d",pid);
  pcb[pid]->SetExitCode(exitcode);

	pcb[pid]->JoinRelease();

	pcb[pid]->ExitWait();
  pcb[pcb[pid]->parentID]->DecNumWait();

  
  Remove(pid);

  printf("\nPTable:ExitUpdate exit process id %d",pid);

  return 0;
}

int PTable::JoinUpdate(int pid)
{
  // Tat cac ngat va luu lai hien trang truoc do
  IntStatus oldLevel = interrupt->SetLevel(IntOff);

  int currentid = currentThread->processID;
  
  if (currentid == pid){
    printf("\n Can't join to yourself or main thread");
    interrupt->SetLevel(oldLevel);
    return -1;
  }

//  gmutex->P();    Cai nay la cai gi ????
  //
  if(pcb[pid] == NULL){//not enough main memory 
    interrupt->SetLevel(oldLevel);
    return -1;
  }

  if(currentid != pcb[pid]->parentID){
    printf("\nPtable:Join: Process %d is not a child of calling process %d",pid,currentid);
    interrupt->SetLevel(oldLevel);
    return -1;
  }
  
  //debug
  //printf("\nBegin Join p%d",pid);

  pcb[pid]->IncNumWait();

  //printf("\n2");

  pcb[pid]->JoinWait();

  //printf("\n3");

  int exitcode = pcb[pid]->GetExitCode();

  pcb[pid]->ExitRelease();

  //printf("\nF ExitRelease");
  interrupt->SetLevel(oldLevel);

  //debug
  //printf("\nFinish Join p%d",pid);

  return (exitcode);
}

int PTable::GetFreeSlot(){
  bmsem->P();
  return (bm->Find());
  bmsem->V();
}

int PTable::GetMax()
{
  return psize;
}

bool PTable::IsExist(int pid){
 // if (pid < 0 || pid > bm->Size())
 //  return false;
  return bm->Test(pid);
}

void PTable::Remove(int pid){
  //  printf("\nR0");
  bmsem->P();
  //printf("\nR1");
  bm->Clear(pid);
  //printf("\nR2");
  delete pcb[pid];
  //printf("\nR3");
  pcb[pid] = NULL;
  bmsem->V();
}

char* PTable::GetName(int pID)
{
	if(bm->Test(pID))
		return pcb[pID]->GetThreadName();
}
