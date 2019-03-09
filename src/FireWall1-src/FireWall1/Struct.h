

#if !defined(FIREWALLSTRUCT)
#define FIREWALLSTRUCT

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//�豸����IP��ַ��ӳ��
struct MapDevToIp{
          int  nDevNum;
          ULONG nIpAddr;
};
//��ʱ���ã��ö�̬�˿�NAT
struct TimeOut{
          int nTcpTimeOut;//Сʱ
          int nUdpTimeOut;//����
};

//(��̬����̬)����NATӳ��
struct MapAddrNat{
          unsigned long nIntraLocalAddr;//����ip��ַ
          unsigned long nOutsideGlobalAddr;//����ip��ַ
};

// ��̬�˿�NAT����
struct DynamicPortNat{
          USHORT  nMinPort;//��MaxPortһ���ʾ�˿�ת����Χ
          USHORT  nMaxPort;
          USHORT  nMaxPortConversion;//�˿����ת����
          struct TimeOut sTimeOut;// ��ʱ����
};
// ��̬�˿�NATӳ��
struct MapDynamicPortNat{
          unsigned long nIntraLocalAddr; //����ip��ַ
          unsigned long nOutsideGlobalAddr;//����ip��ַ
          unsigned short  nPort;//�˿�
};
//��̬�˿�NATӳ��
struct STATICTABLEITEM{
	BYTE     type;                  //=1:tcp;=2:udp;3:icmp 
    ULONG    ulIpadress;              
	USHORT   usPort;                  
};

struct STATICTABLE{
	int   count; 
    STATICTABLEITEM*  item;
};
typedef union _in_addr {
//	ULONG s_addr;
	struct {
		BYTE s_b[4];
	} s_un_b;
//	struct {
//		unsigned short s_w[2];
//	} s_un_w;
ULONG ss_addr;
#define s_b s_un_b.s_b
//#define s_w s_un_w.s_w
} in_addr_t;

//��������
struct  NAT_Cfg {
        ULONG   nGlobalAddress; //�ⲿ��ַ:��nOutsideIp
//      int     nDenyInternalAddressNumber; //��ֹNAT���ڲ���ַ����
//      ULONG*  ul_DenyGlobalAddress;      //�ڲ���ֹ��ַ��Χ
//      int     nInternalNetWorkNumber;     //�ڲ���ַ��������
//      ULONG*  ul_NetworkAddress;          //�ڲ������ַ
        int     nTimeOut_TCP;           //��̬�˿�ת����ʱ�����ֵ
        int     nTimeOut_UDP;           //��̬�˿�ת����ʱ�����ֵ
        USHORT  us_LowPort;             //��Ϳ��ö˿ں� 
        USHORT  us_HighPort;            //��߿��ö˿ں�
        USHORT  us_MaxNatNumber;        //���˿�ת������
        USHORT  us_PortNumber;          //��̬�˿�ӳ������
        struct  STATICTABLE  StaticTable;//��̬�˿�NATӳ���б�
};

struct  Filterdrv_Cfg {
int         n_Dev_number;          //��װ�����豸����
USHORT      us_Dev_Status[10][2];    //�豸״̬
            //��1 �� = �豸���
            //�� 2 �� = �豸����״̬
                     // 0        �����������ݰ�
                    // 1          ���������ݰ�
                    // 2          ֻ������վ���ݰ�
                    // 3          ֻ���س�վ���ݰ�
                    // 4          �����������ݰ�
};


//������������DM������
struct  FireWallConfig{
	      USHORT   OutNumber;
		  USHORT   InNumber;
		  USHORT   MdNumber;
          ULONG     nOutsideIp;//����ip
		  ULONG     nOutsideMask;//������ַ����
		  ULONG     nMiddleIp;//DM��IP
		  ULONG     nMiddleMask;
          ULONG     nIntraIp;
		  ULONG     nIntraMask;
//          struct  Filterdrv_Cfg   sFilterdrvCfg;
//          struct  NAT_Cfg    sNatCfg;         
};
struct  IpaddrRange{
         USHORT    nIpaddrNum;//��������1��ʼ
         ULONG     nIpMask;//IP����
         ULONG     nIpAddr;//��ʼIP��ַ������nIpaddrNum�������IP��ַ
};

