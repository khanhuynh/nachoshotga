#include "ptable.h"
#include "system.h"

//khoi tao size doi tuong pcb de luu size process. Gán giá tri ban dau là null.
//khoi tao bm va bmsem de su dung
PTable::PTable(int size){
  
  if (size > MAXPROCESS)
    size = MAXPROCESS;

  bm = new BitMap(size );
  ASSERT(bm != NULL);

  //ko su dung 0 vi dday la cha cua tat ca
  bm->Mark(0);

  psize = size;
  for (int i = 0; i < psize ; i++)
    pcb[i] = NULL;
 
  bmsem = new Semaphore("bitmapAccess",1);
}

//huy cac doi tuong da tao
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

  if (filename == NULL)
    {
      printf("\n Not enough memory in system");
      DEBUG(dbgFile,"\n Not enough memory in system");
      delete filename;
      return -1;
    }


  //Kiem tra ten dai
  if (strlen(filename) == 0 || (strlen(filename) >= MaxFileLength+1))
    {
      printf("\n Too many characters in filename: %s",filename);
      DEBUG(dbgFile,"\n Too many characters in filename");
      delete filename;
      return -1;
    }

  //Kiem tra coi file co ton tai ko
  OpenFile* executable = fileSystem->Open(filename);

  if (executable == NULL){
    printf("\nPtable::Error opening file:  %s",filename);
    DEBUG(dbgFile,"\n Error opening file.");
    delete filename;
    return -1;
  }

  delete executable;

  
  // Ko cho phep no gio chinh no
  if (strcmp(currentThread->getName(),filename) == 0){
    printf("\nPTable:Error do NOT support a process execute itself");
    delete filename;
    return -1;
  }
  

  int pid = this->GetFreeSlot();    // Tim slot trong

  if (pid == -1){// no empty slot
    printf("\nPTableError:: No empty slot.");
    return -1;
  }

  // cap phat 1 block moi
  pcb[pid] = new PCB(pid);
  pcb[pid]->parentID = currentThread->processID;


  int rs = pcb[pid]->Exec(filename,pid);
  
  //  delete filename;
  //printf("\nPTable:ExecUpdate numwait %d",pcb[pid]->GetNumWait());
  if (rs == -1)// loi
    pid = -1;

  return pid;
}


int PTable::ExitUpdate(int exitcode)
{
   
  
  int pid = currentThread->processID;

  // kiem tra ton tai
  if(!IsExist(pid)){
    printf("\nPTable::ExitUpdate unexpected process id %d",pid);
    return -1;
  }

  //
  if(pid == 0)// neu la tien trinh chinh (cho cua tat ca
  {  
    interrupt->Halt();
    return 0;
  }

   
   
   pcb[pid]->SetExitCode(exitcode);
   // Neu co tien trinh cha doi no
  if(pcb[pid]->GetNumWait() > 0)
  { 
    //printf("\nPTable:Chui vao");
	  pcb[pid]->JoinRelease();       // releasse cho tien trinh cha

	  pcb[pid]->ExitWait();           // Xin phep thoat
    pcb[pid]->DecNumWait();
  } 

   //printf("\nPTable:ExitUpdate exit process id %d",pid);
  Remove(pid);         // Xoa khoi pcb

 

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


  if(pcb[pid] == NULL){//not enough main memory 
    interrupt->SetLevel(oldLevel);
    return -1;
  }

  if(currentid != pcb[pid]->parentID){
    printf("\nPtable:Join: Process %d is not a child of calling process %d",pid,currentid);
    interrupt->SetLevel(oldLevel);
    return -1;
  }
  

  pcb[pid]->IncNumWait();

  pcb[pid]->JoinWait();

  int exitcode = pcb[pid]->GetExitCode();

  pcb[pid]->ExitRelease();


  interrupt->SetLevel(oldLevel);
  // Tra ve trang thai ban dau

  return (exitcode);
}
//tim free slot de luu thong tin cho tien trinh moi
int PTable::GetFreeSlot(){
  bmsem->P();
  return (bm->Find());
  bmsem->V();
}

int PTable::GetMax()
{
  return psize;
}

// Kt xem id co ton tai?
bool PTable::IsExist(int pid){

  return bm->Test(pid);
}

//khi tien trinh ket thuc , xoa 1 processID ra khoi mang quan ly no
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
