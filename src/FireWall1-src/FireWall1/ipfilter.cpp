
//����־������ͳ�ƶ��������ı�
//IPԴ·��
//ע��:���������ڶ���������������
//�������ʵ��������
//ע��:���Ǳ��ǵ�����������ʱʹ�õĶ��ⲿ����ip��ַ�����ã�����tcpʹ�ã�
#include "StdAfx.h"
#include "ip.h"
#include "ipfilter.h"
#include "winsock2.h"
#include "Struct.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern DefaultSetData TheDefaultSetData;
struct LogView  logview;
struct LogViewIcmp  logviewIcmp;
extern USHORT logType;

 Statistics filterstats = { NO }; 	//ȱʡ����״̬
 FilterConfig filtercfg = { DEFAULT_SYSL_MASK };	//ȱʡ��־����
 ULONG ptrs_initialized = 0;      
 ULONG initialized = NO;
extern HWND myhWnd1;

 pointerStruct active, newspace;
 FILE *hlogFile,*statFile;
 int filtercount=0;
 struct pkt_struct *mtodPkt(BYTE *r);
 static void freeOutTree (OutaddrTreeNode *tbl, unsigned char level);
 VOID GetStatisticsInfo(Statistics* StatisticInfo);
 USHORT treeOutLookup (OutaddrTreeNode *tbl, ULONG addr, unsigned short max_group);
 static void 
 syslogMessage(unsigned char eventNo,ULONG protocolNo=NULL,ULONG srcAddr=NULL,
			  ULONG dstAddr=NULL, unsigned short srcPort=NULL,unsigned short dstPort=NULL,
			  unsigned char icmptype=NULL,UCHAR dostype=NULL);
 void writestatistic();
 VOID clearipfilter();
 //�ͷ����ṹ�ռ�
 static void freeAllPointers (pointerStruct *p);
//��ʼ�����ṹ����groups����Ϊ1��1�������ࣩ
 static ULONG  initTables (pointerStruct *p, unsigned short groups);
//�ͷŵ�ַ���ռ䣨pointerStruct�е�ָ�룩
 static void freeTree (addrTreeNode *tbl, unsigned char level);
//����ʹ�õĺ���
static ULONG
OutnetaddToTree(OutaddrTreeNode *tbl, in_addr_t addr,/*��������ʼ��ַ,*/ 
			unsigned short hostnums,/*����������ʼ��ַ��ʼ��������,*/
			unsigned short group/*�����˿��б�����һ��*/);
static ULONG
GjaddToTree(addrTreeNode *tbl, in_addr_t addr/*��������ʼ��ַ*/, 
			unsigned short hostnums/*����������ʼ��ַ��ʼ��������*/,
			unsigned short group/*�����˿��б�����һ��*/);
//��portListReq�ṹ�еĶ˿��б���ӵ���pointerStruct�ģ�PortListEntry�ṹ��
static ULONG copyGroup (portListReq *u, PortListEntry *kptr);
//���ö˿��б��������ṹ��
static ULONG portListRequest (portListReq *u, pointerStruct *p);
//��ӵ�ַ������
static ULONG addrTreeRequest (addrTreeReq *u);
//��Ӿܾ������󣨵����ṹ��
static ULONG rejaccTableRequest (ULONG id, rejaccTableReq *u, pointerStruct *p);
static int
OutrejaccTableRequest(int id, rejaccTableReq *u, pointerStruct *p);
//��Ӹ��Ǳ�����
static ULONG overrideTableRequest (overrideTableReq *u, pointerStruct *p);
//����(���ṹ)����;���µ�ǰ����
static ULONG manageTablesRequest (manageTablesReq *u);
//������Ӧ�ĵ�ַ
USHORT treeLookup (addrTreeNode *tbl, ULONG addr, unsigned short max_group);
//�����վ��tcp��
static ULONG checkIncomingTcp (ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort);
//����վ��tcp��
static ULONG checkOutgoingTcp (ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort);
//�����վ��udp
static ULONG checkIncomingUdp (ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort);
//����վudp
static ULONG checkOutgoingUdp (ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort);
//�����վ��icmp����
static ULONG checkIncomingIcmp (ULONG srcAddr, ULONG dstAddr, unsigned char type);
//�����վ��icmp����
static ULONG checkOutgoingIcmp (ULONG srcAddr, ULONG dstAddr, unsigned char type);
//�����DM��tcp(��)
static ULONG checkIncomingDMTcp (ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort);
//�����DM��udp(��)
static ULONG checkIncomingDMUdp (ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort);
//�����DM��icmp(��)
static ULONG checkIncomingDMIcmp(ULONG srcAddr, ULONG dstAddr, unsigned char type);
//���ܾ���
static ULONG checkRejectTable (ip *ipHeader, ULONG srcAddr, ULONG dstAddr);
//static ULONG checkIncomingPacket (unsigned char *pkt,unsigned char protolen);
//�����վ��
static ULONG checkIncomingPacket(USHORT,BYTE*,ULONG,ULONG);
//��������
static ULONG checkAcceptTable (struct ip *ipHeader, ULONG srcAddr, ULONG dstAddr);//END:GJ
//����վ��
static ULONG checkOutgoingPacket (unsigned short protocol,unsigned char *pkt,unsigned short protolen);
//����
static ULONG checkIncomingDM(ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort,UCHAR from);
//��ʼ������������
static ULONG initRequest (initReq *req);
//У���
extern USHORT  GetCksum(USHORT* buffer,int size);
USHORT in_cksum_tcp(BYTE *buffer);//gj
BOOL   in_cksum_hdr(BYTE* buffer);
//input eth and calculate checksum
struct pkt_struct *mtodPkt(BYTE *r)
{
    struct pkt_struct *pkt;
	pkt=(struct pkt_struct *)r[14];
    return pkt;
}

//input eth and calculate checksum
USHORT in_cksum_tcp(BYTE* buffer)
{
    //BYTE* tempbuffer;
	USHORT  tcpleng;
	USHORT  tcpchecksum;
	USHORT*  tempushort;
	USHORT  IPleng;
	//
	BYTE*   tempbuffer;
	//set checksum=0;
	buffer[34+16] = 0;
	buffer[34+17] = 0;

    //getIPleng(total)
    ((BYTE*)(&IPleng))[0] = buffer[17];
    ((BYTE*)(&IPleng))[1] = buffer[16];
	//get tcpleng(total)
	tcpleng = IPleng - 20;
    
	//get pseudo head
		tempbuffer = new BYTE[tcpleng+12];
		memcpy(tempbuffer+12,buffer+34,tcpleng);
		tempbuffer[0] = buffer[26];
		tempbuffer[1] = buffer[27];
		tempbuffer[2] = buffer[28];
		tempbuffer[3] = buffer[29];
		tempbuffer[4] = buffer[30];
		tempbuffer[5] = buffer[31];
		tempbuffer[6] = buffer[32];
		tempbuffer[7] = buffer[33];
        tempbuffer[8] = 0;
		tempbuffer[9] = 6;
		tempbuffer[10] = ((BYTE*)(&tcpleng))[1];
		tempbuffer[11] = ((BYTE*)(&tcpleng))[0];
        
		tempushort = (USHORT*) tempbuffer;
		tcpchecksum = GetCksum(tempushort, tcpleng+12);
		buffer[34+16] = ((BYTE*)(&tcpchecksum))[0];
		buffer[34+17] = ((BYTE*)(&tcpchecksum))[1];
		return tcpchecksum;
}


//input eth and calculate checksum
BOOL in_cksum_hdr(BYTE* buffer)
{
	BYTE* tempbufferttt;
	USHORT* tempushortbuffer;
	USHORT  checksum;
	
	tempbufferttt = &buffer[14];
	tempushortbuffer = (USHORT* )tempbufferttt;
	tempushortbuffer[5] = 0;
    checksum = GetCksum(tempushortbuffer,20);
	tempushortbuffer[5] = checksum;

    return TRUE;
}
/*
//input buffer  and calculate checksum
USHORT  GetCksum(USHORT* buffer,ULONG size)
{
	DWORD cksum;
	USHORT OddByte;

	cksum=0;
	while(size>1)
	{
		cksum+=*buffer++;
		size-=2;
	}
	if(size==1)
	{
        OddByte = 0;
		*((unsigned char*)&OddByte)= *(unsigned char*)buffer;
		cksum+=OddByte;
	}
	cksum=(cksum>>16)+(cksum&0xffff);
	cksum+=(cksum>>16);
	cksum=~cksum;

    return (unsigned short)cksum;
}
*/


static ULONG checkIncomingDM(ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort,UCHAR from)
{
	USHORT group;
	//����Ŀ�ĵ�ַ�Ƿ����鲥
	if (IN_CLASSD(dstAddr)) //0xe0000000~0xef000000
	{
		if (filtercfg.discardMulticast)
		{
			filterstats.f_insideMcast++;
			syslogMessage(SYSL_OUT_CLASSD,IP_PROTO_TCP,
				srcAddr, dstAddr, srcPort, dstPort);
			return DISCARD_PKT;
		}
		return ALLOW_PKT;
	}
    if(from=FROMOUT)
	   group = treeOutLookup(active.Out_DmTree, srcAddr, active.num_groups);
	else
	   group = treeLookup(active.In_DmTree, srcAddr, active.num_groups);
    if(group==1)
       return ALLOW_PKT;
	syslogMessage(SYSL_IN_REJECT,NULL,
			         srcAddr, dstAddr);
    return DISCARD_PKT;
}

// ���в�����������˳��
// ע�⣺�������ı�ı䣬��ipfilter.h�е�enum�ṹҲҪ�ı䡣
// ע��: DM����־
// log����
static SyslogMessageEntry syslogMessages[] = {
	{ "���鲥",				LOG_WARNING },//	SYSL_IN_CLASSD,
	{ "���鲥",				LOG_WARNING },//	SYSL_OUT_CLASSD,
	{ "��˿�",				LOG_WARNING },//	SYSL_IN_PORT,
	{ "���˿�",				LOG_WARNING },//	SYSL_OUT_PORT,		/* only TCP */
	{ "��Э��ͷ̫��",			LOG_WARNING },//	SYSL_IN_LENGTH,		/* TCP/UDP-Header maybe not initialized */
	{ "��Э��ͷ̫��",			LOG_WARNING },//	SYSL_IN_DOS,		/* denial of service attack packet in */
	{ "��ICMP����",				LOG_WARNING },//	SYSL_IN_TYPE,		/* only srcAddr, dstAddr, and icmp type */
	{ "��ICMP����",				LOG_WARNING },//	SYSL_OUT_TYPE,		/* only srcAddr, dstAddr, and icmp type */
	{ "��ܾ�IP",			LOG_WARNING },//	SYSL_IN_REJECT,		/* only protocolNo, srcAddr and dstAddr */
	{ "���ܾ�IP",			LOG_WARNING },//	SYSL_OUT_ACCEPT,	/* only protocolNo, srcAddr and dstAddr */
	{ "���IP",				LOG_NOTICE  },//	SYSL_IN_PROT,		/* only protocolNo, srcAddr and dstAddr */
	{ "����IP",				LOG_WARNING },//	SYSL_OUT_PROT,		/* only protocolNo, srcAddr and dstAddr */
	{ "��ƫ����Ϊ1",	LOG_WARNING },//	SYSL_IN_OFFSET,		/* suspect fragment offset */
	{ "��ƫ����Ϊ1",	LOG_WARNING },//	SYSL_OUT_OFFSET,	/* suspect fragment offset */
	{ "��ֶ�",				LOG_WARNING },//	SYSL_IN_FRAG,		/* fragmented packet */
	{ "���ֶ�",				LOG_WARNING },//	SYSL_OUT_FRAG,		/* fragmented packet */
	{ "��D_O_S����",			LOG_WARNING },//	SYSL_IN_DOS,		/* denial of service attack packet in */
	{ "��D_O_S����",			LOG_WARNING },//	SYSL_OUT_DOS,		/* denial of service attack packet out */
	{"ԭ·�ɹ���",LOG_WARNING},
	{"��ICMP",LOG_WARNING},
	{"��ICMP",LOG_WARNING},
	{"��IGMP",LOG_WARNING},
	{"��IGMP",LOG_WARNING},
};

