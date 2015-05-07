/****************************************************************************/
/*																			*/
/*		PROC用ヘッダーファイル												*/
/*		pcファイルに使用する												*/
/*																			*/
/*		作成日時	2001/1/10												*/
/*		変更日時	2001/2/2												*/
/*		作成者		長島													*/
/*																			*/
/****************************************************************************/



EXEC SQL BEGIN DECLARE SECTION;		/*		SQL SECTION		*/

/****************************************************/
/*	共通項目	レコード作成者	swCreateBy			*/
/****************************************************/
#if(0)
#define DB_USER "nxvandb"
#define DB_PASSWD "nxvandb"
#define DB_USER "vandb"
#define DB_PASSWD "vandb"
#endif
#define CREATE_BY "wip"

/****************************************************/
/*													*/
/*	使用関数名	Proc_BUILD_Insert/Update			*/
/*													*/
/****************************************************/
typedef struct {
	char BuildNameKana[14];
	char BuildCode[11];
	char BuildName[13];
	char BranchCode[9];
	char AcomodateArea[13];
	char BuildType[21];
	char BuildAddress[71];
	char Floor[4];
	char MeContactid[41];
	char PhoneNumber[13];
	char AdminArea[71];
	char PublicBuild[41];
}PROC_BUILD;

/****************************************************/
/*													*/
/*	使用関数名	Proc_BRANCH_Insert/Update			*/
/*													*/
/****************************************************/
typedef struct {
	char BranchCode[9];
	char KenName[11];
	char BranchName[11];
}PROC_BRANCH;

/********************************************************************/
/*																	*/
/*	使用関数名	Proc_ISP_CONTACT_Insert/Update	前ばーじょん		*/
/*																	*/
/********************************************************************/
#if(0)
typedef struct {
	char ISPCode[4];
	char ISPCorpName[101];
	char ISPServiceName[101];
}PROC_ISP_CONTACT;
#endif

/****************************************************/
/*													*/
/*	使用関数名	Proc_ISP_CONTACT_Insert/Update		*/
/*													*/
/****************************************************/
typedef struct {
	char ISPCode[4];
	char ISPCorpName[101];
	char ISPServiceName[101];
	char MainteName[101];
	char MainteNumber[13];
	char MainteReason[41];
	char MainteMailTo[51];
	char MainteTime[13];
	char ConstName[101];
	char ConstNumber[13];
	char ConstReason[41];
	char ConstMailTo[51];
	char ConstTime[13];
	char IspMemo[101];
}PROC_ISP_CONTACT;

/****************************************************/
/*													*/
/*	使用関数名	Proc_ISP_BRANCH_Insert/Update		*/
/*													*/
/****************************************************/
typedef struct {
	char ISPCorpName[101];
	char ISPServiceName[101];
	char BranchCode[9];
	char ISPCode[4];
	char ACENumber[3];
	char ISPIFAddr[16];
	char ISPRouterIP[16];
	char NTEIFAddr[16];
	char ISPNetAddress[16];
	char ISPSubnetmask[16];
	char CircuitType[21];
	char CircuitSpeed[6];
}PROC_ISP_BRANCH;

/****************************************************/
/*													*/
/*	使用関数名	Proc_ISP_NTE_Insert/Update			*/
/*													*/
/****************************************************/
typedef struct {
	char BranchCode[9];
	char ISPCode[4];
	char NTENumber[4];
	char ISPName[101];
	char ISPServiceName[101];
	char CircuitType[21];
	char CircuitSpeed[6];
	char UserNameType[91];
	char SendUserIDType[91];
	char ISPDesc1[31];
	char ISPDesc2[31];
	char ProtcolPortNo1[5];
	char ProtcolPortNo2[5];
	char CertifyType[9];
	char IPAddrType[9];
	char ISPIFAddr[16];
	char ISPRouterIP[16];
	char NetworkAddr1[16];
	char Subnetmask1[16];
	char ISPServerAddr1[16];
	char ISPServerAddr2[16];
	char Timeout[3];
	char RetryCnt[3];
	char PassageTime[4];
	char PoolAddr1[20];
	char PoolAddr2[20];
	char PoolAddr3[20];
	char MaxSession[5];
	char DNSInfo[32];
#if(0)
	char DNSInfo2[32];
#endif
	char RediusKeys[16];
	char VPI_VCI[10];
	char OamPVC[7];
	char AddAttribute[5];
	char ConstDate[11];
	char ServiceStartDate[11];
}PROC_ISP_NTE;