//�˿ڷ�Χ
struct  PortRange{
         unsigned short  nPortBegin;
         unsigned short  nPortEnd;
};
// �˿��б�
struct  PortList {
         BOOL  bRange;//true: PortRange��Ч
         union{
               unsigned short  nPort;// �ܾ��ľ���˿�,
               struct   PortRange     sPortRange;// �ܾ��Ķ˿ڷ�Χ
		 };
};
//������ܾ��Ķ˿��б�
struct  PortSet{
         CPtrList  sAllowOutDstPortList;// �����ȥ��Ŀ�Ķ˿��б�
         CPtrList  sAllowOutSrcPortList;// �����ȥ��Դ�˿��б�
};

struct  IcmpRange{
         USHORT   nIcmpBegin;
         USHORT   nIcmpEnd;
};// �ܾ���ICMP���ͷ�Χ
// ICMP�б�
struct  IcmpList {
   //      IcmpList * pIcmpList;
         BOOL  bRange;//true: PortRange��Ч
         union{
               USHORT   nIcmpType;// �ܾ��ľ���ICMP����,
               struct  IcmpRange sIcmpRange;// �ܾ���ICMP���ͷ�Χ
		 };
};
//���ṹ������������������
struct  HostList{
 //       HostList *pHostList;
         struct   IpaddrRange   sIpRange;//��ʶ������Χ������Ϊһ̨�������̨��
         struct  PortSet  sTcpPortSet;
         struct  PortSet  sUdpPortSet;
		 CPtrList    sInIcmpSet;//���������ICMP��������
         CPtrList    sOutIcmpSet; //�����ȥ��ICMP��������
};


//ȫ�����ã����firewall����
struct GlobalFlags{
        BOOL bCheckInNetAttack;//�Ƿ���������ķ��ʣ�
        BOOL bCheckOutNetAccack;//�Ƿ���������ķ��ʣ�
        BOOL bCheckDmAccack;//�Ƿ����DM���ķ��ʣ�
        BOOL bMulticast;//�鲥�����־
        BOOL bNonIpPacket;//��Ip��
        BOOL bOtherIpPacket;//��tcp,udp,icmp�����ip��
        BOOL bSusPectOffset;// Ip���ڷ��ֿ��ɵ�ƫ��
        BOOL bFragmentIcmp;//�ֶε�ICMP��
        BOOL bSusPectIcmp;//���ɵ�ICMP��
		BOOL bforbidIcmp;
};


//�������ڽ�����ʾ���ļ���ȡ
//������������������
struct HostChar{
	    char HostIp[24];         //������ip��Χ
	    char TcpSrcOut[30][16];  //�����ȥ��tcpԴ�˿�
	    char TcpDstOut[30][16];  //�����ȥ��tcpĿ�Ķ˿�
	    char UdpSrcOut[30][16];  //�����ȥ��udpԴ�˿�
	    char UdpDstOut[30][16];  //�����ȥ��udpĿ�Ķ˿�
	    char IcmpTypeIn[30][5];  //���������icmp����
	    char IcmpTypeOut[30][5]; //�����ȥ��icmp����
		int  nPos[10];           
};


