// addrspace.cc 
//	Routines to manage address spaces (executing user programs).
//
//	In order to run a user program, you must:
//
//	1. link with the -N -T 0 option 
//	2. run coff2noff to convert the object file to Nachos format
//		(Nachos object code format is essentially just a simpler
//		version of the UNIX executable object code format)
//	3. load the NOFF file into the Nachos file system
//		(if you haven't implemented the file system yet, you
//		don't need to do this last step)
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "addrspace.h"
#include "noff.h"
#ifdef HOST_SPARC
#include <strings.h>
#endif

//----------------------------------------------------------------------
// SwapHeader
// 	Do little endian to big endian conversion on the bytes in the 
//	object file header, in case the file was generated on a little
//	endian machine, and we're now running on a big endian machine.
//----------------------------------------------------------------------

static void 
SwapHeader (NoffHeader *noffH)
{
 // Chuyen tu Little Endian sang Big Endian ( vi MIPS la Big Endian)
	noffH->noffMagic = WordToHost(noffH->noffMagic);
	noffH->code.size = WordToHost(noffH->code.size);
	noffH->code.virtualAddr = WordToHost(noffH->code.virtualAddr);
	noffH->code.inFileAddr = WordToHost(noffH->code.inFileAddr);
	noffH->initData.size = WordToHost(noffH->initData.size);
	noffH->initData.virtualAddr = WordToHost(noffH->initData.virtualAddr);
	noffH->initData.inFileAddr = WordToHost(noffH->initData.inFileAddr);
	noffH->uninitData.size = WordToHost(noffH->uninitData.size);
	noffH->uninitData.virtualAddr = WordToHost(noffH->uninitData.virtualAddr);
	noffH->uninitData.inFileAddr = WordToHost(noffH->uninitData.inFileAddr);
}

//----------------------------------------------------------------------
// AddrSpace::AddrSpace
// 	Create an address space to run a user program.
//	Load the program from a file "executable", and set everything
//	up so that we can start executing user instructions.
//
//	Assumes that the object code file is in NOFF format.
//
//	First, set up the translation from program memory to physical 
//	memory.  For now, this is really simple (1:1), since we are
//	only uniprogramming, and we have a single unsegmented page table
//
//	"executable" is the file containing the object code to load into memory
//----------------------------------------------------------------------

AddrSpace::AddrSpace(OpenFile *executable)
{
    NoffHeader noffH;
    unsigned int i, j, size;
    unsigned int iNumCodePage, iNumDataPage; // So trang cua phan code va so trang cua phan data
    unsigned int iLastCodePageSize, iFirstDataPageSize, iRemainDataSize, iLastDataPageSize;
    // Doc Header
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    // Kiem tra xem du lieu co duoc luu theo Little Endian ko
    if ((noffH.noffMagic != NOFFMAGIC) && 
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

    
// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size 
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize); // 1 pagesize = 2*SectorSize
    size = numPages * PageSize;            // Kich thuoc ma ct se can

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory
            
    addrMutex->P();     // Truy xuat doc quyen gPhysPageBitMAp
    // Kiem tra xem voi so trang ct can, co con du bo nho vat ly trong khong
    if (numPages > gPhysPageBitMap->NumClear())
	  {
		  printf("\nAddrSpace:Load: not enough memory for new process..!");
		  numPages = 0;
		  delete executable;
		  addrMutex->V();
      return;
	  }        
    DEBUG('a', "Initializing address space, num pages %d, size %d\n", 
					numPages, size);
// first, set up the translation 
  /*  pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	pageTable[i].physicalPage = i;
	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on 
					// a separate page, we could set its 
					// pages to be read-only
    }
    
// zero out the entire address space, to zero the unitialized data segment 
// and the stack segment
    bzero(machine->mainMemory, size);  */
    // first, set up the translation
	pageTable = new TranslationEntry[numPages];
	for (i = 0; i < numPages; i++)
	{
		pageTable[i].virtualPage = i; // for now, virtual page # = phys page #
		pageTable[i].physicalPage = gPhysPageBitMap->Find();  // Tim trang con trong --> day la phan thay doi
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE; // if the code segment was entirely on a separate page, we could set its pages to be read-only

		// Xoa trang da chon duoc
		bzero(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]), PageSize);
		printf("Trang vat ly %d \n",pageTable[i].physicalPage);
	}
  addrMutex->V();
/*// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n", 
			noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n", 
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
    */
    //Tinh so trang cua phan code
	iNumCodePage = divRoundUp(noffH.code.size, PageSize);

    // Tinh kich thuoc trang cuoi cua phan code
	iLastCodePageSize = noffH.code.size - (iNumCodePage-1)*PageSize;
    // Tinh phan con lai cua phan data de phong truong hop trang cuoi phan code khong su dung het page
	iRemainDataSize = noffH.initData.size - (PageSize - iLastCodePageSize);
  // Tinh kich thuoc trang dau phan data de co the load vua du vao phan du cua trang code cuoi
	if (iRemainDataSize < 0)
	{
		iNumDataPage = 0;
		iFirstDataPageSize = noffH.initData.size;
	}
	else
	{
		iNumDataPage = divRoundUp(iRemainDataSize, PageSize);
		iLastDataPageSize = iRemainDataSize - (iNumDataPage-1)*PageSize;
		iFirstDataPageSize = PageSize - iLastCodePageSize;
	}
