#ifndef PTABLE_H
#define PTABLE_H
#include "bitmap.h"
#include "pcb.h"
//#include "scexception.h"
#include "semaphore.h"

#define MAXPROCESS 10

class PTable {
 private:
  BitMap *bm;
  PCB    *pcb[MAXPROCESS];
  int     psize;
  Semaphore *bmsem;//dung de ngan chan truong hop nap 2 tien trinh cung luc
 public:

//khoi tao size doi tuong pcb de luu size process. Gán giá tri ban dau là null.
  PTable(int size);

  ~PTable();//huy cac doi tuong da tao
  int ExecUpdate(char* filename);// return PID
  int ExitUpdate(int ec);
  int JoinUpdate(int id);

//tim free slot de luu thong tin cho tien trinh moi
  int GetFreeSlot();

//kiem tra ton tai processID nay hay ko?
  bool IsExist(int pid);
  int GetMax();

//khi tien trinh ket thuc , xoa 1 processID ra khoi mang quan ly no
  void Remove(int pid);
  char* GetName(int pID);
};

#endif