struct  Nat_CfgChar{
//        ULONG   nGlobalAddress; //�ⲿ��ַ
//		ULONG   nTotalIntraIp;//��ֹNAT�������ڲ���ַ����;����DenyIntraAddr��ֵ��ͬʱ�Ըó�Ա��ֵ
//        USHORT  nDenyIntraAddrlen;// struct  IpaddrRange����������������
//       char    DenyIntraAddr[100][24];//��ֹNAT�������ڲ���ַ
//        int     nInternalNetWorkNumber;     //�ڲ���ַ��������(������)
//        char    NetworkAddress[50][20];   //�ڲ������ַ ����δ�ӡ�����������������������                
        UINT     nTimeOut_TCP;           //��̬�˿�ת����ʱ�����ֵ
        UINT     nTimeOut_UDP;           //��̬�˿�ת����ʱ�����ֵ
        UINT  nLowPort;             //��Ϳ��ö˿ں� 
        UINT  nHighPort;            //��߿��ö˿ں�
        UINT  nMaxNatNumber;        //���˿�ת������
        USHORT   us_PortNumberTcp;          //tcp��̬�˿�ӳ������
		USHORT   us_PortNumberUdp;
		char  MapStaticTcp[50][24];//tcp��̬�˿�NATӳ��
		char  MapStaticUdp[50][24];//udp��̬�˿�NATӳ��
};
struct  IpFilter{
	BOOL isLogIp;			//1,Ҫ����IP���˲���־��
	DWORD Options;//bit0, inbroad;  //1��Ҫ��¼���鲥��
				//bit1,  outbroad;//1,Ҫ��¼���鲥
				//bit 2,incommingPort; //1��Ҫ��¼����˿ڣ�
				//bit 3,outgoingPort; //1��Ҫ��¼��ȥ�˿ڣ�
				//bit 4,inlength;//1,Ҫ��¼��Э��ͷ̫��
				//bit 5,outlength;//1,Ҫ��¼��Э��ͷ̫��
				//bit 6, incommingType; //1��Ҫ��¼����ICMP���ͣ�
				//bit 7, outgoingType; //1��Ҫ��¼��ȥICMP���ͣ�
				//bit 8, incommingIP; //1��Ҫ��¼����IP��
				//bit 9, outgoingIP;	//1��Ҫ��¼��ȥIP��
				//bit 10,11,=0;//��IP;
				//bit 12,inoffset;//��¼�����ƫ����
				//bit 13,outoffset;//��¼������ƫ����
				//bit 14,inmore;//��¼��ֶ�
				//bit 15,outmore;//��¼���ֶ�
				//bit 16, incommingDOS; //1��Ҫ��¼����DOS��
				//bit 17, outgoingDOS; //1��Ҫ��¼��ȥDOS��
};
struct ProCommentsLog{
	BOOL isLogProtocal;	//1,Ҫ����Э�����ݹ�����־��
	DWORD Options;	    //bit 0,��¼IP�Ͷ˿ڣ�
						//bit 1,��¼Э�����ͣ�
						//bit 2,��¼���URL��
						//bit 3,��¼ʱ��
};
struct DefaultSetData{
	DWORD Priority;			//����ǽ��ȫ�ȼ��߶ȣ�
							//0��High;1,Moderate;2,Low;
	DWORD CheckINorOUTNetCard;	//bit 0,IN,=1,check;
								//bit 1,OUT,=1,check;
								//���м���ʱ������飬�ͣ����������
	DWORD Pro;//BIT 0,ICMP,PERMIT OR DENY INtoOUT(1 OR 0);
			//BIT 1,ICMP,PERMIT OR DENY OUTtoIN(1,OR 0);
			// BIT 2,IGMP,PERMIT OR DENY INtoOUT(1 OR 0);
			//BIT 3,IGMP,PERMIT OR DENY OUTtoIN(1,OR 0);
			//ע�ͣ������ʱ��ȫ0����ʱ��ȫ1��
			//�����м䣬ICMP��IGMP 0��
	int TCPProtocal; //BIT 0,PERMIT OR DENY IN 80(1 OR 0);
					//BIT 1,PERMIT OR DENY OUT 80(1,OR 0);
					//BIT 2,PERMIT OR DENY IN FTP(1 OR 0);
					//BIT 3,PERMIT OR DENY OUT FTP(1,OR 0);
					//BIT 4,PERMIT OR DENY IN NBT(1 OR 0);
					//BIT 5,PERMIT OR DENY OUT NBT(1,OR 0);
					//BIT 6,PERMIT OR DENY IN TELNET(1 OR 0);
					//BIT 7,PERMIT OR DENY OUT TELNET(1,OR 0);
					//BIT 8,SNMP,PERMIT OR DENY IN(1 OR 0);
					//BIT 9,SNMP,PERMIT OR DENY OUT(1,OR 0);
					//BIT 10, IN and OUT FINGER��79��PERMIT OR DENY OUT(1,OR 0);
					//BIT 11,�ڲ���ȥ�˿�<=1024,�������϶˿��⣺
					//1�� PERMIT��0��DENY��
					//BIT 12���ڲ���ȥ�˿�>1024:1��PERMIT��0��DENY��
					//bit 13,�ⲿ�����˿�>1024��1��PERMIET��0��DENY��
					//BIT 14���ⲿ�����˿�<=1024���������϶˿��⣺1��PERMIET��0��DENY��
										//bit 15-31,����
										//�����ʱ��BIT 0��1��12λ��=1������=0��
					//������ʱ��BIT 10��13=0������=1��
					//�����ʱ��BIT 10Ϊ1��

