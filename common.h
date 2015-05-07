#ifndef COMMON_H
#define COMMON_H

/****************************************/
/* P-Agent�����M�����f�[�^Check       */
/****************************************/
/*	BUILD_*�n	*/
#define BUILD_BUILDNAMEKANA_LEN	13
#define BUILD_BUILDCODE_LEN		10
#define BUILD_BUILDNAME_LEN		12
#define BUILD_BRANCHCODE_LEN	8
#define BUILD_ACOMODATEAREA_LEN	12
#define BUILD_BUILDTYPE_LEN		20
#define BUILD_BUILDADDRESS_LEN	70
#define BUILD_FLOOR_LEN			3
#define BUILD_MECONTACTID_LEN	40
#define BUILD_PHONENUMBER_LEN	12
#define BUILD_ADMINAREA_LEN		70
#define BUILD_PUBLICBUILD_LEN	40

/*	BRANCH_*�n	*/
#define BRANCH_BRANCHCODE_LEN	8
#define BRANCH_KENNAME_LEN		10
#define BRANCH_BRANCHNAME_LEN	10

/*	ISP_CONTACT__*�n	*/
#define ISP_CONTACT_ISPCODE_LEN				3
#define ISP_CONTACT_ISPCORPNAME_LEN			100
#define ISP_CONTACT_ISPSERVICENAME_LEN		100
#define ISP_CONTACT_MAINTENAME_LEN			100
#define ISP_CONTACT_MAINTENUMBER_LEN		12
#define ISP_CONTACT_MAINTEREASON_LEN		40
#define ISP_CONTACT_MAINTEMAILTO_LEN		50
#define ISP_CONTACT_MAINTETIME_LEN			12
#define ISP_CONTACT_CONSTNAME_LEN			100
#define ISP_CONTACT_CONSTNUMBER_LEN			12
#define ISP_CONTACT_CONSTREASON_LEN			40
#define ISP_CONTACT_CONSTMAILTO_LEN			50
#define ISP_CONTACT_CONSTTIME_LEN			12
#define ISP_CONTACT_ISPMEMO_LEN				100
#if(0)	/* �O�΁[�����̖��c��	���͎g��Ȃ�	*/
#endif

/*	ISP_BRANCH_*�n	*/
#define ISP_BRANCH_ISPCORPPNAME_LEN			100	
#define ISP_BRANCH_ISPSERVICENAME_LEN		100
#define ISP_BRANCH_BRANCHCODE_LEN			8
#define ISP_BRANCH_ISPCODE_LEN				3
#define ISP_BRANCH_ACENUMBER_LEN			2
#define ISP_BRANCH_ISPIFADDR_LEN			15
#define ISP_BRANCH_ISPROUTERIP_LEN			15
#define ISP_BRANCH_NTEIFADDR_LEN			15
#define ISP_BRANCH_ISPNETADDRESS_LEN		15
#define ISP_BRANCH_ISPSUBNETMASK_LEN		15
#define ISP_BRANCH_CIRCUITTYPE_LEN			20
#define ISP_BRANCH_CIRCUITSPEED_LEN			5

