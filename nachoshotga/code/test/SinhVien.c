#include "syscall.h"
#include "global.h"

int main()
{
  int iTime;
  iTime = 0;
  Wait("Uong");
  
  Write("\nUong ", 20, ConsoleOutput);
  //Write(iBottle,2,ConsoleOutput);
  Write(" lit nuoc.", 20, ConsoleOutput);

  while(iTime < 1000)
  {
     iTime++;
  }
  Signal("Rot");
} 
