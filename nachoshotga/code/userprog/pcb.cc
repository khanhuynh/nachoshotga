// pcb.cpp: implementation of the pcb class.
//
//////////////////////////////////////////////////////////////////////

#include "pcb.h"
#include "system.h"


void ProcessFunc(int pID)
{
  char *filename= pTab->GetName(pID);
	AddrSpace *space= new AddrSpace(filename);
	if(space == NULL)
	{
		printf("\nLoi: Khong du bo nho de cap phat cho tien trinh !!!\n");
		return;
	}
	currentThread->space = space;

	space->InitRegisters();		// set the initial register values
	space->RestoreState();		// load page table register

	machine->Run();			// jump to the user progam
	ASSERT(FALSE);			// machine->Run never returns;
						// the address space exits
						// by doing the syscall "exit"
}

//constuctor
PCB::PCB()
{
	this->pid = 0;
	this->parentID = -1;
	this->numwait = 0;
	this->exitcode = 0;
	this->thread = NULL;
	this->joinsem = new Semaphore("joinsem",0);
	this->exitsem = new Semaphore("exitsem",0);
	this->mutex = new Semaphore("mutex",1);
}

PCB::PCB(int id)
{
  if(id != 0)
  {
	  parentID = currentThread->processID;
  }
  else
  {
     parentID = 0;
  }
   
	this->pid = id;
	this->numwait = 0;
	this->exitcode = 0;
	this->thread = NULL;
	this->joinsem = new Semaphore("joinsem",0);
	this->exitsem = new Semaphore("exitsem",0);
	this->mutex = new Semaphore("mutex",1);
}

//deconstructor
PCB::~PCB()
{
	//IntStatus oldLevel = interrupt->SetLevel(IntOff);
	//printf("\n~PCB : Huy tien trinh con");
	//printf("\ncurrentThread->ID = %d",currentThread->processID);
	//printf("\ncurrentThread->name = %s",currentThread->getName());

	if(joinsem != NULL)
		delete this->joinsem;
	if(exitsem != NULL)
		delete this->exitsem;
	if(mutex != NULL)
		delete this->mutex;
	if(thread != NULL)
	{
		//printf("\n~PCB : free and finish");
		thread->FreeSpace();
		thread->Finish();
	}
	  // (void) interrupt->SetLevel(oldLevel);
}


int PCB::GetNumWait()
{
	return this->numwait;
}




void PCB::JoinWait()
{
  IncNumWait();
	joinsem->P();
}

void PCB::JoinRelease()
{
	joinsem->V();
  DecNumWait();
}


void PCB::ExitWait()
{
	exitsem->P();
}

void PCB::ExitRelease()
{
	exitsem->V();
}


void PCB::IncNumWait()
{
	mutex->P();
	++numwait;
	mutex->V();
}
                               
void PCB::DecNumWait()
{
	mutex->P();
	if(numwait > 0)
		--numwait;
	mutex->V();
}


char* PCB::GetThreadName()
{
	return thread->getName();
}


//nap chuong trinh co ten luu trong bien file name va ProcessID la id
int PCB::Exec(char* filename,int id)
{
	mutex->P();
	
	//this->pid = id;
	this->thread = new Thread(filename);
	if(this->thread == NULL)
	{
		printf("\nPCB::Exec : Khong the tao 1 mot tien trinh ");
		return -1;
	}

	this->thread->processID = id;
	this->parentID = currentThread->processID;

	this->thread->Fork(ProcessFunc,id);

	mutex->V();

	return id;

}