/*	ISP_NTE_*�n	*/
#define ISP_NTE_BRANCHCODE_LEN				8
#define ISP_NTE_ISPCODE_LEN					3
#define ISP_NTE_NTENUMBER_LEN				3
#define ISP_NTE_ISPNAME_LEN					100
#define ISP_NTE_ISPSERVICENAME_LEN			100
#define ISP_NTE_CIRCUITTYPE_LEN				20
#define ISP_NTE_CIRCUITSPEED_LEN			5
#define ISP_NTE_USERNAMETYPE_LEN			90
#define ISP_NTE_SENDUSERIDTYPE				90
#define ISP_NTE_ISPDESC1_LEN				30
#define ISP_NTE_ISPDESC2_LEN				30
#define ISP_NTE_PROTCOLPORTNO1_LEN			4
#define ISP_NTE_PROTCOLPORTNO2_LEN			4
#define ISP_NTE_CERTIFYTYPE_LEN				8
#define ISP_NTE_IPADDRTYPE_LEN				8
#define ISP_NTE_ISPIFADDR_LEN				15
#define ISP_NTE_ISPROUTERIP_LEN				15
#define ISP_NTE_NETWORKADDR1_LEN			15
#define ISP_NTE_SUBNETMASK1_LEN				15
#define ISP_NTE_ISPSERVERADDR1_LEN			15
#define ISP_NTE_ISPSERVERADDR2_LEN			15
#define ISP_NTE_TIMEOUT_LEN					2
#define ISP_NTE_RETRYCNT_LEN				2
#define ISP_NTE_PASSAGETIME_LEN				3
#define ISP_NTE_POOLADDR1_LEN				19
#define ISP_NTE_POOLADDR2_LEN				19
#define ISP_NTE_POOLADDR3_LEN				19
#define ISP_NTE_MAXSESSION_LEN				4
#define ISP_NTE_DNSINFO_LEN					31
#if(0)
#define ISP_NTE_DNSINFO2_LEN				31
#endif
#define ISP_NTE_REDIUSKEYS_LEN				15
#define ISP_NTE_VPI_VCI_LEN					9
#define ISP_NTE_OAMPVC_LEN					6
#define ISP_NTE_ADDATTRIBUTE_LEN			4
#define ISP_NTE_CONSTDATE_LEN				10
#define ISP_NTE_SERVICESTARTDATE_LEN		10

/*	MEDO_CONTACT_*�n	*/
#define MEDO_CONTACT_MECONTACTID_LEN		40
#define MEDO_CONTACT_MEMAINTENANCEAREA_LEN	12
#define MEDO_CONTACT_MECENTERNAME_LEN		10
#define MEDO_CONTACT_MEBASENAME_LEN			12
#define MEDO_CONTACT_MEDIVISION_LEN			40
#define MEDO_CONTACT_MEPERSONDAY_LEN		20
#define MEDO_CONTACT_MECONTACTDAY_LEN		12
#define MEDO_CONTACT_NIGHTSERVICEFLAG_LEN	2
#define MEDO_CONTACT_MECONTACTNIGHT_LEN		12
#define MEDO_CONTACT_FAXNUMBERDAY_LEN		12
#define MEDO_CONTACT_PORTABLETELEPHONE_LEN	13
#if(0)	/* �O�΁[�����̖��c��	���͎g��Ȃ�	*/
#endif

/*	MAKER_CONTACT_*�n	*/
#define MAKER_CONTACT_MAKERCONTACTID_LEN	50
#define MAKER_CONTACT_MAKERNAME_LEN			30
#define MAKER_CONTACT_MAKERARRANGE_LEN		40
#define MAKER_CONTACT_MAKERCONTACTDAY_LEN	12
#define MAKER_CONTACT_MAKERPERSONDAY_LEN	20
#define MAKER_CONTACT_FAXNUMBERDAY_LEN		12
#define MAKER_CONTACT_MAKERCONTACTNIGHT_LEN	12
#define MAKER_CONTACT_MAKERPERSONNIGHT_LEN	20
#define MAKER_CONTACT_FAXNUMBERNIGHT_LEN	12
#if(0)  /* �O�΁[�����̖��c�� ���͎g��Ȃ�    */
#endif

/*	EQP_*�n	*/
#define EQP_BRANCHCODE_LEN		8
#define EQP_HOSTNAME_LEN		11
#define EQP_PRODUCTNAME_LEN		30
#define EQP_MAKERCONTACTID_LEN	50
#define EQP_COMPONENTID_LEN		24

/*	COMPONENT_*�n	*/
#define COMPONENT_COMPONENTID_LEN			24
#define COMPONENT_COMPONENTNO_LEN			3
#define COMPONENT_COMPONENTKATANO_LEN		100
#define COMPONENT_COMPONENTKANANAME_LEN		100
#define COMPONENT_COMPONENTNUM_LEN			2

/********* �ۗ�	*****************************/
/*	ISP_*�n	*/
/*
#define ISP_ISPCORPNAME_LEN		100
#define ISP_ISPSERVICENAME_LEN	100
#define ISP_AREANAME_LEN		10
#define ISP_ISPCODE_LEN			11
#define ISP_ACENUMBER_LEN		2
#define ISP_ISPIFADDR_LEN		15
#define ISP_ISPREOUTERIP_LEN	15
#define ISP_NTEIFADDR_LEN		15
#define ISP_ISPNETADDRESS_LEN	15
#define ISP_ISPSUBNETMASK_LEN	15
#define ISP_CIRCUITTYPE_LEN		20
#define ISP_CIRCUITSPEED_LEN	5
#define ISP_CIRCUITID_LEN		10
*/