static const char *syslogDosTypes[] = { "\"Smurf\"", "\"Pong\"" };
//Config.cpp�������־����
static void
syslogMessage(unsigned char eventNo,ULONG protocolNo,ULONG srcaddr,
			  ULONG dstaddr, unsigned short srcport,unsigned short dstport,
			  unsigned char icmptype,UCHAR dostype)
{

	//�������־����ֹ���ͷ��ء�
	if ( !((1UL << eventNo) & filtercfg.logMask))
		return;
	char tempchar[10];
	SYSTEMTIME  SystemTime;
	GetLocalTime(&SystemTime);
	char cdstaddr[16];
	char csrcaddr[16];
    struct in_addr dstin,srcin;
	dstin.S_un.S_addr=htonl(dstaddr);
	srcin.S_un.S_addr=htonl(srcaddr);
    memcpy(cdstaddr,inet_ntoa(dstin),16);
    memcpy(csrcaddr,inet_ntoa(srcin),16);
	switch ( eventNo ) {
		case SYSL_IN_OFFSET:
		case SYSL_OUT_OFFSET:
		case SYSL_IN_FRAG:
		case SYSL_OUT_FRAG:
		case SYSL_IN_REJECT:
		case SYSL_OUT_ACCEPT:
		case SYSL_IN_PROT: 
		case SYSL_OUT_PROT: 
		case SYSL_OUT_ROUT:
        case SYSL_IN_ICMP:
		case SYSL_OUT_ICMP:
		case SYSL_IN_IGMP:
		case SYSL_OUT_IGMP:
		{	/* IP - general */
		    	switch(protocolNo)
				{
		    	case IP_PROTO_TCP:
				    strcpy(tempchar,"TCP");
			    	break;
			    case IP_PROTO_UDP:
				    strcpy(tempchar,"UDP");
				    break;
			    case IP_PROTO_ICMP:
				    strcpy(tempchar,"ICMP");
				    break;
				case	IP_PROTO_IGMP:
					strcpy(tempchar,"IGMP");
			    default:
					strcpy(tempchar,"");
				    break;
			}
/*			sprintf(filterlog.str[filterlog.pos],"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tЭ�飺%s\tFrom:%s" "To:%s\n",
				SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,
				tempchar,csrcaddr,cdstaddr);
			filterlog.pos++;
*/          if(logType==1)
			{
            sprintf(logview.time,"%d��%d��%d:%d:%d",SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				      SystemTime.wMinute,SystemTime.wSecond);
            strcpy(logview.prot,tempchar);
			strcpy(logview.src,csrcaddr);
			strcpy(logview.dst,cdstaddr);
			strcpy(logview.demo,syslogMessages[eventNo].message);
            //PostMessage(AfxGetMainWnd()->GetSafeHwnd(),M_LOGVIEW,(WPARAM)&logview,(LPARAM)111);
            PostMessage(myhWnd1,M_LOGVIEW,(WPARAM)&logview,(LPARAM)111);
			}
			fprintf(hlogFile,"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tЭ�飺%s\tFrom:%s" "To:%s\n"
				,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,
				tempchar,csrcaddr,cdstaddr);
              filtercount++;
			break;
			}
		case SYSL_IN_CLASSD:
		case SYSL_OUT_CLASSD:
		case SYSL_IN_PORT:
		case SYSL_OUT_PORT:
		case SYSL_IN_LENGTH:
		case SYSL_OUT_LENGTH:
			{	/* IP - TCP/UDP */
			  strcpy(tempchar,(protocolNo == IP_PROTO_UDP) ? "UDP" : "TCP");
/*			  sprintf(filterlog.str[filterlog.pos],"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tЭ�飺%s\tFrom%s:%d"  "To%s:%d\n",
				SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,
				tempchar,csrcaddr,ntohs(srcport),cdstaddr,ntohs(dstport));	
			  filterlog.pos++;
*/
			if(logType==1)
            {
            memset((char*)&logview,0,sizeof(LOGVIEW));
			sprintf(logview.time,"%d��%d��%d:%d:%d",SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				      SystemTime.wMinute,SystemTime.wSecond);
            strcpy(logview.prot,tempchar);
			sprintf(logview.src,"%s:%d",csrcaddr,ntohs(srcport));
			sprintf(logview.dst,"%s:%d",cdstaddr,ntohs(dstport));
			strcpy(logview.demo,syslogMessages[eventNo].message);
            PostMessage(myhWnd1,M_LOGVIEW,(WPARAM)&logview,(LPARAM)111);
 //           PostMessage(AfxGetMainWnd()->GetSafeHwnd(),M_LOGVIEW,(WPARAM)&logview,(LPARAM)111);
			}

			  fprintf(hlogFile,"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tЭ�飺%s\tFrom%s:%d"  "To%s:%d\n"
				,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,
				tempchar,csrcaddr,ntohs(srcport),cdstaddr,ntohs(dstport));	
              filtercount++;
	          break;
			}

		case SYSL_IN_TYPE: 	/* IP - ICMP */
		case SYSL_OUT_TYPE:
			{
/*			  sprintf(filterlog.str[filterlog.pos],"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tFrom%s"  "To%s\tICMP���ͣ�%d\n",
				SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,
				csrcaddr,cdstaddr,icmptype);	
			  filterlog.pos++;
*/
		    strcpy(tempchar,"ICMP");
			if(logType==1)
			{
            memset((char*)&logviewIcmp,0,sizeof(LOGVIEWICMP));
			sprintf(logviewIcmp.time,"%d��%d��%d:%d:%d",SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				      SystemTime.wMinute,SystemTime.wSecond);
            strcpy(logviewIcmp.prot,tempchar);
			strcpy(logviewIcmp.src,csrcaddr);
			strcpy(logviewIcmp.dst,cdstaddr);
			strcpy(logviewIcmp.demo,syslogMessages[eventNo].message);
			sprintf(logviewIcmp.type,"%d",icmptype);
   //         PostMessage(AfxGetMainWnd()->GetSafeHwnd(),M_LOGVIEW,(WPARAM)&logviewIcmp,(LPARAM)112);
            PostMessage(myhWnd1,M_LOGVIEW,(WPARAM)&logviewIcmp,(LPARAM)112);
			}

			  fprintf(hlogFile,"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tFrom%s"  "To%s\tICMP���ͣ�%d\n",
				SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,
				csrcaddr,cdstaddr,icmptype);
              filtercount++;
              break;
			}

		case SYSL_IN_DOS:	/* DOS ���� */
		case SYSL_OUT_DOS:
			{
/*			  sprintf(filterlog.str[filterlog.pos],"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tDOS���ͣ�%d\tFrom%s"  "To%s\n",
				SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
			  SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,syslogDosTypes[dostype],
			    csrcaddr,cdstaddr);	
			  filterlog.pos++;
*/
            if(logType==1)
            {
            memset((char*)&logview,0,sizeof(LOGVIEW));
			sprintf(logview.time,"%d��%d��%d:%d:%d",SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
				      SystemTime.wMinute,SystemTime.wSecond);
			strcpy(logview.src,csrcaddr);
			strcpy(logview.dst,cdstaddr);
			strcpy(logview.demo,syslogMessages[eventNo].message);
    //        PostMessage(AfxGetMainWnd()->GetSafeHwnd(),M_LOGVIEW,(WPARAM)&logview,(LPARAM)111);
            PostMessage(myhWnd1,M_LOGVIEW,(WPARAM)&logview,(LPARAM)111);
			}	

			  fprintf(hlogFile,"ʱ�䣺%d��%d��%d:%d:%d\t˵����%s\tDOS���ͣ�%d\tFrom%s"  "To%s\n"
				,SystemTime.wMonth,SystemTime.wDay,SystemTime.wHour,
			  SystemTime.wMinute,SystemTime.wSecond,syslogMessages[eventNo].message,syslogDosTypes[dostype],
			    csrcaddr,cdstaddr);
              filtercount++;
			  break;
			}
/*
			if(filterlog.pos==1000)
			{
	            fwrite(filterlog.str,sizeof(char),filterlog.pos*100,hlogFile);
				memset(&filterlog,0,sizeof(FilterLog));
				filterlog.pos=0;
                writestatistic();
				fprintf(statFile,"\n");
				fclose(statFile);
	            fclose(hlogFile);
				hlogFile=fopen("��־.txt","a");
	            statFile=fopen("ͳ��.txt","a");
			}
*/	
			if(filtercount==100)
			{
				memset(&filterstats,0,sizeof(Statistics));
				filtercount=0;
                writestatistic();
				fprintf(statFile,"\n");
				fclose(statFile);
	            fclose(hlogFile);
				hlogFile=fopen("��־.txt","a");
	            statFile=fopen("ͳ��.txt","a");
			}



	}

}

/* ------------------------------------------------------------------------ */

static void
freeAllPointers(pointerStruct *p)
{//��free������ȥ����M_DEVBUF�������˲�����ͷ�ļ���
	if (p->addrTree)	freeTree(p->addrTree, 0);
	if (p->t_dst_out)	free(p->t_dst_out);
        if (p->t_src_out)	free(p->t_src_out);
        if (p->u_dst_out)	free(p->u_dst_out);
        if (p->u_src_out)	free(p->u_src_out);
        if (p->i_typ_in)	free(p->i_typ_in);
        if (p->i_typ_out)	free(p->i_typ_out);
        if (p->t_rst_in)	free(p->t_rst_in);
        if (p->t_rst_out)	free(p->t_rst_out);
        if (p->acceptTable)	free(p->acceptTable);
        if (p->acceptTree)	freeTree(p->acceptTree, 0);
        if (p->rejectTable)	free(p->rejectTable);
        if (p->rejectTree)	freeOutTree(p->rejectTree, 0);
        if(p->In_DmTree)freeTree(p->In_DmTree,0);
        if(p->Out_DmTree)freeOutTree(p->Out_DmTree,0);

	memset(p,0,sizeof(pointerStruct));
}

