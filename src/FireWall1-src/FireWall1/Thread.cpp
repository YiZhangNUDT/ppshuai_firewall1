
#include "stdafx.h"
#include "FireWall1.h"
#include "outnat.h"
#include "authentication.h"
/////////////////////////////////////
/*
�����������1����ջ��TCP/IPջ�����ݣ�
����tempdataout[480][1600]��
����tempdataout COPY������������global[68]��Buffer��
global_var global[68]��"testddk.h"
�õ��������ͱ��������س�ͬ����
����global_mutex��һ��mutex��
���ڿ���global[]���±�global_cur��
global[].hMutex��һ��Event��
���ڿ��ƻ�������
*/
////////////////////////////////////////
extern BOOL CloseAll;
extern BOOL Out_IOCancel;
extern BOOL In_IOCancel;//������Ҳ��Ҫ
extern global_var global[80];
static int offout;
static int mydataout;
static int count40out;
static int countout;
static int deferout;
static int count1out;
BOOL writeout;
extern BOOL exeThread[480];
void ReadOutBuffer();
static BYTE tempdataout[480][1600];
extern HANDLE global_handle;
extern BYTE SourceIPFlag[4096];
BOOL OPEROUT;
static OVERLAPPED   OverLappedread_out,*lpOverLappedreadout;
extern HANDLE mythread_out;
extern BOOL ACK;
extern int global_cur;
extern HANDLE global_mutex;
extern HANDLE fin_out1thread_mutex;
//BYTE SpecialBuffer[1500];
extern HANDLE mythread_write;
extern int Resno;//�ָ��س�
VOID WINAPI OutReadCompletionRoutine(DWORD dwErrorCode, 
   DWORD dwNumberOfBytesTransferred, 
   LPOVERLAPPED pOverlapped);
void initdata_out();
static int inittime_out=0;
static int timecount;

VOID ReadOutThread(VOID){
	if(inittime_out++==0)	initdata_out();
	Sleep(1);//����Ϊ�����������Ӧ����һ��
	while(!CloseAll)//ϴ������֪ͨ
	{

		{
	
	
			if((offout>=235)&&(mydataout<190))
			{
	//				continue;
			}
			if(count40out==60)
			{
				
				SleepEx(1,TRUE);
				continue;
			}
			else count40out++;
			if(offout>479)offout=0;
			ReadOutBuffer();
		}
	
	}
	


	// TRACE(_T("OUT THREAD FINISH_BEGIN\n"));
	
	
}  
void ReadOutBuffer()
{

	BOOL Result;  
	memset(&OverLappedread_out,0,sizeof(OVERLAPPED));

	lpOverLappedreadout=&OverLappedread_out;
	Result=ReadFileEx(
		global_handle,
		tempdataout[offout],
		1520,
		lpOverLappedreadout,
		OutReadCompletionRoutine);
	if (Result==0)
	{
		int ii=GetLastError();
		OPEROUT=FALSE;
		TRACE(_T("��ͨ\n"));


	}
	else
	{
		
		offout++;   
		OPEROUT=TRUE;

	}
}

VOID WINAPI OutReadCompletionRoutine(DWORD dwErrorCode, 
   DWORD dwNumberOfBytesTransferred, 
   LPOVERLAPPED pOverlapped) {

	if(OPEROUT)
	{
		if(mydataout>479)mydataout=0;
		
		countout++;
		DWORD dw=WaitForSingleObject(global_mutex,INFINITE);
		if(dw==WAIT_OBJECT_0)
		{
			BOOL fdoneout=FALSE;
			{
	
				memcpy(global[global_cur].Buffer,
					tempdataout[mydataout],
					1518);
				fdoneout=TRUE;
			
				global[global_cur].overlap=*pOverlapped;
				SetEvent(global[global_cur].hMutex);

				global_cur++;
				if(global_cur>79)global_cur=0;
				ReleaseMutex(global_mutex);
				count40out--;
			//	count40out=0;
				mydataout++;
			}
			
		}
	}

	if(dwErrorCode!= 0)		
	{
		// TRACE(_T("I/O ERR OUT READCOMP\n"));
	    return;
	}
}
void initdata_out()
{
	mydataout=0;
	deferout=0;
	count40out=0;
	offout=0;
	count1out=0;
	timecount=0;
	OPEROUT=FALSE;
	memset(&OverLappedread_out,0,sizeof(OVERLAPPED));
	for(int aa=0;aa<480;aa++)
	{
		memset(tempdataout[aa],0,1600);
		tempdataout[aa][0]=1;
	}
}