/*	ISPSetting_*�n	*/
/*
#define ISPSETTING_ISPID_LEN				14
#define ISPSETTING_BRANCHNAME_LEN			10
#define ISPSETTING_NTENUMBER_LEN			3
#define ISPSETTING_ISPNAME_LEN				100
#define ISPSETTING_ISPSERVICENAME_LEN		100
#define ISPSETTING_CIRCUITTYPE_LEN			20
#define ISPSETTING_CIRCUITSPEED_LEN			5
#define ISPSETTING_CIRCUITID_LEN			10
#define ISPSETTING_USERNAMETYPE_LEN			60
#define ISPSETTING_SENDUSERIDTYPE_LEN		60
#define ISPSETTING_ISPDESC_LEN				30
#define ISPSETTING_PROTCOLPORTNO1_LEN		4
#define ISPSETTING_PROTCOLPORTNO2_LEN		4
#define ISPSETTING_CERTIFYTYPE_LEN			8
#define ISPSETTING_IPADDRTYPE_LEN			8
#define ISPSETTING_ISPIFADDR_LEN			15
#define ISPSETTING_ISPROUTERIP_LEN			15
#define ISPSETTING_NETWORKADDR1_LEN			15
#define ISPSETTING_SUBNETMASK1_LEN			15
#define ISPSETTING_ISPSERVERADDR_LEN		15
#define ISPSETTING_TIMEOUT_LEN				2
#define ISPSETTING_RETRYCNT_LEN				2
#define ISPSETTING_PASSAGETIME_LEN			3
#define ISPSETTING_POOLADDR_LEN				19
#define ISPSETTING_MAXSESSION_LEN			4
#define ISPSETTING_DNSINFO_LEN				31
#define ISPSETTING_REDIUSKEYS_LEN			15
#define ISPSETTING_VPI_VCI_LEN				9
#define ISPSETTING_OAMPVC_LEN				6
#define ISPSETTING_ADDRATTRIBUTE_LEN		4
#define ISPSETTING_CONSTDATE_LEN			10
#define ISPSETTING_SERVICESTARTDATE_LEN		10
*/


/****************************************************************************/
/* �ő�l                                   */
/****************************************************************************/
#define GWP_MXPAGDTLEN      4096            /* pagent   TCP �f�[�^�ő咷 */
#define GWP_MXDTLEN     4096            /* �S��     TCP �f�[�^�ő咷 */
/*  �L�����N�^�R�[�h    */
#define GWP_CHR_EOL     012         /* EOL �R�[�h   */

/****************************************************************************/
/* Gateway �v���Z�X������                         */
/****************************************************************************/
/* DB P-Agent DB G/W ������e�[�u��                                       */
typedef struct{
	char        dcmenvpath[GW_MXFILELEN+1];		/* Docomo���t�@�C���p�X�� */
	char        pagent_host[TCPIF_MXHOSTLEN+1]; /* P-Agent �z�X�g��         */
	int 	    pagent_port;					/* P-Agent �|�[�g�ԍ�       */
	char        owneqpname[GW_MXNAMELEN+1];		/* �� ���ʖ�                */
	char        usr_name[GW_MXNAMELEN+1];		/* DB ���[�U��              */
	char        passwd[GW_MXNAMELEN+1];			/* �p�X���[�h               */
	char        sid[GW_MXNAMELEN+1];			/* SID                      */
	char        pipepath[GW_MXFILELEN+1];		/* ���O�o�͗p pipe�p�X��    */
	TCPIF_INF   tcpinf_pagent;                  /* TCP/IP ������(Pagent)  */
} GWP_INF;

/********** �ǉ� �������� V01 **********/
#define DBP_ENVPATH     "DBP_ENV_PATH"  /* �ݒ�t�@�C��dbp.conf �̃p�X���ϐ� */
#define DBP_CONF        "dbp.conf"      /* �ݒ�t�@�C����                      */
#define OSI_HOME        "OSI_HOME"		/* OSI HOME ���ϐ���                 */
/********** �ǉ� �����܂� V01 **********/

#endif