/* ------------------------------------------------------------------------ */
//groups:�����趨�����������Ŀ
static ULONG
initTables(pointerStruct *p, unsigned short groups)
{
	/* ����������һ�� */
	if (groups == 0)
		groups = 1;

	/* �ͷ�������ָ��ṹ����Ŀռ�*/
	freeAllPointers(p);

	/* ����һ��ȱʡ���� */
	p->addrTree  = MALLOC_NODE;

 //groups*32*sizeof(PortListEntry)
	p->t_dst_out = MALLOC_GROUP(groups);
	p->t_src_out = MALLOC_GROUP(groups);
	p->u_dst_out = MALLOC_GROUP(groups);
	p->u_src_out = MALLOC_GROUP(groups);
	p->i_typ_in  = MALLOC_GROUP(groups);
	p->i_typ_out = MALLOC_GROUP(groups);
	p->t_rst_in  = MALLOC_GROUP(groups);
	p->t_rst_out = MALLOC_GROUP(groups);
	if ((p->addrTree == NULL) ||
	    (p->t_dst_out == NULL) ||(p->t_src_out == NULL) ||
        (p->u_dst_out == NULL) ||(p->u_src_out == NULL) ||
	    (p->i_typ_in  == NULL) || (p->i_typ_out == NULL) ||
	    (p->t_rst_in  == NULL) || (p->t_rst_out == NULL)) {
		freeAllPointers(p);
		return ERROR_NO_MEMORY;
	}
	p->num_groups = groups;
	memset(p->addrTree, 0, sizeof(addrTreeNode));
	memset(p->t_dst_out,0,groups * LIST_TABLE_SIZE);
	memset(p->t_src_out,0,groups * LIST_TABLE_SIZE);
	memset(p->u_dst_out,0,groups * LIST_TABLE_SIZE);
	memset(p->u_src_out,0,groups * LIST_TABLE_SIZE);
	memset(p->i_typ_in,0,groups * LIST_TABLE_SIZE);
	memset(p->i_typ_out,0,groups * LIST_TABLE_SIZE);
	memset(p->t_rst_in,0,LIST_TABLE_SIZE);
	memset(p->t_rst_out,0,LIST_TABLE_SIZE);

	/* ��дȱʡ���*/
	switch(TheDefaultSetData.Priority)
	{
	case 0://high
        p->t_dst_out[0].begin = 2;	// TCP - 1�� 
        p->t_dst_out[1].begin = 80;	// TCP -http out    
        p->t_dst_out[1].end = 80;
        p->t_dst_out[2].begin=1024;//>=1024 OUT
	    p->t_dst_out[2].begin=0xFFFF;
        
		p->t_src_out[0].begin=1;
	    p->t_src_out[1].begin=1024;
	    p->t_src_out[1].end=0xFFFF;

	    p->u_src_out[0].begin=1;
	    p->u_src_out[1].begin=1024;
        p->u_src_out[1].end=0xFFFF;
	    p->u_dst_out[0].begin = 1;	///* UDP - 1��
	    p->u_dst_out[1].begin = 1024;	///* UDP >=1024 out    
	    p->u_dst_out[1].end = 0xFFFF;

	    p->i_typ_in[0].begin = 1;	///* ICMP - 1��
	    p->i_typ_in[1].begin = 0;	///* ICMP - FORBID      
	    p->i_typ_in[1].end = 0;

	    p->i_typ_out[0].begin = 1;	//* ICMP - 1��
	    p->i_typ_out[1].begin = 0;	//* ICMP - FORBID       
	    p->i_typ_out[1].end = 0;
		break;
	case 1://middle
        p->t_dst_out[0].begin = 5;	// TCP - 1�� 
        p->t_dst_out[1].begin = 80;	// TCP -http out    
        p->t_dst_out[1].end = 80;
        p->t_dst_out[2].begin =20;	// TCP -FTP TELNET out    
        p->t_dst_out[2].end = 23;
        p->t_dst_out[3].begin =137;	// TCP -NBT out    
        p->t_dst_out[3].end =139;
        p->t_dst_out[4].begin =161;	// TCP -SNMP out    
        p->t_dst_out[4].end = 162;	
		p->t_dst_out[5].begin=1024;//>=1024 OUT
	    p->t_dst_out[5].begin=0xFFFF;
        
		p->t_src_out[0].begin=1;
	    p->t_src_out[1].begin=0;
	    p->t_src_out[1].end=0xFFFF;

	    p->u_src_out[0].begin=1;
	    p->u_src_out[1].begin=0;
        p->u_src_out[1].end=0xFFFF;
	    p->u_dst_out[0].begin = 1;	///* UDP - 1��
	    p->u_dst_out[1].begin = 1024;	///* UDP >=1024 out    
	    p->u_dst_out[1].end = 0xFFFF;

	    p->i_typ_in[0].begin = 1;	///* ICMP - 1��
	    p->i_typ_in[1].begin = 0;	///* ICMP - everything out 
	    p->i_typ_in[1].end = 0xFF;

	    p->i_typ_out[0].begin = 1;	//* ICMP - 1��
	    p->i_typ_out[1].begin = 0;	//* ICMP - everything out       
	    p->i_typ_out[1].end = 0xFF;
		break;
	case 2://low
        p->t_dst_out[0].begin = 5;	// TCP - 1�� 
        p->t_dst_out[1].begin = 80;	// TCP -http out    
        p->t_dst_out[1].end = 80;
        p->t_dst_out[2].begin =20;	// TCP -FTP TELNET out    
        p->t_dst_out[2].end = 23;
        p->t_dst_out[3].begin =137;	// TCP -NBT out    
        p->t_dst_out[3].end =139;
        p->t_dst_out[4].begin =161;	// TCP -SNMP out    
        p->t_dst_out[4].end = 162;	
		p->t_dst_out[5].begin=1024;//>=1024 OUT
	    p->t_dst_out[5].begin=0xFFFF;
        
		p->t_src_out[0].begin=1;
	    p->t_src_out[1].begin=0;
	    p->t_src_out[1].end=0xFFFF;

	    p->u_src_out[0].begin=1;
	    p->u_src_out[1].begin=0;
        p->u_src_out[1].end=0xFFFF;
	    p->u_dst_out[0].begin = 2;	///* UDP - 1��
	    p->u_dst_out[1].begin = 53;	///* UDP dns out    
	    p->u_dst_out[1].end = 53;
	    p->u_dst_out[1].begin =1024;
	    p->u_dst_out[1].end = 0xFFFF;
	    p->i_typ_in[0].begin = 1;	///* ICMP - 1��
	    p->i_typ_in[1].begin = 0;	///* ICMP - everything out 
	    p->i_typ_in[1].end = 0xFF;

	    p->i_typ_out[0].begin = 1;	//* ICMP - 1��
	    p->i_typ_out[1].begin = 0;	//* ICMP - everything out       
	    p->i_typ_out[1].end = 0xFF;
		break;
	default://high
        p->t_dst_out[0].begin = 2;	// TCP - 1�� 
        p->t_dst_out[1].begin = 80;	// TCP -http out    
        p->t_dst_out[1].end = 80;
        p->t_dst_out[2].begin=1024;//>=1024 OUT
	    p->t_dst_out[2].begin=0xFFFF;
        
		p->t_src_out[0].begin=1;
	    p->t_src_out[1].begin=1024;
	    p->t_src_out[1].end=0xFFFF;

	    p->u_src_out[0].begin=1;
	    p->u_src_out[1].begin=1024;
        p->u_src_out[1].end=0xFFFF;
	    p->u_dst_out[0].begin = 1;	///* UDP - 1��
	    p->u_dst_out[1].begin = 1024;	///* UDP >=1024 out    
	    p->u_dst_out[1].end = 0xFFFF;

	    p->i_typ_in[0].begin = 1;	///* ICMP - 1��
	    p->i_typ_in[1].begin = 0;	///* ICMP - FORBID      
	    p->i_typ_in[1].end = 0;

	    p->i_typ_out[0].begin = 1;	//* ICMP - 1��
	    p->i_typ_out[1].begin = 0;	//* ICMP - FORBID       
	    p->i_typ_out[1].end = 0;
	}
	return NOO_ERROR;
}

/*-----------------------------------------------------------------------*/

static void
freeTree(addrTreeNode *tbl, unsigned char level)
{
        ULONG i;

        if (!tbl || (level >1)) 
		{
		//��������
                return;
		}

        for (i=0; i<256; i++) 
		{
                if (tbl->u[i].pd)
				{
			        free(tbl->u[i].pd);
				}
		}

	free(tbl);
}

static ULONG
copyGroup(portListReq *u, PortListEntry *kptr)
{
	if (kptr == NULL)
		return ERROR_NET_NOT_FOUND; 
	//׼ȷ����ӵ�β
	 USHORT pos=kptr[0].begin;
     kptr[0].begin+=u->bytes/sizeof(PortListEntry);
	 if(pos==0)
		 pos++;
	 memcpy(&kptr[pos],u->ptr, u->bytes);//��Ҫ�ж��Ѿ��趨�Ķ˿���
	 return u->bytes;
}

/* ------------------------------------------------------------------------ */

static ULONG
portListRequest(portListReq *u, pointerStruct *p)
{
	//u->group:��ǰ��������
	ULONG byte=u->bytes*sizeof(PortListEntry);
	ULONG ret=u->group + ((byte/ LIST_TABLE_SIZE) +
		        (byte% LIST_TABLE_SIZE) ? 1 : 0);
	if (ret > p->num_groups)
		return ERROR_OUT_OF_RANGE;

	switch (u->id) {
	case T_DST_OUT:
		return copyGroup(u ,&(p->t_dst_out[(u->group-1)  * LIST_SIZE]));
	case T_SRC_OUT:
		return copyGroup(u ,&(p->t_src_out[(u->group-1) * LIST_SIZE]));
	case U_DST_OUT:
		return copyGroup(u ,&(p->u_dst_out[(u->group-1) * LIST_SIZE]));
	case U_SRC_OUT:
		return copyGroup(u ,&(p->u_src_out[(u->group-1) * LIST_SIZE]));
	case I_TYP_IN:
		return copyGroup(u ,&(p->i_typ_in[(u->group-1) * LIST_SIZE]));
	case I_TYP_OUT:
		return copyGroup(u ,&(p->i_typ_out[(u->group-1) * LIST_SIZE]));
	case T_RST_IN:
		if (u->group > 1)
			return ERROR_OUT_OF_RANGE;
		return copyGroup(u, p->t_rst_in);
	case T_RST_OUT:
		if (u->group > 1)
			return ERROR_OUT_OF_RANGE;
		return copyGroup(u, p->t_rst_out);
	default:
		return ERROR_UNKNOWN_GROUP;
	}
}

/* ------------------------------------------------------------------------ */

static ULONG
addrTreeRequest(addrTreeReq *u)
{
	register unsigned char i;
    if (newspace.addrTree == NULL) 
			return ERROR_NET_NOT_FOUND; 

		/* Add the specified IP cidr(s) to the 'newspace' config */
	//���128��ip��ַ��������ÿ�����������ʼip��(�����128��������)
		for (i=0; (i < u->num) && (i < 128); i++) {
			if (GjaddToTree(newspace.addrTree, u->addr[i],
				     u->bits[i], u->group[i])) {
				freeTree(newspace.addrTree, 0);
				newspace.addrTree = NULL;
				return ERROR_NO_MEMORY;
			}
		}
	return NOO_ERROR;
}

/* ------------------------------------------------------------------------ */
static int
OutrejaccTableRequest(int id, rejaccTableReq *u, pointerStruct *p)
{
	register unsigned char i;
	RejAccTableEntry *table;   //����ܾ���
	OutaddrTreeNode *tree;        //��ַ�������ڰ�����
	//u->bytes������sizeof(RejAccTableEntry)��������
	if ((u->bytes == 0) || (u->bytes % sizeof(RejAccTableEntry)))
		return ERROR_INVALID_OP;

	//Ϊ������ռ�		 
	table = (RejAccTableEntry *)malloc(u->bytes);//,M_DEVBUF,M_NOWAIT);
	if (table == NULL)
		return ERROR_NO_MEMORY;
	tree = MALLOC_OUTNODE;
	if (tree == NULL)
	{
		free(table);//);
		return ERROR_NO_MEMORY;
	} 
	memset(tree,0,sizeof(addrTreeNode));
	memset(table,0, u->bytes);
    memcpy(table, u->ptr, u->bytes);//gj
//		return u->bytes;//gj
		// Populate the tree from the table - keep the table around
		// to spit back out when requested by the user interface.
	for (i=u->bytes/sizeof(RejAccTableEntry); i; i--) {
		register int r;
		//���ܾ���IP��ַ��ӵ���
		if ((r=OutnetaddToTree(tree, table[i-1].network,
			table[i-1].bits, table[i-1].flag))) {
			freeOutTree(tree, 0);
			free(table);//);
			return (r>0) ? ERROR_NO_MEMORY :
			ERROR_INVALID_OP;
			}
		}
	//�������ṹ
	 switch(id)
	 {
	   case T_REJTBL:
		if ((p->rejectTable != NULL) || (p->rejectTree != NULL))
		{
		free(p->rejectTable);
		freeOutTree(p->rejectTree,0);
		} 
		p->rejectTable = table;
		p->rejectTree = tree;
		p->reject_bytes = u->bytes;
		break;
	   case T_OUTDM:
		 if (p->Out_DmTree!= NULL)
		freeOutTree(p->Out_DmTree,0);
		p->Out_DmTree = tree;
		free(table);
		   break;
	   default:
		   freeOutTree(tree,0);
		   free(table);
		   return ERROR_INVALID_OP;
	 }
		return NOO_ERROR;
}