/********************************************************************/
/*																	*/
/*	使用関数名	Proc_MEDO_CONTACT_Insert/Update	前ばーじょん		*/
/*																	*/
/********************************************************************/
#if(0)
typedef struct {
	char MeContactid[35];
	char MeMaintenanceArea[13];
	char MeCenterName[11];
	char MeBaseName[13];
}PROC_MEDO_CONTACT;
#endif

/****************************************************/
/*													*/
/*	使用関数名	Proc_MEDO_CONTACT_Insert/Update		*/
/*													*/
/****************************************************/
typedef struct {
	char MeContactid[41];
	char MeMaintenanceArea[13];
	char MeCenterName[11];
	char MeBaseName[13];
	char MeDivision[41];
	char MePersonDay[21];
	char MeContactDay[13];
	char NightServiceFlag[3];
	char MeContactNight[13];
	char FaxNumberDay[13];
	char PortableTelephone[14];
}PROC_MEDO_CONTACT;

/********************************************************************/
/*																	*/
/*	使用関数名	Proc_MAKER_CONTACT_Insert/Update	前ばーじょん	*/
/*																	*/
/********************************************************************/
#if(0)
typedef struct {
	char MakerContactid[40];
	char MakerName[31];
}PROC_MAKER_CONTACT;
#endif

/****************************************************/
/*													*/
/*	使用関数名	Proc_MAKER_CONTACT_Insert/Update	*/
/*													*/
/****************************************************/
typedef struct {
	char MakerContactid[51];
	char MakerName[31];
	char MakerArrange[41];
	char MakerContactDay[13];
	char MakerPersonDay[21];
	char FaxNumberDay[13];
	char MakerContactNight[13];
	char MakerPersonNight[21];
	char FaxNumberNight[13];
}PROC_MAKER_CONTACT;

/****************************************************/
/*													*/
/*	使用関数名	Proc_EQP_Insert/Update				*/
/*													*/
/****************************************************/
typedef struct {
	char BranchCode[9];
	char HostName[12];
	char ProductName[31];
	char MakerContactid[51];
	char Componentid[25];
}PROC_EQP;

/****************************************************/
/*													*/
/*	使用関数名	Proc_COMPONENT_Insert/Update		*/
/*													*/
/****************************************************/
typedef struct {
	char Componentid[25];
	char ComponentNo[4];
	char ComponentKataNo[101];
	char ComponentName[101];
	char ComponentNum[3];
}PROC_COMPONENT;

/************	保留	******************************/
/****************************************************/
/*													*/
/*	使用関数名	Proc_ISP_Insert/Update			*/
/*													*/
/****************************************************/
/*
typedef struct {
	char ISPCorpName[101];
	char ISPServiceName[101];
	char AreaName[11];
	char ISPCode[12];
	char ACENumber[3];
	char ISPIFAddr[16];
	char ISPRouterIP[16];
	char NTEIFAddr[16];
	char ISPNetAddress[16];
	char ISPSubnetmask[16];
	char CircuitType[21];
	char CircuitSpeed[6];
	char CircuitID[11];
}PROC_ISP;
*/
/****************************************************/
/*													*/
/*	使用関数名	Proc_ISPSetting_Insert/Update		*/
/*													*/
/****************************************************/
/*
typedef struct {
	char ISPID[15];
	char BranchName[11];
	char NTENumber[4];
	char ISPName[101];
	char ISPServiceName[101];
	char CircuitType[21];
	char CircuitSpeed[6];
	char CircuitID[11];
	char UserNameType[61];
	char SendUserIDType[61];
	char ISPDesc[31];
	char ProtcolPortNo1[5];
	char ProtcolPortNo2[5];
	char CertifyType[9];
	char IPAddrType[9];
	char ISPIFAddr[16];
	char ISPRouterIP[16];
	char NetworkAddr1[16];
	char Subnetmask1[16];
	char ISPServerAddr[16];
	char Timeout[3];
	char RetryCnt[3];
	char PassageTime[4];
	char PoolAddr[20];
	char MaxSession[5];
	char DNSInfo[32];
	char RediusKeys[16];
	char VPI_VCI[10];
	char OamPVC[7];
	char AddAttribute[5];
	char ConstDate[11];
	char ServiceStartDate[11];
}PROC_ISPSETTING;
*/


EXEC SQL END DECLARE SECTION;

/*	END OF FILE	*/