// Copy phan code vao memory
	for (i = 0; i < iNumCodePage; i++)
	{
		executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage*PageSize, i < (iNumCodePage-1) ? PageSize :iLastCodePageSize, noffH.code.inFileAddr + i*PageSize);
	}

  // Xet coi phan code o trang cuoi co day ko, neu khong load trang dau cua phan data vao lap
  // day phan nay
	if (iLastCodePageSize < PageSize)
	{
		// Copy phan data vao phan con lai cua trang cuoi phan code
		if (iFirstDataPageSize > 0)
			executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr])+(pageTable[i-1].physicalPage*PageSize + iLastCodePageSize), iFirstDataPageSize, noffH.initData.inFileAddr);
	}

// Copy phan con lai cua data vao cac trang sau
	for (j = 0; j < iNumDataPage; j++)
	{
		// if(noffH.initData.size > 0)
		executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage*PageSize,j < (iNumDataPage-1) ? PageSize : iLastDataPageSize,noffH.initData.inFileAddr + j*PageSize + iFirstDataPageSize);
		i++;
	}
	delete executable;

}

AddrSpace::AddrSpace(char *filename)
{
  NoffHeader noffH;
  unsigned int i, j, size;
  unsigned int iNumCodePage, iNumDataPage; // So trang cua phan code va so trang cua phan data
  unsigned int iLastCodePageSize, iFirstDataPageSize, iRemainDataSize, iLastDataPageSize;

	OpenFile* executable = fileSystem->Open(filename);
	if (executable == NULL)
	{
		printf("\nAddrspace::Loi mo file: %s",filename);
		//DEBUG(dbgFile,"\n Error opening file.");
		return;
	}
    // Doc Header
    executable->ReadAt((char *)&noffH, sizeof(noffH), 0);
    // Kiem tra xem du lieu co duoc luu theo Little Endian ko
    if ((noffH.noffMagic != NOFFMAGIC) &&
		(WordToHost(noffH.noffMagic) == NOFFMAGIC))
    	SwapHeader(&noffH);
    ASSERT(noffH.noffMagic == NOFFMAGIC);

// how big is address space?
    size = noffH.code.size + noffH.initData.size + noffH.uninitData.size
			+ UserStackSize;	// we need to increase the size
						// to leave room for the stack
    numPages = divRoundUp(size, PageSize); // 1 pagesize = 2*SectorSize
    size = numPages * PageSize;            // Kich thuoc ma ct se can

    ASSERT(numPages <= NumPhysPages);		// check we're not trying
						// to run anything too big --
						// at least until we have
						// virtual memory

    addrMutex->P();     // Truy xuat doc quyen gPhysPageBitMAp
    // Kiem tra xem voi so trang ct can, co con du bo nho vat ly trong khong
    if (numPages > gPhysPageBitMap->NumClear())
	  {
		  printf("\nAddrSpace:Load: not enough memory for new process..!");
		  numPages = 0;
		  delete executable;
		  addrMutex->V();
      return;
	  }
    DEBUG('a', "Initializing address space, num pages %d, size %d\n",
					numPages, size);
// first, set up the translation
  /*  pageTable = new TranslationEntry[numPages];
    for (i = 0; i < numPages; i++) {
	pageTable[i].virtualPage = i;	// for now, virtual page # = phys page #
	pageTable[i].physicalPage = i;
	pageTable[i].valid = TRUE;
	pageTable[i].use = FALSE;
	pageTable[i].dirty = FALSE;
	pageTable[i].readOnly = FALSE;  // if the code segment was entirely on
					// a separate page, we could set its
					// pages to be read-only
    }

// zero out the entire address space, to zero the unitialized data segment
// and the stack segment
    bzero(machine->mainMemory, size);  */
    // first, set up the translation
	pageTable = new TranslationEntry[numPages];
	for (i = 0; i < numPages; i++)
	{
		pageTable[i].virtualPage = i; // for now, virtual page # = phys page #
		pageTable[i].physicalPage = gPhysPageBitMap->Find();  // Tim trang con trong
		pageTable[i].valid = TRUE;
		pageTable[i].use = FALSE;
		pageTable[i].dirty = FALSE;
		pageTable[i].readOnly = FALSE; // if the code segment was entirely on a separate page, we could set its pages to be read-only

		// Xoa trang da chon duoc
		bzero(&(machine->mainMemory[pageTable[i].physicalPage*PageSize]), PageSize);
		printf("Trang vat ly %d \n",pageTable[i].physicalPage);
	}
  addrMutex->V();
/*// then, copy in the code and data segments into memory
    if (noffH.code.size > 0) {
        DEBUG('a', "Initializing code segment, at 0x%x, size %d\n",
			noffH.code.virtualAddr, noffH.code.size);
        executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]),
			noffH.code.size, noffH.code.inFileAddr);
    }
    if (noffH.initData.size > 0) {
        DEBUG('a', "Initializing data segment, at 0x%x, size %d\n",
			noffH.initData.virtualAddr, noffH.initData.size);
        executable->ReadAt(&(machine->mainMemory[noffH.initData.virtualAddr]),
			noffH.initData.size, noffH.initData.inFileAddr);
    }
    */
    //Tinh so trang cua phan code
	iNumCodePage = divRoundUp(noffH.code.size, PageSize);

    // Tinh kich thuoc trang cuoi cua phan code
	iLastCodePageSize = noffH.code.size - (iNumCodePage-1)*PageSize;
    // Tinh phan con lai cua phan data de phong truong hop trang cuoi phan code khong su dung het page
	iRemainDataSize = noffH.initData.size - (PageSize - iLastCodePageSize);
  // Tinh kich thuoc trang dau phan data de co the load vua du vao phan du cua trang code cuoi
	if (iRemainDataSize < 0)
	{
		iNumDataPage = 0;
		iFirstDataPageSize = noffH.initData.size;
	}
	else
	{
		iNumDataPage = divRoundUp(iRemainDataSize, PageSize);
		iLastDataPageSize = iRemainDataSize - (iNumDataPage-1)*PageSize;
		iFirstDataPageSize = PageSize - iLastCodePageSize;
	}
// Copy phan code vao memory
	for (i = 0; i < iNumCodePage; i++)
	{
		executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage*PageSize, i < (iNumCodePage-1) ? PageSize :iLastCodePageSize, noffH.code.inFileAddr + i*PageSize);
	}

  // Xet coi phan code o trang cuoi co day ko, neu khong load trang dau cua phan data vao lap
  // day phan nay
	if (iLastCodePageSize < PageSize)
	{
		// Copy phan data vao phan con lai cua trang cuoi phan code
		if (iFirstDataPageSize > 0)
			executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr])+(pageTable[i-1].physicalPage*PageSize + iLastCodePageSize), iFirstDataPageSize, noffH.initData.inFileAddr);
	}