/* ------------------------------------------------------------------------ */
static ULONG
rejaccTableRequest(ULONG id, rejaccTableReq *u, pointerStruct *p)
{
	register unsigned char i;
	RejAccTableEntry *table;   //����ܾ���
	addrTreeNode *tree;        //��ַ�������ڰ�����
	//u->bytes������sizeof(RejAccTableEntry)��������
	if ((u->bytes == 0) || (u->bytes % sizeof(RejAccTableEntry)))
		return ERROR_INVALID_OP;

	//Ϊ������ռ�		 
	table = (RejAccTableEntry *)malloc(u->bytes);//,M_DEVBUF,M_NOWAIT);
	if (table == NULL)
		return ERROR_NO_MEMORY;
	tree = MALLOC_NODE;
	if (tree == NULL)
	{
		free(table);//);
		return ERROR_NO_MEMORY;
	} 
	memset(tree,0,sizeof(addrTreeNode));
	memset(table,0, u->bytes);
    memcpy(table, u->ptr, u->bytes);//gj
//		return u->bytes;//gj
		// Populate the tree from the table - keep the table around
		// to spit back out when requested by the user interface.
	for (i=u->bytes/sizeof(RejAccTableEntry); i; i--) {
		register ULONG r;
		//���ܾ���IP��ַ��ӵ���
		if ((r=GjaddToTree(tree, table[i-1].network,
			table[i-1].bits, table[i-1].flag))) {
			freeTree(tree, 0);
			free(table);//);
			return (r>0) ? ERROR_NO_MEMORY :
			ERROR_INVALID_OP;
			}
		}
	//�������ṹ
	 switch(id)
	 {
	   case T_ACCTBL:
		if ((p->acceptTree != NULL) || (p->acceptTable!= NULL))
		{
		free(p->acceptTable);
		freeTree(p->acceptTree,0);
		} 
		p->acceptTable = table;
		p->acceptTree = tree;
		p->accept_bytes = u->bytes;
		break;
	   case T_INDM:
		 if (p->In_DmTree != NULL)
		  freeTree(p->In_DmTree,0);
		   p->In_DmTree = tree;
		free(table);
		   break;
	   default:
		   freeTree(tree,0);
		   free(table);
		   return ERROR_INVALID_OP;
	 }
		return NOO_ERROR;
}

static ULONG
manageTablesRequest(manageTablesReq *u)
{
	pointerStruct tmp;
	unsigned long rcode;
	switch (u->command) {
	//��������Ϊ��ǰ���ã������ԭ������
	case ACTIVATE_NEW:
		if ((newspace.num_groups == 0) || (newspace.addrTree == NULL) ||
		    (newspace.t_dst_out == NULL) ||(newspace.t_src_out == NULL) ||
           (newspace.u_dst_out == NULL) ||(newspace.u_src_out == NULL) ||
		    (newspace.i_typ_in == NULL) || (newspace.i_typ_out == NULL) ||
		    (newspace.t_rst_in == NULL) || (newspace.t_rst_out == NULL))
			return ERROR_NET_NOT_FOUND;
		memcpy(&tmp, &active, sizeof(pointerStruct));
		memcpy(&active, &newspace, sizeof(pointerStruct));
		memset(&newspace,0,sizeof(pointerStruct));
		freeAllPointers(&tmp);
		break;
	//���û�������Ϊȱʡ����
	case CLEAR_ALL:
		rcode = initTables(&newspace, 1);
		if (rcode)
		 return rcode;
		memcpy(&tmp, &active, sizeof(pointerStruct));
		memcpy(&active, &newspace, sizeof(pointerStruct));
		memset(&newspace,0,sizeof(pointerStruct));
		freeAllPointers(&tmp);
		break;

	//������������ǰҪ���³�ʼ��
	case INIT_NEW:
		rcode = initTables(&newspace, u->val);
		if (rcode)
			return rcode;
		break;
	}
	return NOO_ERROR;
}

/*-----------------------------------------------------------------------*/
/*
 *����IP��ַ��������
 */
USHORT
treeOutLookup(OutaddrTreeNode *tbl, ULONG addr, unsigned short max_group)
{
    addrTreePtr t1,t2;
	in_addr_t ip;
	if (tbl == NULL)
		return DEFAULT_ACCESS;
//	ip = (in_addr_t)addr;
	ip.ss_addr=addr;//gj..................
        //���ҵ�ַ�ĵ�һ����λ     
		//flg=1: group ;   flg=0: pointer
        if ((t1.p = tbl->u[ip.s_b[3]].p) == NULL)
                return DEFAULT_ACCESS;
        if (tbl->flg[ip.s_b[3]])
                return ((t1.c < max_group) ? t1.c : DEFAULT_ACCESS);

        //���ҵ�ַ�ĵڶ�����λ
        //flg=1: group ;   flg=0: pointer
        // t1.p = tbl->u[ip.s_b[3]].p
      if ((t2.p = t1.p->u[ip.s_b[2]].p) == NULL)
                return DEFAULT_ACCESS;
        if (t1.p->flg[ip.s_b[2]])
                return ((t2.c < max_group) ? t2.c : DEFAULT_ACCESS);

		//begin gj  //���ҵ�ַ�ĵ�������λ
       if ((t1.p =t2.p->u[ip.s_b[1]].p) == NULL)
                return DEFAULT_ACCESS;
       if (t2.p->flg[ip.s_b[1]])
                return ((t2.c < max_group) ? t2.c : DEFAULT_ACCESS);      
		//end gj
		//flg=1: group;    flg=0: pointer
       //t2.p = tbl->u[ip.s_b[3]].p->u[ip.s_b[2]].p
/*        if ((t1.pd = t2.p->u[ip.s_b[1]].pd) == NULL)
			    return DEFAULT_ACCESS;
        if (t2.p->flg[ip.s_b[1]])
                return ((t1.c < max_group) ? t1.c : DEFAULT_ACCESS);
*/

		//���ҵ�ַ�ĵ��ĸ���λ
		//a group
        // * t1.pd = tbl->u[ip.s_b[3]].p->u[ip.s_b[2]].p->u[ip.s_b[1]].pd
        t2.c = t1.pd->c[ip.s_b[0]];
        return ((t2.c < max_group) ? t2.c : DEFAULT_ACCESS);
}


USHORT
treeLookup(addrTreeNode *tbl, ULONG addr, unsigned short max_group)
{
	in_addr_t ip;
	if (tbl == NULL)
		return DEFAULT_ACCESS;
//	ip = (in_addr_t)addr;
	ip.ss_addr=addr;//gj..................
        //���ҵ�ַ�ĵ�һ����λ     
		//flg=1: group ;   flg=0: pointer
	USHORT group;
	if(tbl->u[ip.s_b[1]].pd)
	{
		group=tbl->u[ip.s_b[1]].pd->c[ip.s_b[0]];
		return ((group<max_group)?group:DEFAULT_ACCESS);
	}
	else 
		return DEFAULT_ACCESS;
}

/* ------------------------------------------------------------------------ */
//����������˳��
static ULONG
checkIncomingTcp(ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort)
{
	//����Ƿ��鲥
	if (IN_CLASSD(dstAddr))//dstAddr���鲥��ַ
	{
		if (filtercfg.discardMulticast)
		{
			filterstats.f_outsideMcast++;
			syslogMessage(SYSL_IN_CLASSD, IP_PROTO_TCP,
			      	srcAddr, dstAddr, srcPort, dstPort);
			return DISCARD_PKT;
		}
		return ALLOW_PKT;
	}
	/*
	//ͨ��Ŀ�ĵ�ַ�����в��Ҹõ�ַ��������
	group = treeLookup(active.addrTree, dstAddr, active.num_groups);
	if(group>0&&group<active.num_groups)
	   portList = &active.t_dst_in[(group-1) * LIST_SIZE];
	else
	    	return DISCARD_PKT;
	//���õ�ַ��Ӧ��Ŀ�Ķ˿����ô�
	//����Ŀ�Ķ˿��Ƿ�����
	for (i=1; i <= portList[0].begin; i++)
	{
		if ((dstPort >= portList[i].begin) &&
		    (dstPort <= portList[i].end))
	    		return ALLOW_PKT;
	}
	//dstPort<1024�򲻼��Դ�˿ڣ�ֱ�ӽ�ֹ
	if (dstPort > 1024) 
	{
		//���õ�ַ��Ӧ��Դ�˿����ô�
		portList = &active.t_src_in[group * LIST_SIZE];
		//�Ƚ�Դ�˿�
		for (i=1; i <= portList[0].begin; i++)
		{
			if ((srcPort >= portList[i].begin) &&
			    (srcPort <= portList[i].end))
				return ALLOW_PKT;
		}
	}
	filterstats.f_outsideTcpPort++;
//	syslogMessage(SYSL_IN_PORT, IP_PROTO_TCP,
//		      srcAddr, dstAddr, (ULONG)srcPort, (ULONG)dstPort);
	//��������������TCP�˿���Ӧ������ɼ��ϣ�
	for (i=1; i <= active.t_rst_in[0].begin; i++)
	{
		if ((dstPort >= active.t_rst_in[i].begin) &&
		    (dstPort <= active.t_rst_in[i].end))
		{
			filterstats.outsideTCPreset++;
			return DISCARD_AND_ANSWER_PKT;
		}
	}
	*/
	return DISCARD_PKT;
}

/* ------------------------------------------------------------------------ */
static ULONG
checkOutgoingTcp(ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort)
{
	ULONG i;
	ULONG group;
	PortListEntry *portList;

	//����Ŀ�ĵ�ַ�Ƿ����鲥
	if (IN_CLASSD(dstAddr)) 
	{
		if (filtercfg.discardMulticast)
		{
			filterstats.f_insideMcast++;
		syslogMessage(SYSL_OUT_CLASSD,IP_PROTO_TCP,
				srcAddr, dstAddr, srcPort, dstPort);
			return DISCARD_PKT;
		}
		return ALLOW_PKT;
	}

	//ͨ��Դ��ַ������
	group = treeLookup(active.addrTree, srcAddr, active.num_groups);
	if(group>0&&group<active.num_groups)
	{		
	portList = &active.t_dst_out[(group-1) * LIST_SIZE];
	}
	else 
       return ALLOW_PKT;//δ���ö˿��б�Ĭ�����еĶ˿�����
	//����Ŀ�Ķ˿��Ƿ��������ȥ���ʵĶ˿�
	for (i=1; i <= portList[0].begin; i++) 
	{
		if ((dstPort >= portList[i].begin) &&
		    (dstPort <= portList[i].end))
			return ALLOW_PKT;
	}
	if (dstPort > 1024)
	{
		portList = &active.t_src_out[group * LIST_SIZE];
		for (i=1; i <= portList[0].begin; i++)
		{
			if ((srcPort >= portList[i].begin) &&
			    (srcPort <= portList[i].end))
				return ALLOW_PKT;
		}
	}
		syslogMessage(SYSL_OUT_PORT,IP_PROTO_TCP,
				srcAddr, dstAddr, srcPort, dstPort);
		return DISCARD_PKT;
}
	
/* ------------------------------------------------------------------------ */
static ULONG
checkIncomingUdp(ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort)
{
//	PortListEntry *portList;
	if (IN_CLASSD(dstAddr)) 
	{
		if (filtercfg.discardMulticast) 
		{
			filterstats.f_outsideMcast++;
			syslogMessage(SYSL_IN_CLASSD,IP_PROTO_UDP,
				srcAddr, dstAddr, srcPort,dstPort);
			return DISCARD_PKT;
		}
		return ALLOW_PKT;
	}
	syslogMessage(SYSL_IN_PORT, IP_PROTO_UDP,
		      srcAddr, dstAddr, srcPort, dstPort);

	return DISCARD_PKT;
}

/* ------------------------------------------------------------------------ */

static ULONG
checkOutgoingUdp(ULONG srcAddr, ULONG dstAddr,
		 unsigned short srcPort, unsigned short dstPort)
{
	register ULONG i;
	ULONG group;
	PortListEntry *portList;
	if (IN_CLASSD(dstAddr)) {
		if (filtercfg.discardMulticast) {
			filterstats.f_insideMcast++;
			syslogMessage(SYSL_OUT_CLASSD,IP_PROTO_UDP,
				srcAddr, dstAddr,srcPort, dstPort);
			return DISCARD_PKT;
		}
		return ALLOW_PKT;
	}
	group = treeLookup(active.addrTree, srcAddr, active.num_groups);
    if(group>0&&group<active.num_groups)
	{
	portList = &active.u_dst_out[(group-1) * LIST_SIZE];
	}
	else
	   return	ALLOW_PKT;
	for (i=1; i <= portList[0].begin; i++) {
		if ((dstPort >= portList[i].begin) &&
		    (dstPort <= portList[i].end))
			return ALLOW_PKT;
	}
	if (dstPort >= 1024) {
		portList = &active.u_src_out[(group-1) * LIST_SIZE];
		for (i=1; i <= portList[0].begin; i++) {
			if ((srcPort >= portList[i].begin) &&
			    (srcPort <= portList[i].end))
				return ALLOW_PKT;
		}
	}
	filterstats.f_insideUdpPort++;
	syslogMessage(SYSL_OUT_PORT, IP_PROTO_UDP,
		      srcAddr, dstAddr, srcPort, dstPort);

	return DISCARD_PKT;
}

