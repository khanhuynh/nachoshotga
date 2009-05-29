#include "syscall.h"
#include "global.h"



int main()
{
  int A[3][3];

  int i,j;
  int id;
  A[0][0] = 2;
  A[0][1] = 1;
  A[0][2] = 4;
  A[1][0] = 3;
  A[1][1] = 2;
  A[1][2] = 1;
  A[2][0] = 2;
  A[2][1] = 2;
  A[2][2] = 1;
  CreateSemaphore("Uong",0);
  CreateSemaphore("Rot",1);
  for(i = 0;i < 3;i++)
  {
    //iV1 = 0;
    //iV2 = 0;
    for(j = 0;j < 3;j++)
    {
      //iBottle = A[i][j];
       
      Exec("./test/SinhVien");
      id = Exec("./test/VoiNuoc");
      Join(id);
      
    } 
  }  
} 
