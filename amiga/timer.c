/* amiga/timer.c: high level io commands for the timer.device

   Copyright (c) 1990-92 Corey Gehman, James E. Wilson

   This software may be copied and distributed for educational, research, and
   not for profit purposes provided that this copyright and statement are
   included in all such copies. */

#include <exec/memory.h>
#include <devices/timer.h>
#include <functions.h>

extern UBYTE *malloc();

struct mydevice {
   struct IORequest *readreq;
   struct MsgPort *readport;
   unsigned short size;
} *currentdevice;

void clean(i)
int i;
{
   switch (i){
      case 6:  if (currentdevice->readreq)
         CloseDevice(currentdevice->readreq);
      case 5:  if (currentdevice->readreq)
         DeleteExtIO(currentdevice->readreq);
      case 4:  if (currentdevice->readport)
         DeletePort(currentdevice->readport);
      case 1:  free(currentdevice);
   }
}

int openreadports(size)   /* 0=error,-1=ok */
int size;
{
   currentdevice->size=size;
   currentdevice->readport=CreatePort("Read Port",0);
   if (currentdevice->readport==0L) {
      printf("The Read message port did not open\n");
      clean(1);
      return(0);
   }
   currentdevice->readreq=CreateExtIO(currentdevice->readport,size);
   if (currentdevice->readreq==0L) {
      printf("Read Standard IO Did Not Open\n");
      clean(4);
      return(0);
   }
   return(-1);
}

int opentimer()         /* 0=error,-1=ok */
{
   currentdevice=(struct mydevice *)malloc(sizeof(struct mydevice));
   if (currentdevice==0) {
      printf("Not enough memory\n");
      return(0);
   }
   setmem(currentdevice,sizeof(struct mydevice),0);
   openreadports(sizeof(struct timerequest));
   if (OpenDevice("timer.device",UNIT_VBLANK,currentdevice->readreq,0)) {
      printf("New Timer OpenDevice Failed\n");
      clean(5);
      return(0);
   }
   currentdevice->readreq->io_Command=TR_ADDREQUEST;
   return(-1);
}

void closetimer()
{
   clean(6);
}

void sendtimer(microsec)
int microsec;
{
struct timerequest *tr;
   tr=(struct timerequest *)currentdevice->readreq;
   tr->tr_time.tv_secs=microsec/1000000;
   tr->tr_time.tv_micro=microsec%1000000;
   SendIO((struct IORequest *)tr);
}

void waittimer()
{
   Wait(1<<(currentdevice->readport->mp_SigBit));
   GetMsg(currentdevice->readport);
}