/* ------------------------------------------------------------------------ */
static ULONG
checkIncomingIcmp(ULONG srcAddr, ULONG dstAddr, unsigned char type)
{
	register ULONG i;
	PortListEntry *portList;

	portList = &active.i_typ_in[ (treeLookup(active.addrTree,
				dstAddr, active.num_groups)-1) * LIST_SIZE ];
	for (i=1; i <= portList[0].begin; i++) {
		if ((type >= portList[i].begin) &&
		    (type <= portList[i].end))
		{
			syslogMessage(SYSL_IN_TYPE,IP_PROTO_ICMP,srcAddr, dstAddr,NULL,NULL,type);
			filterstats.f_outsideIcmpType++;
			return DISCARD_PKT;
		}

	}

	return ALLOW_PKT;
}

/* ------------------------------------------------------------------------ */
static ULONG
checkOutgoingIcmp(ULONG srcAddr, ULONG dstAddr, unsigned char type)
{
	register ULONG i;
	PortListEntry *portList;
	//�ȴӵ�ַ���еõ��õ�ַ����Ӧ���������*LIST_SIZE��õ�icmp�����б��Ӧ���±�
	ULONG group;
	group=treeLookup(active.addrTree,srcAddr,active.num_groups);
	if(group>0&&group<active.num_groups)
	{
		portList=&active.i_typ_out[(group-1)*LIST_SIZE];
	}
	else//���δ���øõ�ַ��־
        return ALLOW_PKT;//δ�����������ƣ�Ĭ������

	//�жϱȽ�
	for (i=1; i <= portList[0].begin; i++) {
		if ((type >= portList[i].begin) &&
		    (type <= portList[i].end))
		{
     	  syslogMessage(SYSL_OUT_TYPE,IP_PROTO_ICMP,srcAddr, dstAddr,NULL,NULL, type);
	      filterstats.f_insideIcmpType++;
	      return DISCARD_PKT;
		}
	}

			return ALLOW_PKT;
}

/* ------------------------------------------------------------------------ */

static ULONG
checkRejectTable(struct ip *ipHeader, ULONG srcAddr, ULONG dstAddr)
{
  //�ܾ���Ϊ����ͨ�����еİ�
	if (active.rejectTree == NULL)
		return ALLOW_PKT;

	/* If srcAddr is in the tree and it's group is 1, then discard pkt */
	//�ܾ�ip������Ϊ1
	if (treeOutLookup(active.rejectTree, srcAddr, 2) == 1) {
		filterstats.f_outsideRejectTable++;
		syslogMessage(SYSL_IN_REJECT, (ULONG)ipHeader->Prot,
			srcAddr, dstAddr);
			
		return DISCARD_PKT;
	}

	/* Process by default */
	return ALLOW_PKT;
}

/* ------------------------------------------------------------------------ */
/*
 * Parameters 'protocol' and 'protolen' are expected to be in host byte order
 * The packet pointed to by 'pkt' is, of course, in network btye order
 */

static ULONG
checkIncomingPacket(unsigned short protocol/*����Э������*/, unsigned char *pkt/*����ͷ*/,
		    unsigned long protolen, ULONG checkAccept/*���������ܾ���*/)
{
	struct ip *ipHeader;
	struct tcphdr *tcpHeader;
	struct udphdr *udpHeader;
	struct icmp *icmpHeader;
	ULONG srcAddr, dstAddr;
	unsigned short ip_len,dstport,srcport;

	switch (protocol) 
	{

	case ETHER_PROTO_IP:
		ipHeader = (struct ip *)pkt;
		//12.4:�㲥������
		if(ipHeader->ip_dst.s_b[3]==255)
			return ALLOW_PKT;
		//12.4
	    srcAddr = ntohl(ipHeader->ip_src.ss_addr);
		dstAddr = ntohl(ipHeader->ip_dst.ss_addr);
		switch (ipHeader->Prot/*>ip_p*/)
		{
		 case IP_PROTO_TCP:
			 {
		     tcpHeader = (struct tcphdr *)(pkt + 
						(ipHeader->ip_hl<< 2));//ipHeader->ip_hl=5�����ƺ�Ϊ20(bytes);
			 dstport=ntohs(tcpHeader->th_dport);
			 srcport=ntohs(tcpHeader->th_sport);
			 }
			 break;
		 case IP_PROTO_UDP:
			 {
			 udpHeader = (struct udphdr *)(pkt + 
						(ipHeader->ip_hl << 2));
			 dstport=ntohs(udpHeader->uh_dport);
			 srcport=ntohs(udpHeader->uh_sport);
			 }
			 break;
		}
		/*
		 * Pass all IP fragments that do not have offset 0 (beginning
		 * of the packet) without checking since the TCP/UDP
		 * headers are not in this packet.
		 ��������£�TCP/UDPͷ���ڴ˰��У����޹�����Ϊ��ͨ��!
		 */
		   if ((ipHeader->Flgoff/*>ip_off*/ & SW_IP_OFFMASK) != 0)
		   {
			/*
			 * If option is set, then discard pkts with offset of 1
			 */
			  if (filtercfg.discardSuspectOffset &&
			    (ntohs(ipHeader->Flgoff/*ip_off*/ & SW_IP_OFFMASK) == 1) &&
			    (ipHeader->Prot/*>ip_p*/ == IP_PROTO_TCP)) 
			  {

				filterstats.f_outsideSuspectOffset++;
				syslogMessage(SYSL_IN_OFFSET, IP_PROTO_TCP,
					srcAddr, dstAddr);
					
				return DISCARD_PKT;
			  } 

			/*
			 * If option is set, then discard fragmented ICMP pkts
			 */
			//�����ֶε�icmp����
			   if (filtercfg.discardFragmentedICMP &&
			    (ipHeader->Prot/*>ip_p*/ == IP_PROTO_ICMP)) 
			   { 

				filterstats.f_outsideIcmpFrag++;
			    	syslogMessage(SYSL_IN_FRAG, IP_PROTO_ICMP,
					srcAddr, dstAddr);
					
				return DISCARD_PKT;
			   } 

			return ALLOW_PKT;
		   } 
         if(dstport==21||dstport==23||dstport==80||dstport==53)
		 {//������DM��
            if (checkIncomingDM(srcAddr,dstAddr,srcport,dstport,FROMOUT) == DISCARD_PKT)
			{
				filterstats.b_dmFiltered++;
//				filterstats.b_dmFiltered+=pktlen;
			    return IPF_DROP_PKT;
			}
			else 
			    return ALLOW_PKT;
		 }
		 else//�����������
		 {
		  if (checkAccept)
		  { //��������
			if (checkAcceptTable(ipHeader, srcAddr, dstAddr)
			    == DISCARD_PKT)
				return DISCARD_PKT;
		  }  
		  else 
		  {//���ܾ�����վ����
			if (checkRejectTable(ipHeader, srcAddr, dstAddr)
			    == DISCARD_PKT)
				return DISCARD_PKT;
		  }
        //���ip���ϲ�Э��
		 switch (ipHeader->Prot/*>ip_p*/)
		 { 

		 case IP_PROTO_TCP:
		/*	tcpHeader = (struct tcphdr *)(pkt + 
						(ipHeader->ip_hl<< 2));//ipHeader->ip_hl=5�����ƺ�Ϊ20(bytes);
			/*
			 * Make sure this packet (fragment) includes enough of
			 * the TCP header before making the other checks.
			 * Otherwise a SYN can sneak through since we might be
			 * trying to test something which is actually in the
			 * next fragment.
			 *
			 * NOTE: we drop all of these since we do not keep any
			 *       state between fragments.
			 *
			 * Many thanks to Uwe Ellermann at DFN-CERT for
			 * reporting this problem.
			 */
			//��֤��С��tcpͷ��:���ٰ���SYN
			ip_len = ntohs(ipHeader->TtlLen/*>ip_len*/);
			if ((ip_len >65535/* protolen*/) ||
			    ((ip_len - (ipHeader->ip_hl << 2)) < 14)) 
			{
//				filterstats.f_outsideShortTcpHdr++;
				syslogMessage(SYSL_IN_LENGTH, IP_PROTO_TCP,
					srcAddr, dstAddr,
					ntohs(tcpHeader->th_sport),
				ntohs(tcpHeader->th_dport));
	
				return DISCARD_PKT;
			}
            //���ڽ��������İ�����Ŀ�ĵ�ַΪ����ǽ���ʲ����˿�����	
			/* Check for "ACKless SYN" */
		/*	if ((tcpHeader->th_flags & (TH_SYN|TH_ACK)) == TH_SYN)
			{
				return checkIncomingTcp(srcAddr, dstAddr,
						ntohs(tcpHeader->th_sport),
						ntohs(tcpHeader->th_dport));
			}
			*/
			break;
//���udp
		case IP_PROTO_UDP:
		/*	udpHeader = (struct udphdr *)(pkt + 
						(ipHeader->ip_hl << 2));
			/*
			 * Make sure this packet (fragment) includes enough
			 * of the UDP header before making the other checks.
			 */
			//�����㹻��UDPͷ:���ٰ���srcPort,dstPort;
			ip_len = ntohs(ipHeader->TtlLen/*>ip_len*/);
			if ((ip_len >65535 /*protolen*/) ||
			    ((ip_len - (ipHeader->ip_hl<< 2)) < 4)) {
//				filterstats.f_outsideShortUdpHdr++;
				syslogMessage(SYSL_IN_LENGTH, IP_PROTO_UDP,
					srcAddr, dstAddr,
					ntohs(udpHeader->uh_sport),
					ntohs(udpHeader->uh_dport));

				return DISCARD_PKT;
			}

			/* Check UDP protocols. */
            //���ڽ��������İ�����Ŀ�ĵ�ַΪ����ǽ���ʲ����и���ļ��
			/*return checkIncomingUdp(srcAddr, dstAddr,
						ntohs(udpHeader->uh_sport),
						ntohs(udpHeader->uh_dport));
						*/
			break;
//ICMP
		case IP_PROTO_ICMP:
			if(filtercfg.discardIcmp)
			{
			    syslogMessage(SYSL_IN_ICMP, IP_PROTO_ICMP,
					srcAddr, dstAddr);
				return DISCARD_PKT;
			}
			icmpHeader = (struct icmp *)
				     (pkt + (ipHeader->ip_hl << 2));

			/*
			 * If option is set, then discard fragmented ICMP pkts
			 */
			if (filtercfg.discardFragmentedICMP &&
			    (ipHeader->Flgoff/*>ip_off */& IP_MF) != 0) 
			{
				filterstats.f_outsideIcmpFrag++;
			    	syslogMessage(SYSL_IN_FRAG, IP_PROTO_ICMP,
					srcAddr, dstAddr);
					

				return DISCARD_PKT;
			}

			/*
			 * If option is set, discard attack ICMP pkts.
			 *
			 * If the ICMP type, code, identifier, and seq num all
			 * equal 0, this may be an ICMP echo reply attack pkt.
			 */
			if (filtercfg.discardAttackICMP &&
			    (*((unsigned short *)icmpHeader) == 0) && /*type & code*/
			    (icmpHeader->icmp_void == 0))
			{      /*id & seq*/

				if (icmpHeader->icmp_cksum == 0xFFFF) 
				{
				
					/* This is a "smurf" attack */
					filterstats.f_outsideSmurfDos++;
			    		syslogMessage(SYSL_IN_DOS,NULL,
						      srcAddr, dstAddr,
						      DOS_SMURF);
							  
					return DISCARD_PKT;

				}
				else
					if (*((unsigned short *)icmpHeader->icmp_data)//?????????
					   == htons(0x4500)) 
					{
					/* This is a "pong" attack *///???????????????////???????????
					filterstats.f_outsidePongDos++;
			    		syslogMessage(SYSL_IN_DOS,NULL,
						      srcAddr, dstAddr,NULL,NULL,NULL,
						      DOS_PONG);
							  
					return DISCARD_PKT;
					}
			} 
            /*�����и���ļ��
			return checkIncomingIcmp(srcAddr, dstAddr,
						 icmpHeader->icmp_type);
						 */
			break;
		case IP_PROTO_IGMP:
			if((filtercfg.discardMulticast)||((ipHeader->Flgoff& IP_MF) != 0)) 
			{
			    syslogMessage(SYSL_IN_IGMP, IP_PROTO_IGMP,
					srcAddr, dstAddr);
			  return DISCARD_PKT;
			}
			else 
              return ALLOW_PKT; 
		default:
			if (filtercfg.discardOtherIp)
			{

				filterstats.f_outsideOtherIp++;
				syslogMessage(SYSL_IN_PROT, (ULONG)ipHeader->Prot,
					srcAddr, dstAddr);

				return DISCARD_PKT;
			}

			/* Everything is allowed so far. */
			break;
		 }
//first switch���
		break;
/*������ARP��RARP����
	case ETHER_PROTO_ARP:
	//case ETHER_PROTO_REVARP:
		/* pass these guys through no matter what */
//		break;


	default:
		/* Only pass the rest if told to do so. */
		if (filtercfg.discardNonIp)
		{

			/* Boy will this get ugly if the syslog mask is not */
			/* configured properly..... */

//			filterstats.f_outsideNonIp++;
			return DISCARD_PKT;
		}

	  } 

	return ALLOW_PKT;
	}
	return ALLOW_PKT;
}