	int PortocalComments;
					//BIT 0,PERMIT OR DENY IN JAVA(1,OR 0);
					//BIT 1,PERMIT OR DENY IN SCRIPT(1,OR 0);
					//BIT 2,PERMIT OR DENY IN ACTIVEX (1,OR 0);
					//BIT 3-31,������Priority=�ߣ�ȫ0���ͣ�1��
					//ע�ͣ��������ʱ��ȫ0����ʱ��ȫ1��
	DWORD UDPProtocal;//BIT 0��OuterNetwork DNS,1,permit,0,deny;
						//������Priority=�ߣ�ȫ0���ͣ�ȫ1��
};


struct  LogManage{
	BOOL RequireLog;	//1����Ҫ��־��¼����0������Ҫ
	IpFilter TheIpFilter;
	ProCommentsLog TheProCommentsLog;
};
//������������DM������
struct  FireWallConfigChar{
          char   csOutsideIp[16];//����ip
		  char   csOutsideMask[16];//������ַ����
		  char   csMiddleIp[16];//DM��IP
		  char   csMiddleMask[16];
          char   csIntraIp[16];
		  char   csIntraMask[16];
 //         struct  Filterdrv_Cfg   sFilterdrvCfg;
          struct  Nat_CfgChar    sNatCfg;         
};
//�ýṹ���ļ���ȡ��������ʾ����ʼ�����õ���
struct AllLineChar{
        struct GlobalFlags  sGlobalFlags;
	    ULONG  CurHostNum;//Hostchar�ĵ�ǰ�±�
	    struct HostChar Hostchar[100];
		int    AllowPos;
	    char   AllowIpTab[100][24];
		int    RejPos;
		char   RejIpTab[100][24];
		struct FireWallConfigChar FireWallCfg;
		int     InDmPos;
        char    InDmIpTab[100][24];
		int     OutDmPos;
        char    OutDmIpTab[100][24];
};

//��̬�˿�NATӳ��
struct MapStaticPortNatChar{
          char  ServicePort[16];//��׼����˿�
          char  IntraIpaddr[24];//����ip��ַ
};

typedef struct LogView{
	char time[30];
	char prot[10];
	char src[24];
	char dst[24];
	char demo[20];
}LOGVIEW;
struct LogView1{
	char time[30];
	char keyword[20];
	char URL[250];
	char src[24];
	char dst[24];
	char demo[20];
};
typedef struct LogViewIcmp{
	char time[30];
	char prot[10];
	char src[24];
	char dst[24];
	char demo[20];
	char type[3];
}LOGVIEWICMP;
 
#define M_LOGVIEW WM_USER+1111
enum{TCPSRCOUT=0,TCPDSTOUT,UDPSRCOUT,UDPDSTOUT,ICMPTYPEIN,ICMPTYPEOUT};
#endif