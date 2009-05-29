#include "syscall.h"
#include "global.h"


int main()
{
  int i,iTime;
  iTime = 0;
  Wait("Rot");

  //if(iV1 <= iV2)
  //{
    //iV1 += iBottle;
    Write("Voi 1 rot nuoc", 30, ConsoleOutput);
  //}
  //else
  //{
    //iV2 += iBottle;
    Write("Voi 2 rot nuoc", 30, ConsoleOutput);
  //}

  while(iTime < 1000)
  {
     iTime++;
  }            

  Signal("Uong");      
  
} 