/* ------------------------------------------------------------------------ */

static ULONG
checkAcceptTable(struct ip *ipHeader, ULONG srcAddr, ULONG dstAddr)
{
	/* If the accept table is empty, then forward all packets */
	if (active.acceptTree == NULL)
		return ALLOW_PKT;

	/* If srcAddr is in the tree and it's group is 1, then process pkt */
	//����ip������ҲӦ����Ϊ1
	if (treeLookup(active.acceptTree, srcAddr, 2) == 1)
	{
	/* Discard by default */
	filterstats.f_insideAcceptTable++;
	syslogMessage(SYSL_OUT_ACCEPT, (ULONG)ipHeader->Prot,
			srcAddr, dstAddr);
	  return DISCARD_PKT;
	}
	return ALLOW_PKT;
}

/* ------------------------------------------------------------------------ */
/*
 * Parameters 'protocol' and 'protolen' are expected to be in host byte order
 * The packet pointed to by 'pkt' is, of course, in network btye order
 */
//��ȥ��ֻ��������
static ULONG
checkOutgoingPacket(unsigned short protocol, unsigned char *pkt, unsigned short protolen)
{

	struct ip *ipHeader;
	struct tcphdr *tcpHeader;
	struct udphdr *udpHeader;
	struct icmp *icmpHeader;
	unsigned short ip_len,dstport,srcport;
	ULONG srcAddr, dstAddr;

	switch (protocol)
	{
	case ETHER_PROTO_IP:
		ipHeader = (struct ip *)pkt;
		//12.4:�㲥������
		if(ipHeader->ip_dst.s_b[3]==255)
			return ALLOW_PKT;
		//12.4
		srcAddr = ntohl(ipHeader->ip_src.ss_addr);
		dstAddr = ntohl(ipHeader->ip_dst.ss_addr);
		//���Ƚ��й������
		/*
		 * Pass all IP fragments that do not have offset 0 (beginning
		 * of the packet) without checking since the TCP/UDP
		 * headers are not in this packet.  An area for improvement
		 * would be to cache session info so we could drop all
		 * disallowed fragments also instead of just the first one.
		 ���������(ƫ�������ڣ�0~224��)��TCP/UDPͷ���ڴ˰��У����޹�����Ϊ��ͨ��!
		 */
		if ((ipHeader->Flgoff/*>ip_off*/ & SW_IP_OFFMASK) != 0)
		{//��first���ݰ�SW_IP_OFFMASK:ntohs(0xFF1F)=0x1FFF:��Ϊǰ��λΪ��־
			if (filtercfg.discardSuspectOffset &&
			    (ntohs(ipHeader->Flgoff/**/ & SW_IP_OFFMASK) == 1) &&//ipHeader->Flgoff:0X0100~0X01E0(256~480)
			    (ipHeader->Prot/*>ip_p*/ == IP_PROTO_TCP)) 
			{

				filterstats.f_insideSuspectOffset++;
				syslogMessage(SYSL_OUT_OFFSET, IP_PROTO_TCP,
					srcAddr, dstAddr);
				return DISCARD_PKT;
			}

			/*
			 * If option is set, then discard fragmented ICMP pkts
			 */
			if (filtercfg.discardFragmentedICMP &&
			    (ipHeader->Prot/*>ip_p*/ == IP_PROTO_ICMP)) 
			{

				filterstats.f_insideIcmpFrag++;
			    	syslogMessage(SYSL_OUT_FRAG, IP_PROTO_ICMP,
					srcAddr, dstAddr);
					
				return DISCARD_PKT;
			}

			return ALLOW_PKT;
		}
        //�ǵ�һ��ip��Ƭ�������ܰ����ϲ�Э�鱨ͷ����˼���ϲ�Э��
		switch (ipHeader->Prot/*>ip_p*/)
		 {
		  case IP_PROTO_TCP:
			  {
		     tcpHeader = (struct tcphdr *)(pkt + 
						(ipHeader->ip_hl<< 2));//ipHeader->ip_hl=5�����ƺ�Ϊ20(bytes);
			 dstport=ntohs(tcpHeader->th_dport);
			 srcport=ntohs(tcpHeader->th_sport);
			  }
			 break;
		  case IP_PROTO_UDP:
			  {
			 udpHeader = (struct udphdr *)(pkt + 
						(ipHeader->ip_hl << 2));
			 dstport=ntohs(udpHeader->uh_dport);
			 srcport=ntohs(udpHeader->uh_sport);
			  }
			 break;
		 }
        if(dstAddr==filtercfg.dm_ip)//����dm���
		{
		   if(checkIncomingDM(srcAddr,dstAddr,srcport,dstport,FROMIN) == DISCARD_PKT)
			{
				filterstats.b_dmFiltered++;
		//		filterstats.b_dmFiltered += pktlen;
			    return IPF_DROP_PKT;
			}
			else 
		    	return ALLOW_PKT;
		}
		else//�����������
		{
		if (checkAcceptTable(ipHeader,srcAddr,dstAddr) == DISCARD_PKT)
			return DISCARD_PKT;
		switch (ipHeader->Prot/*>ip_p*/) 
		{
		case IP_PROTO_TCP:
			tcpHeader = (struct tcphdr *)(pkt + 
						(ipHeader->ip_hl<< 2)); 
			/*
			 * Make sure this packet (fragment) includes enough
			 * of the TCP header before making the other checks.
			 */
			ip_len = ntohs(ipHeader->TtlLen/*>ip_len*/);
			if ((ip_len >65535/* protolen*/) ||
			    ((ip_len - (ipHeader->ip_hl<< 2)) < 14))
			{
//				filterstats.f_insideShortTcpHdr++;
				syslogMessage(SYSL_OUT_LENGTH, IP_PROTO_TCP,
					srcAddr, dstAddr,
					ntohs(tcpHeader->th_sport),
					ntohs(tcpHeader->th_dport));

				return DISCARD_PKT;
			}

			/* Check for "ACKless SYN" */
			if ((tcpHeader->th_flags & (TCP_SYN|TCP_ACK)) == TCP_SYN)
			{
				return checkOutgoingTcp(srcAddr, dstAddr,
						ntohs(tcpHeader->th_sport),
						ntohs(tcpHeader->th_dport));
			}
			break;

		case IP_PROTO_UDP:
			udpHeader = (struct udphdr *)(pkt + 
						(ipHeader->ip_hl << 2));
			/*
			 * Make sure this packet (fragment) includes enough
			 * of the UDP header before making the other checks.
			 */
			ip_len = ntohs(ipHeader->TtlLen);
			if ((ip_len >65535 /*protolen*/) ||
			    ((ip_len - (ipHeader->ip_hl << 2)) < 4)) 
			{
//				filterstats.f_insideShortUdpHdr++;
				syslogMessage(SYSL_OUT_LENGTH, IP_PROTO_UDP,
					srcAddr, dstAddr,
					ntohs(udpHeader->uh_sport),
					ntohs(udpHeader->uh_dport));

				return DISCARD_PKT;
			}

			return  checkOutgoingUdp( srcAddr, dstAddr,
					ntohs(udpHeader->uh_sport),
					ntohs(udpHeader->uh_dport));
        case IP_PROTO_IGMP:
			if(filtercfg.discardMulticast)
			{
			    syslogMessage(SYSL_OUT_IGMP, IP_PROTO_IGMP,
					srcAddr, dstAddr);
				return DISCARD_PKT;
			}
			else 
				return ALLOW_PKT;
		case IP_PROTO_ICMP:
			if(filtercfg.discardIcmp)
			{
			    syslogMessage(SYSL_OUT_ICMP, IP_PROTO_ICMP,
					srcAddr, dstAddr);
				return DISCARD_PKT;
			}
			icmpHeader = (struct icmp *)
				     (pkt + (ipHeader->ip_hl<< 2));

			/*
			 * If option is set, then discard fragmented ICMP pkts
			 */
			if (filtercfg.discardFragmentedICMP &&
			    (ipHeader->Flgoff/*>ip_off*/ & IP_MF) != 0)
			{//IP_MF:0x4000;��־��ĵڶ�λΪ1��˵���к����ķ�Ƭ
				filterstats.f_insideIcmpFrag++;
			    	syslogMessage(SYSL_OUT_FRAG, IP_PROTO_ICMP,
					srcAddr, dstAddr);
					

				return DISCARD_PKT;
			}

			/*
			 * If option is set, discard attack ICMP pkts.
			 *
			 * If the ICMP type, code, identifier, and seq num all
			 * equal 0, this may be an ICMP echo reply attack pkt.
			 */
			if (filtercfg.discardAttackICMP &&
			    (*((unsigned short *)icmpHeader) == 0) && /*type & code*/
			    (icmpHeader->icmp_void == 0))
			{      /*id & seq*/

				if (icmpHeader->icmp_cksum == 0xFFFF)
				{
				
					/* This is a "smurf" attack */
					filterstats.f_insideSmurfDos++;
			    		syslogMessage(SYSL_OUT_DOS,NULL,
						      srcAddr, dstAddr,NULL,NULL,NULL,
						      DOS_SMURF);
							  
					return DISCARD_PKT;

				} 
				else
					if (*((unsigned short *)icmpHeader->icmp_data)
					   == htons(0x4500))//69
					{
					/* This is a "pong" attack */
					filterstats.f_insidePongDos++;
			    		syslogMessage(SYSL_OUT_DOS,NULL,
						      srcAddr, dstAddr,NULL,NULL,NULL,
						      DOS_PONG);
							  
					return DISCARD_PKT;
					}
			}

			return checkOutgoingIcmp(srcAddr, dstAddr,
						 icmpHeader->icmp_type);

		default:
			if (filtercfg.discardOtherIp)
			{

				filterstats.f_insideOtherIp++;
				syslogMessage(SYSL_OUT_PROT,(ULONG)ipHeader->Prot,
					srcAddr, dstAddr);

				return DISCARD_PKT;
			}

			/* Everything is allowed so far. */
			break;
		}
      }
		break;

//	case ETHER_PROTO_ARP:
//	case ETHER_PROTO_REVARP:
		/* pass these guys through no matter what */
//		break;

	default:
		/* Only pass the rest if told to do so. */
		if (filtercfg.discardNonIp) {
//			filterstats.f_insideNonIp++;
	//		syslogMessage(SYSL_OUT_FILTER, (ULONG)protocol);
		return DISCARD_PKT;
		}
	}

	return ALLOW_PKT;
}

