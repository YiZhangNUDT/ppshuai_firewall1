
#include "ip.h"
struct UserAuth
{
	BYTE  PassTimes; //bit 0Ϊ1��ʾͨ����֤��bit 1,2,3Ϊ��֤�Ĵ���;
};
struct UserPasswd
{
	struct UserPasswd* pnext;
	char*  userpass;
};

/*struct EtherAddr 
{
	unsigned char AddrByte[6];
};*/

struct EtherPacketHead 
{
	struct Ether_Addr DestEther;
	struct Ether_Addr SourEther;
	unsigned short ServType;
};

/*struct IPAddr 
{
	unsigned char AddrByte[4];
};*/

struct IPPacketHead 
{
	BYTE VerHLen;
	BYTE Type;
	WORD TtlLen;
	WORD Id;
	WORD FlgOff;
	BYTE TTL;
	BYTE Proto;
	WORD ChkSum;
	struct IP_Addr SourIP;
	struct IP_Addr DestIP;
};

struct TCPPacketHead 
{
	WORD SourPort;
	WORD DestPort;
	DWORD SeqNo;
	DWORD AckNo;
	BYTE HLen;
	BYTE Flag;
	WORD WndSize;
	WORD ChkSum;
	WORD UrgPtr;
};
struct Hash
{
	WORD pois[5120][127];
	BYTE number[5120];
};
struct URLBlockLogMsg
{
	SYSTEMTIME  Time;        //�����ʱ��
	struct IP_Addr SourIP;   //ԴIP
	struct IP_Addr DestIP;   //Ŀ��IP
	WORD   SourPort;        //ԴPort
	WORD   DestPort;        //Ŀ��Port
	char   URL[250];         //�����URL
	char   Block;           //�Ƿ����
};
struct FilterLogMsg
{
	SYSTEMTIME  Time;
	struct IP_Addr SourIP;
	struct IP_Addr DestIP;
	WORD          SourPort;      //ԴPort
	WORD          DestPort;
	char          KeyWord[3][20];   //���˵Ĺؼ���
};

void ProtoManInit();
BOOL ProtoMain(BYTE* pdata);
BOOL Authentication(BYTE* pdata); 
void SendAutheMsg(char* pdata, int Flag);
void Unbase64(char* pszText, char* pszCode);
WORD swaps(WORD net);
DWORD swapl(DWORD net);
BYTE GetSixBits(char* pszStr, int& i);
int SearchSourceIP(struct IPPacketHead* pIPPacketHead); 
void SendRToServer(BYTE* buf);
BOOL URLBlock(char *buffer);
void ContentFilter(char *buffer);
void SearchFilter(char *text,int len);
void TimeBeginThread();
void SetTimeout();
void SendRes(BYTE *buf);
void SendACK(BYTE *buf);
void SystemLog();
void GetHost(char* buffer);
void SendSpecialBuffer();

extern struct IP_Addr    mask;