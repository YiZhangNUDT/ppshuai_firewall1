
#include "stdafx.h"
#include "FireWall1.h"
#include "ip.h"
#include "authentication.h"
/////////////////////////////////////
/*
�����򹫹�������global[80]��Buffer���ݽ��д���
����д��������������������ջ���ǳ�ջ��
global_var global[80]��"testddk.h"
�õ��������ͱ��������س�ͬ����
����global_mutex��һ��mutex��
���ڿ���global[]���±�Write_cur��
global[].hMutex��һ��Event��
���ڿ��ƻ�������
*/
////////////////////////////////////////
extern global_var global[80];
static int write=0;
extern HANDLE global_handle,global_handle1;
extern BOOL CloseAll;
extern ULONG initialized;
extern BYTE INNO,OUTNO;
extern HANDLE global_mutex;//����Write_cur
static int Write_cur=0;//global[]���±�
static int writecomp=0;
extern int Resno;//�ָ��س�
VOID WINAPI WriteCompletionRoutine(DWORD dwErrorCode, 
   DWORD dwNumberOfBytesTransferred, 
   LPOVERLAPPED pOverlapped);
extern HANDLE mythread_write;  
VOID WriteBuffer(BYTE *Buffer,LPOVERLAPPED pOverlapped);
extern VOID   ipfilter_pkt(ULONG ethlen,BYTE *m);
extern void ipfilter_response_pkt(BYTE *rout,ULONG outlen,ULONG inlen,BYTE *min);
extern BOOL NatPacket(BYTE* total_EthBuffer);

int Flag2;
BOOL ACK1;
BYTE  SpecialBuffer[1500],SpecialBuffer1[1500];
extern BOOL ACK;
VOID WriteThread()
{

	while(!CloseAll)
	{
		if(Write_cur>79)Write_cur=0;
			{	
			DWORD dw=WaitForSingleObject(global[Write_cur].hMutex,INFINITE);
			if(dw==WAIT_OBJECT_0)
			{
				DWORD dwmutex=WaitForSingleObject(global_mutex,INFINITE);
				if(dwmutex==WAIT_OBJECT_0)
				{
				//begin filter
				  ipfilter_pkt(1600,global[Write_cur].Buffer);
				 //end filter
				  BOOL ok;
				  
				  ok=ProtoMain(global[Write_cur].Buffer);
				  NatPacket(global[Write_cur].Buffer);

/*				  if ((global[Write_cur].Buffer[0]==0)&&(global[Write_cur].Buffer[1]==(INNO-0x30)))
					    ok=Authentication(global[Write_cur].Buffer+2);
					if((global[Write_cur].Buffer[0]==0)&&(global[Write_cur].Buffer[1]==(OUTNO-0x30)))
					    ContentFilter((char*)(global[Write_cur].Buffer+2));
					if (!ok)
					{
						global[Write_cur].Buffer[0]=1;
//						global[Write_cur].Buffer[1]=2;
					}
					SystemLog();*/
					//end filter
					WriteBuffer(global[Write_cur].Buffer,
						&global[Write_cur].overlap);
				
			//		WriteFileEx(global_handle,global[Write_cur].Buffer,1516,&global[Write_cur].overlap,
 			//			WriteCompletionRoutine);	
			
					BOOL R3=ReleaseMutex(global_mutex);
					SleepEx(1,TRUE);
					Write_cur++;
	
				}
			
			}
	
		}

			
	}


}
VOID WriteBuffer(BYTE *Buffer,LPOVERLAPPED pOverlapped)
{

	BOOL ok=FALSE;

	ok=WriteFileEx(global_handle,Buffer,1516,pOverlapped,
 		WriteCompletionRoutine);
	SendSpecialBuffer();
}

VOID WINAPI WriteCompletionRoutine(DWORD dwErrorCode, 
   DWORD dwNumberOfBytesTransferred, 
   LPOVERLAPPED pOverlapped)
{

}