/* ------------------------------------------------------------------------ */
/* char      dev_instance;   //�����豸���
   char      direction;       //��������
                               // 1 ��վ
 							   // 2 ��վ  
   struct ether_header  h;    //��̫֡ͷ��ָ��
   struct mbuf    m;          //���ݻ�������ָ��
*/
ULONG
ipfilter_test_pkt(USHORT dev_instance, UCHAR direction,ULONG ethlen,/* struct ether_header *h,*/BYTE *m)
{
	unsigned short protocol,protolen;
	unsigned char *pkt;
	unsigned long  pktlen;
	struct ip *ipHeader;
	ULONG srcAddr, dstAddr;
//	unsigned short ip_len;
	 //�����������û��������ͨ�����а�
	if (!filterstats.running)
		return IPF_PROCESS_PKT;
    struct Ether_Header *h=(struct Ether_Header *)m;

	//�õ���̫֡��Э��ţ���̫֡�����ݲ��֣����ݲ��ֳ�����̫֡�ܳ�
	protocol = ntohs(h->ether_type);
	pkt = m+sizeof(struct Ether_Header);
	protolen =ethlen-sizeof(struct Ether_Header)/* m->m_pkthdr.len*/;
	pktlen =ethlen;

	//ֻ��IP�����й��˼�飬��IP������ͨ��
	if (protocol != ETHER_PROTO_IP) 
		return IPF_PROCESS_PKT;
	else 
	{//�����IP������õ�IP����Ŀ�ĵ�ַ��Դ��ַ      
		ipHeader = (struct ip *)pkt;
		if(ipHeader->ip_hl>5)
		{
		    syslogMessage((SYSL_OUT_ROUT,ntohs(ipHeader->Prot),srcAddr,dstAddr));		             
			return IPF_DROP_PKT;
		}

//		static iii;
//		iii++;
//		TRACE("AAA%d\n",iii);
		srcAddr = ntohl(ipHeader->ip_src.ss_addr);
		dstAddr = ntohl(ipHeader->ip_dst.ss_addr);
	}
	if (dev_instance == filtercfg.in_number)//�ж��豸��
	{   /* �����ڲ��ӿڵİ� */
	    ULONG rc;
		filterstats.p_insideRx++;
		filterstats.b_insideRx += pktlen;		
		//��������IP��ַ��ð�İ�
		if (srcAddr&filtercfg.in_mask == dstAddr&filtercfg.in_mask) //��ͬһ����
		{
		//�ڴ˼��� if (ipHeader->ip_hl != 5) {
		//          if (filtercfg.discardRouteIP){ 
		//             DBstat.f_insideRouteIP++;
//		    syslogMessage((SYSL_OUT_ROUT,ntohs(ipHeader->Prot),srcAddr,dstAddr));		             
			return IPF_DROP_PKT;
		}
			

		//����������DM�����
		//��������
		rc = checkOutgoingPacket(protocol,pkt,protolen);
		if (rc != ALLOW_PKT)
		{
			filterstats.p_insideFiltered++;
			filterstats.b_insideFiltered += pktlen;
/*			if (rc == DISCARD_AND_ANSWER_PKT)
				return IPF_DROP_AND_ANSWER_PKT;
				*/
			return IPF_DROP_PKT;
		}
		filterstats.p_insideTx++;
		filterstats.b_insideTx+= pktlen;
		return IPF_FORWARD_PKT;
		//end
	}
	 else
	 {
		if (dev_instance == filtercfg.out_number) 
		{  /* from the outside interface1 */ //�����ⲿ�ӿڵİ�
		 ULONG rc;
		 filterstats.p_outsideRx++;
		 filterstats.b_outsideRx += pktlen;
        
		 if (srcAddr&&filtercfg.out_mask == dstAddr&&filtercfg.out_mask) 
			return IPF_DROP_PKT;
	     
		 //��������������DM��
         rc = checkIncomingPacket(protocol,pkt,protolen,0);//0:���ܾ���
   	     if (rc != ALLOW_PKT)
		 {
			filterstats.p_outsideFiltered++;
			filterstats.b_outsideFiltered += pktlen;
			if (rc == DISCARD_AND_ANSWER_PKT)
				return IPF_DROP_AND_ANSWER_PKT;
			return IPF_DROP_PKT;
		 }

	    filterstats.p_outsideTx++;
		filterstats.b_outsideTx+= pktlen;
		return IPF_FORWARD_PKT;
		}
	 }
	/*
	 * The pkt is not from either filter interface so process it normally
	 */
	 //��dm�����İ��ٶ�Ϊʼ�պϷ�
	return DB_PROCESS_PKT;
}
VOID   ipfilter_pkt(ULONG ethlen,BYTE *m)
{
//			TRACE("initialized=%d\n",initialized);
            if(initialized)
			{
				  USHORT dev_instance=m[1];//test
		          UCHAR  direction;//test
		          direction=m[0];
		//		  TRACE("direction=%d\n",direction);
				  if(direction==0)//����������
				  {
					static int ii;
                    ULONG ret;
		            ret=ipfilter_test_pkt(dev_instance,direction,1600,m+2);
		            switch(ret)
					{      
		               case IPF_DROP_PKT:
	                   case IPF_DROP_AND_ANSWER_PKT:
                           TRACE("memset%d\n",ii);
						   ii++;
						   memset(&m[2],0,1598);
						   break;
			        
		               case  DB_PROCESS_PKT:
		               case  IPF_PROCESS_PKT:
					   case  IPF_FORWARD_PKT:
						   break;
		               default:
						   TRACE("default\n");
			                break;
					}
				  } 
			}

}
/*------------------------------------------------------------------------ */
/*
 * Sends tcp reset packet.
 */
//��rout���ͳ�ȥ��������������������������������������������������
void
ipfilter_response_pkt(BYTE *rout,ULONG outlen,ULONG inlen/* void *inh, */,BYTE *min)
{
	memcpy(rout,min,inlen);
	ULONG tmp;
	register struct pkt_struct *out_pkt;
	register struct pkt_struct *in_pkt;

	/* Outbound packet's media header */
	if (filtercfg.media_type == IFT_ETHER)
	{
		register struct Ether_Header *in_h =(struct Ether_Header *)/*gj*/ min;
//		register struct Ether_Header *out_h =mtod(r, struct Ether_Header *);
//	    register struct Ether_Header *out_h =mtod(r);////gj	
  //      register struct Ether_Header *out_h =mtod(r);////gj	
        register struct Ether_Header *out_h =(struct Ether_Header *)rout;////gj
		//����ԭ��Ŀ��
		memcpy(&out_h->ether_dhost, &in_h->ether_shost, 6);
		memcpy(&out_h->ether_shost, &in_h->ether_dhost, 6);
		out_h->ether_type = in_h->ether_type;

//		rout->m_data += sizeof(struct ether_header);

	} 
	
/*	else
	{ /* IFT_FDDI */
/*		register struct fddi_header *in_h = (struct fddi_header *)h;
		register struct fddi_header *out_h =mtodFddi(r);//gj

		memcpy(out_h->fddi_dhost, in_h->fddi_shost, 6);
		memcpy(out_h->fddi_shost, in_h->fddi_dhost, 6);
		out_h->fddi_fc = in_h->fddi_fc;

		r->m_data += sizeof(struct fddi_header);
		memcpy(r->m_data, m->m_data, LLC_SNAPLEN);
		r->m_data += LLC_SNAPLEN;
		m->m_data += LLC_SNAPLEN;
	}
*/
	/* Setup pointers and clear outbound packet */
//	in_pkt = mtod(m, struct pkt_struct *);
//	out_pkt = mtod(r, struct pkt_struct *);
	in_pkt = mtodPkt(min);//����̫֡����ת��Ϊstruct pkt_struct
	out_pkt = mtodPkt(rout);
//	rout->m_len = sizeof(struct pkt_struct);
	memset(out_pkt,0, sizeof(struct pkt_struct));

	/* IP header fields included in the TCP checksum */
	out_pkt->i.TtlLen = htons(20);		/* TCP len */
	out_pkt->i.Prot = IP_PROTO_TCP;
	out_pkt->i.ip_src = in_pkt->i.ip_dst;
	out_pkt->i.ip_dst = in_pkt->i.ip_src;

	/* TCP header */
	out_pkt->t.th_sport = in_pkt->t.th_dport;
	out_pkt->t.th_dport = in_pkt->t.th_sport;
	out_pkt->t.th_seq = in_pkt->t.th_ack;
	tmp = ntohl(in_pkt->t.th_seq) + 1;
	out_pkt->t.th_ack = htonl(tmp);
	out_pkt->t.th_off = 5;
	out_pkt->t.th_flags = 0x14;//ack,rst

	/* TCP checksum */
	out_pkt->t.th_sum = in_cksum_tcp(rout);//gj:��Ӻ�����

	/* Finish the IP header */
	out_pkt->i.ip_v = 4;
	out_pkt->i.ip_hl = 5;			/* IP hdr len >> 2 */
	out_pkt->i.TtlLen = htons(40);		/* Total length */
	out_pkt->i.TTL= 128;

	/* IP header checksum */
	out_pkt->i.ChkSum = in_cksum_hdr((BYTE*)&out_pkt);
//��δ�ı���̫֡ͷ������
//��rout���ͳ�ȥ��������������������������������������������������

	/* Adjust outbound mbuf to include media header */
/*	if (filtercfg.media_type == IFT_ETHER) {
		rout->m_data -= sizeof(struct ether_header);
		rout->m_len += sizeof(struct ether_header);
	}
	*/
	/*else {
		r->m_data -= (sizeof(struct fddi_header) + LLC_SNAPLEN);
		r->m_len += (sizeof(struct fddi_header) + LLC_SNAPLEN);
	}
	*/
//	rout->m_pkthdr.len = rout->m_len;
	//����������������������������������������������������������������
}

/* ------------------------------------------------------------------------ */

static ULONG
initRequest(initReq *req)
{
	//������е�״̬��Ϣ��������־��Ϣ��
    memset(&filterstats, 0, sizeof(filterstats));
	hlogFile=fopen("��־.txt","a");
	statFile=fopen("ͳ��.txt","a");

	//����ȱʡ����
	initTables(&active, 10);//1:��ʼ��Ϊ10�����
	//:����Ҫ����ʱ��̬�����������������initTables(&active, 10)�е����ã�ʱ
	//�������ô�����
	//ÿ����������LIST_SIZE==32��PortListEntry
    initTables(&newspace, 10);
	GetLocalTime(&filterstats.starttime);
	initialized = YES;

	return NOO_ERROR;
}

//��ַ������˳��
//addrTree�ǹ����õġ�
//�������ã������ܾ������ã����������Χ���ɳ���255�������Ų���ȫ0��ȫ1����(1~254)
//��Ϊ������Χ�ܹ�
//��Ӧ����������������������
static ULONG
OutnetaddToTree(OutaddrTreeNode *tbl, in_addr_t addr,/*��������ʼ��ַ,*/ 
			unsigned short hostnums,/*����������ʼ��ַ��ʼ��������,*/
			unsigned short group/*�����˿��б�����һ��*/)
{
	int i, start, end, n, level;
	addrTreePtr *ptr=(addrTreePtr *)malloc(sizeof(addrTreePtr));//gj
        if (!tbl || ((256-addr.s_b[0])<hostnums))
           return -1;

	//��ַ��Χ
    start=addr.s_b[0];
	end=start+hostnums;
	for (i=start; i<end; i++) 
	{
		ptr->p=tbl;
		addr.s_b[0] = i;
		// 'byte' ˵�����ж��� 
		//'level'��¼�����ںδ�: 0, 1, 2, or 3 
        for (n=0, level=0; n < 4; n++, level++)
		{
		//IP��ַ����һ��8λ��Ϊ�����������������ָ����������
		 unsigned char b =addr.s_b[3 - level];
			// ���n=3, ���һ���ֽ�
			if (n == 3) 
			{
				if (level == 3) 
				{
					//Level 3ֻ������
					ptr->pd->c[b] = group;				
				} 
				else
				{
					// Levels 0-2 ������һ���������һ��ָ��
					if (!ptr->p->flg[b] && ptr->p->u[b].p)//����Υ��???????????
					freeOutTree(ptr->p->u[b].p,level+1);
                    //�ͷ�ָ��󣻴洢���
					ptr->p->u[b].p = NULL;
                    ptr->p->u[b].c = group;
					ptr->p->flg[b] = 1;
				}
			//һ��ָ��
			}
			else
			{
		     if ((!ptr->p->flg[b])&&(ptr->p->u[b].p))//����Υ��??????????
				{//��Ӧ�ı�־Ϊ0��ͬʱ��Ӧ����һ�ڵ�ָ�����
				   //���������ƶ�һ�����
				ptr->p = ptr->p->u[b].p;
				}// ��Ӧ�ı�־Ϊ1��	��Ӧ����һ�ڵ�ָ�벻����
				else
				{
				 unsigned short j;
				 addrTreePtr next;
				 if (level == 2)
				 {
					unsigned int tmp_c = ptr->p->u[b].c;
					if (!(next.pd = MALLOC_LEAF))//���һ���ض�����
						return 1;
					for (j=0; j<256; j++)
					{
						next.pd->c[j] = tmp_c;
					}
				 }
				 else //level=0��1
				 {
					void *tmp_p = ptr->p->u[b].p;
					unsigned char tmp_f = ptr->p->flg[b];
					if (!(next.p = MALLOC_OUTNODE))
						return 1;
					//��next��ÿһ��Ա��ֵ;��ӵ�����ǰ��
					for (j=0; j<256; j++) 
					{
						next.p->u[j].p = (OutaddrTreeNode*)tmp_p;
						next.p->flg[j] = tmp_f;
					}
					int gj=0;
				}//������һ���ڵ㣬������־λ����Ϊ0ָʾΪһ��ָ��
				ptr->p->u[b].p = next.p;//��ӵ�������һ�ڵ�
				ptr->p->flg[b] = 0;
				ptr->p = next.p;//����ʱָ��ָ��������һ�ڵ�
				}
			}//end else(2.1)
		}//end for(2)
	}//end for(1)
	free(ptr);
	return 0;
}

