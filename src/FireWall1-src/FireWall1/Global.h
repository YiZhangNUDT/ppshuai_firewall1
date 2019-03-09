
#if !defined(FIREWALLGLOBAL)
#define FIREWALLGLOBAL

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "stdafx.h"
#include "Struct.h"
#define  ALLOWIPTAB  22
#define  REJIPTAB  23
#define  HOSTIP   24
#define  INITALLOWIPTAB 25
#define  INITREJIPTAB  26
#define   MAPSTATIC_UDP 29
#define   MAPSTATIC_TCP  30
#define  INITINDMTAB 31
#define  INITOUTDMTAB  32
#define   INDM  37
#define   OUTDM 38
#define   INSMTP   41
#define   OUTSMTP  42

class CDataConvert
{ 
public:
	CPtrList m_ptrlistIpRange;//��Ϊ��ɾ����Ӧ���͵Ŀռ�
	CPtrList m_ptrlistUlong;//��Ϊ��ɾ����Ӧ���͵Ŀռ�
	CPtrList m_ptrlistIcmp,m_ptrlistIcmpOut;//��Ϊ��ɾ����Ӧ���͵Ŀռ�
	CDataConvert();
	~CDataConvert();
	struct AllLineChar   m_alllinechar;
	ULONG m_CurIp;
    char m_cCurIp[24];
    struct GlobalFlags  globalFlags;
    CPtrList  m_ptrlistRejectIp,m_ptrlistdel;
    CPtrList  m_ptrlistAllowIp,m_listInHttp,m_listOutHttp,m_listInFtp;//items:
	CPtrList  m_listOutFtp,m_listInSmtp,m_listOutSmtp;
    struct  FireWallConfig m_firewallconfig;
    CPtrList m_ptrlistHostPort;//items:struct  HostList
    CPtrList m_ptrlistPort;////ע�⣺m_ptrlistPort,m_ptrlistdelֻ��Ϊ��ɾ���ռ�
	VOID  AddIpTable(UCHAR type,CEdit &edit);//���ip��ַ��������
	//���������ip��ַ��Χ��ӵ��������б�
    VOID  SetHostIp(CString m_HostIp,BOOL &bhaved,struct HostChar*phostchar);
	//��������Ķ˿�������ӵ��������б���
    VOID  SetPortList(CEdit &edit,USHORT type);
	// ����NATģ��������������
	VOID  SetNatCfg(struct  NAT_Cfg *natcfg);
	//���ַ���ʽ�����û����ã��Ա�һ��д���ļ�
    VOID  SaveChar(CEdit &edit,BOOL ishostCfg,UCHAR type);
	//д���ļ�
    VOID   WriteToFile();
	//���ļ�����
	VOID  ReadFromFile();
	//���struct  FireWallConfig�ṹ
    VOID  InitFireWall();//m_firewallconfig
	//���༭���е�����ת��Ϊ������������
    VOID  ConvertEditChar(char *pchar,ULONG &mylong,USHORT &mynum,BOOL bIp);
	//��ʾ�ض�������������
    VOID  DisplayHost(struct HostChar *phostchar,USHORT type,CEdit &edit);
	//���ļ���װ���û��ϴ�������˿�����
    VOID  InitPortList(USHORT type,int &ii,struct HostList *pHostList);
	//���ļ���װ���û��ϴ����������ã����е�����InitPortList��������
    VOID  InitHostList();
    //��ʾ�������ģ������
    VOID  DisplayFireCfg(CEdit &edit,USHORT type);
    //ɾ���ض���������
    VOID  DeleteHost(CString m_HostIp);

};
#endif 