// Copy phan con lai cua data vao cac trang sau
	for (j = 0; j < iNumDataPage; j++)
	{
		// if(noffH.initData.size > 0)
		executable->ReadAt(&(machine->mainMemory[noffH.code.virtualAddr]) + pageTable[i].physicalPage*PageSize,j < (iNumDataPage-1) ? PageSize : iLastDataPageSize,noffH.initData.inFileAddr + j*PageSize + iFirstDataPageSize);
		i++;
	}
	delete executable;
}

//----------------------------------------------------------------------
// AddrSpace::~AddrSpace
// 	Dealloate an address space.  Nothing for now!
//----------------------------------------------------------------------

AddrSpace::~AddrSpace()
{
	for(unsigned int i = 0;i < numPages;i++)
  {
		gPhysPageBitMap->Clear(pageTable[i].physicalPage);
  }
   delete pageTable;
}

//----------------------------------------------------------------------
// AddrSpace::InitRegisters
// 	Set the initial values for the user-level register set.
//
// 	We write these directly into the "machine" registers, so
//	that we can immediately jump to user code.  Note that these
//	will be saved/restored into the currentThread->userRegisters
//	when this thread is context switched out.
//----------------------------------------------------------------------

void
AddrSpace::InitRegisters()
{
    int i;

    for (i = 0; i < NumTotalRegs; i++)
	machine->WriteRegister(i, 0);

    // Initial program counter -- must be location of "Start"
    machine->WriteRegister(PCReg, 0);	

    // Need to also tell MIPS where next instruction is, because
    // of branch delay possibility
    machine->WriteRegister(NextPCReg, 4);

   // Set the stack register to the end of the address space, where we
   // allocated the stack; but subtract off a bit, to make sure we don't
   // accidentally reference off the end!
    machine->WriteRegister(StackReg, numPages * PageSize - 16);
    DEBUG('a', "Initializing stack register to %d\n", numPages * PageSize - 16);
}

//----------------------------------------------------------------------
// AddrSpace::SaveState
// 	On a context switch, save any machine state, specific
//	to this address space, that needs saving.
//
//	For now, nothing!
//----------------------------------------------------------------------

void AddrSpace::SaveState() 
{}

//----------------------------------------------------------------------
// AddrSpace::RestoreState
// 	On a context switch, restore the machine state so that
//	this address space can run.
//
//      For now, tell the machine where to find the page table.
//----------------------------------------------------------------------

void AddrSpace::RestoreState() 
{
    machine->pageTable = pageTable;
    machine->pageTableSize = numPages;
}