VOID clearipfilter()
{
	if(initialized==YES)
	{
	freeAllPointers(&active);
	freeAllPointers(&newspace);
/*	fwrite(filterlog.str,sizeof(char),filterlog.pos*100,hlogFile);
	*/
	writestatistic();
	fprintf(statFile,"\n");
	fclose(statFile);
	fclose(hlogFile);
	initialized=NO;
	memset(&filterstats,0,sizeof(Statistics)); 	
	memset(&filtercfg,0,sizeof(FilterConfig)); 
    filtercount=0;
	}

}
//��ַ������˳��
//addrTree�ǹ����õġ�
//�������ã����������Χ�ɳ���255��//�����Ų���ȫ0��ȫ1������(1~254)

static ULONG
GjaddToTree(addrTreeNode *tbl, in_addr_t addr/*��������ʼ��ַ*/, 
			unsigned short hostnums/*����������ʼ��ַ��ʼ��������*/,
			unsigned short group/*�����˿��б�����һ��*/)
{
	unsigned short   start, end,i,n, level,total,remove,remain;
	UCHAR b;
	BOOL bfirst=TRUE;
	addrTreeLeaf *ptrleaf;
        if (!tbl)
           return -1;
	//��ַ��Χ
	start=addr.s_b[0];
	total=start+hostnums;
	if(total>255)
	{
		end=255;
		remove=255-start;
	}
	else
	{
		end=total;
        remove=hostnums;
	}
    do
	{
    if(!bfirst)
	{
		start=0;
		addr.s_b[1]++;
	}
	for (i=start; i<end; i++) 
	{
		addr.s_b[0] = i;
		//'level'��¼�����ںδ�: 0, 1
        for (n=0, level=0; n < 2; n++, level++)
		{
		//IP��ַ����һ��8λ��Ϊ�����������������ָ����������
		  b=addr.s_b[1- level];
		  if(n==1)
		  {
			 ptrleaf->c[b]=group;
		  }
		  else
		  {
		    if(!tbl->u[b].pd)
			{
			 tbl->u[b].pd=MALLOC_LEAF;
			}
		    ptrleaf=tbl->u[b].pd;
		  }
		}
	}
	bfirst=FALSE;
	remain=hostnums-remove;
	if(remain>255)
	{
		end=255;
	    remove+=256;
	}
	else
	{
		end=remain;	
		remove+=remain;
	}
	}while(remain);
	return 0;
}
static void
freeOutTree(OutaddrTreeNode *tbl, unsigned char level)
{
        int i;

        if (!tbl || (level > 3)) 
		{
		//��������
                return;
		}

        if (level == 3)
		{
		  free((addrTreeLeaf *)tbl);
		  return;
		}

        for (i=0; i<256; i++) 
		{
                if (!tbl->flg[i] && tbl->u[i].p)
				{
			        if (level == 2)
				       free(tbl->u[i].pd);
			        else
				        freeOutTree(tbl->u[i].p, level + 1);//�ݹ�ķ�����������
				}
		}

	free(tbl);
}
/* ------------------------------------------------------------------------ */
//��������������������������
ULONG
filter_command( unsigned long cmd, char* initinfo)
{
	ULONG error = 0;
	//δ����ʼ����IP�����������ܽ�����������
	if (initialized == 0) {
		if ((cmd != DIOCINIT)&&(cmd != DIOCGLOGM) &&
	    	    (cmd != DIOCSLOGM)) {
			return ERROR_NOT_INITILIZED; /* û�г�ʼ�� */
		}
	}

	switch (cmd) {

	case DIOCINIT:		/* ��ʼ�� */		
		if (filterstats.running)
		{
			error = ERROR_RUNNING;
			break;
		}
		error = initRequest((initReq *)initinfo);//!!!!!!!!!!!!!!!!!!!!!!!!!!!
		break;

	case DIOCSTART:		// ���� 		
		if (filterstats.running) 
		{
			error = ERROR_RUNNING;   //�Ѿ�����
			break;
		}	
		filterstats.running = YES;
		break;

	case DIOCSTOP:		// ֹͣ		
		if (!filterstats.running) 
		{		
			error = ERROR_NOT_RUNNING; // û������ 
			break;
		}
		filterstats.running = NO;
		break;
			
	//���ͳ�ƽ��//>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
	case DIOCGSTATS:	
		*((Statistics *)initinfo) = filterstats;
		break;

    //���ͳ����Ϣ
	case DIOCCSTATS:	
        memset(&filterstats, 0, sizeof(filterstats));
		break;

	//���뵽�����
	case DIOCSGROUP://!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		initTables(&newspace, u->val);//���ڽ�����һ������������
		error = portListRequest((portListReq *)initinfo, &newspace);
		break;

	//���뵽�������
	case DIOCSNETWORK:	
		error = addrTreeRequest((addrTreeReq *)initinfo);
		break;
		  
	//�޸������
	case DIOCMNETWORK:	
		error = manageTablesRequest((manageTablesReq *)initinfo);
		break;

	//���뵽�ܾ�����
	case DIOCSREJECT:
		error = OutrejaccTableRequest(T_REJTBL,
					   (rejaccTableReq *)initinfo,&newspace );
		break;
   //���뵽�������
	case DIOCSACCEPT:
		error = rejaccTableRequest(T_ACCTBL,
					   (rejaccTableReq *)initinfo, &newspace);
		break;
    case DIOCS_INDM:
		error = rejaccTableRequest(T_INDM,
					   (rejaccTableReq *)initinfo, &newspace);
		break;
    case DIOCS_OUTDM:
		error = OutrejaccTableRequest(T_OUTDM,
					   (rejaccTableReq *)initinfo, &newspace);
		break;
	default:
		error = ERROR_INVALID_OP; //����Ĳ���
		break;

	}
	return error;
}
void writestatistic()
{
    Statistics StatisticInfo;
    GetStatisticsInfo(&StatisticInfo);
	fprintf(statFile,"��ʼʱ�䣺%d��%d��%d:%d:%d\t"
		,StatisticInfo.starttime.wMonth,StatisticInfo.starttime.wDay,
		StatisticInfo.starttime.wHour,
		StatisticInfo.starttime.wMinute,StatisticInfo.starttime.wSecond);
	SYSTEMTIME systemtime;
	GetLocalTime(&systemtime);
	fprintf(statFile,"����ʱ�䣺%d��%d��%d:%d:%d\t\n"
		,systemtime.wMonth,systemtime.wDay,systemtime.wHour,
		 systemtime.wMinute,systemtime.wSecond);

	fprintf(statFile,"��վ������%d"  "�ֽ�����%d"  "ͨ��������������%d"  "�ֽ�����%d"  "���˵�������������%d"  "�ֽ�����%d,\n��վ������%d"  "�ֽ�����%d"  "ͨ��������������%d"  "�ֽ�����%d"  "���˵�������������%d"  "�ֽ�����%d,\n"
		,StatisticInfo.p_insideRx,StatisticInfo.b_insideRx
		,StatisticInfo.p_insideTx,StatisticInfo.b_insideTx
		,StatisticInfo.p_insideFiltered,StatisticInfo.b_insideFiltered
		,StatisticInfo.p_outsideRx,StatisticInfo.b_outsideRx
		,StatisticInfo.p_outsideTx,StatisticInfo.b_outsideTx
		,StatisticInfo.p_outsideFiltered,StatisticInfo.b_outsideFiltered
		);
	fprintf(statFile,"��DM������%d"  "�ֽ�����%d"  "ͨ��DM�İ�����%d"  "�ֽ�����%d"  "DM���˵��İ�����%d"  "�ֽ�����%d,\n"
	    ,StatisticInfo.p_dmRx,StatisticInfo.b_dmRx
		,StatisticInfo.p_dmFiltered,StatisticInfo.b_dmFiltered
		,StatisticInfo.p_dmTx,StatisticInfo.b_dmTx
		);
	fprintf(statFile,"����:\n\t��ֹIP�İ�����%d"  "��ֹICMP�ֶεİ�����%d"  "��ֹICMP���͵İ�����%d"  "pong����������%d,\n"  "smurf�����İ�����%d"  "����offset�İ�����%d"  "��ֹtcp�˿ڵİ�����%d"  "��ֹudp�˿ڵİ�����%d\n��ֹ�鲥�İ�����%d"  "��ֹ���IP������%d\n"
		,StatisticInfo.f_insideAcceptTable,StatisticInfo.f_insideIcmpFrag
		,StatisticInfo.f_insideIcmpType,StatisticInfo.f_insidePongDos
		,StatisticInfo.f_insideSmurfDos,StatisticInfo.f_insideSuspectOffset
		,StatisticInfo.f_insideTcpPort,StatisticInfo.f_insideUdpPort
		,StatisticInfo.f_insideMcast,StatisticInfo.f_insideOtherIp);
	 fprintf(statFile,"����:\n\t��ֹIP�İ�����%d"  "��ֹICMP�ֶεİ�����%d"  "��ֹICMP���͵İ�����%d"  "pong����������%d\nsmurf�����İ�����%d"  "����offset�İ�����%d"  "��ֹ�鲥�İ�����%d\n"	"��ֹ���IP�İ�����%d\n"
		,StatisticInfo.f_outsideRejectTable,StatisticInfo.f_outsideIcmpFrag
		,StatisticInfo.f_outsideIcmpType,StatisticInfo.f_outsidePongDos
		,StatisticInfo.f_outsideSmurfDos,StatisticInfo.f_outsideSuspectOffset
		,StatisticInfo.f_outsideMcast,StatisticInfo.f_outsideOtherIp);
}
VOID GetStatisticsInfo(Statistics* StatisticInfo)
{
	filter_command(DIOCGSTATS,(char*)StatisticInfo);
}
VOID mysearch()
{
/*	ULONG group0,group1,group2,group3,group4,group5,group6,group7,group8,group9;
	//û���򷵻�0
	//������
	group0 = treeLookup(active.addrTree, ntohl(inet_addr("192.168.130.205")), active.num_groups);
	group1 = treeLookup(active.addrTree, ntohl(inet_addr("192.168.0.13")), active.num_groups);
	group1 = treeLookup(active.addrTree, ntohl(inet_addr("192.168.0.14")), active.num_groups);
	group2=treeLookup(active.addrTree, ntohl(inet_addr("192.168.3.11")), active.num_groups);//no
	group2=treeLookup(active.addrTree, ntohl(inet_addr("192.168.11.11")), active.num_groups);//no
	//������
	group3 = treeLookup(active.acceptTree, ntohl(inet_addr("192.168.130.205")), active.num_groups);
	group4 = treeLookup(active.acceptTree, ntohl(inet_addr("192.168.0.21")), active.num_groups);//no
	group4 = treeLookup(active.acceptTree, ntohl(inet_addr("192.168.13.21")), active.num_groups);//no
	//�ܾ���
	group5 = treeOutLookup(active.rejectTree, ntohl(inet_addr("192.168.130.205")), active.num_groups);
	group6 = treeLookup(active.acceptTree, ntohl(inet_addr("192.168.13.21")), active.num_groups);//no
	group7= treeLookup(active.In_FtpTree, ntohl(inet_addr("111.11.111.11")), active.num_groups);//no
	group8 = treeLookup(active.In_HttpTree, ntohl(inet_addr("111.11.111.11")), active.num_groups);//no
	group9 = treeLookup(active.In_SmtpTree, ntohl(inet_addr("111.11.111.11")), active.num_groups);//no
*/
}
