/****************************************************************************/
/* file nanme  isp_nte_update.c                                             */
/*                                                                          */
/* Vantive用データベースGateWayプロセス                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include "tcp_if.h"
#include "gw_if.h"
#include "dbpag_if.h"
#include "db_func.h"
#include "pagent_trace.h"
#include "proc_str.h"
#include "common.h"

#define  MAX_DATA_LEN   GWP_MXPAGDTLEN + 1

extern int      Proc_ISP_NTE_Update( ISP_NTE* );


/****************************************************************************/
/* Name : isp_nte_update                                                    */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int isp_nte_update(char *src_data) {

	char	tmp_data[35][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    ISP_NTE	isp_nte;


	memset(&tmp_data[0][0], NULL, 35*MAX_DATA_LEN);
	memset(&isp_nte, NULL, sizeof(isp_nte));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 34 && (data - p_data) < MAX_DATA_LEN)
		{
			strncpy(tmp_data[i], p_data, data - p_data);
			p_data = data + 1;
			i ++;
		}
		else
		{
			i = 99;
			break;
		}
	}

	if( i == 34 )
	{
		if(strlen(p_data) < MAX_DATA_LEN)
		{
			strcpy(tmp_data[i], p_data);
		}
		else
		{
			return 2;
		}
	}else{
		return 2;
	}

	if(i == 34) 
	{
		strncpy(isp_nte.BranchCode, &tmp_data[0][0], ISP_NTE_BRANCHCODE_LEN);
        strncpy(isp_nte.ISPCode, &tmp_data[1][0], ISP_NTE_ISPCODE_LEN);
        strncpy(isp_nte.NTENumber, &tmp_data[2][0], ISP_NTE_NTENUMBER_LEN);
        strncpy(isp_nte.ISPName, &tmp_data[3][0], ISP_NTE_ISPNAME_LEN);
        strncpy(isp_nte.ISPServiceName, &tmp_data[4][0], ISP_NTE_ISPSERVICENAME_LEN);
        strncpy(isp_nte.CircuitType, &tmp_data[5][0], ISP_NTE_CIRCUITTYPE_LEN);
        strncpy(isp_nte.CircuitSpeed, &tmp_data[6][0], ISP_NTE_CIRCUITSPEED_LEN);
        strncpy(isp_nte.UserNameType, &tmp_data[7][0], ISP_NTE_USERNAMETYPE_LEN);
        strncpy(isp_nte.SendUserIDType, &tmp_data[8][0], ISP_NTE_SENDUSERIDTYPE);
        strncpy(isp_nte.ISPDesc1, &tmp_data[9][0], ISP_NTE_ISPDESC1_LEN);
        strncpy(isp_nte.ISPDesc2, &tmp_data[10][0], ISP_NTE_ISPDESC2_LEN);
        strncpy(isp_nte.ProtcolPortNo1, &tmp_data[11][0], ISP_NTE_PROTCOLPORTNO1_LEN);
        strncpy(isp_nte.ProtcolPortNo2, &tmp_data[12][0], ISP_NTE_PROTCOLPORTNO2_LEN);
        strncpy(isp_nte.CertifyType, &tmp_data[13][0], ISP_NTE_CERTIFYTYPE_LEN);
        strncpy(isp_nte.IPAddrType, &tmp_data[14][0], ISP_NTE_IPADDRTYPE_LEN);
        strncpy(isp_nte.ISPIFAddr, &tmp_data[15][0], ISP_NTE_ISPIFADDR_LEN);
        strncpy(isp_nte.ISPRouterIP, &tmp_data[16][0], ISP_NTE_ISPROUTERIP_LEN);
        strncpy(isp_nte.NetworkAddr1, &tmp_data[17][0], ISP_NTE_NETWORKADDR1_LEN);
        strncpy(isp_nte.Subnetmask1, &tmp_data[18][0], ISP_NTE_SUBNETMASK1_LEN);
        strncpy(isp_nte.ISPServerAddr1, &tmp_data[19][0], ISP_NTE_ISPSERVERADDR1_LEN);
        strncpy(isp_nte.ISPServerAddr2, &tmp_data[20][0], ISP_NTE_ISPSERVERADDR2_LEN);
        strncpy(isp_nte.Timeout, &tmp_data[21][0], ISP_NTE_TIMEOUT_LEN);
        strncpy(isp_nte.RetryCnt, &tmp_data[22][0], ISP_NTE_RETRYCNT_LEN);
        strncpy(isp_nte.PassageTime, &tmp_data[23][0], ISP_NTE_PASSAGETIME_LEN);
        strncpy(isp_nte.PoolAddr1, &tmp_data[24][0], ISP_NTE_POOLADDR1_LEN);
        strncpy(isp_nte.PoolAddr2, &tmp_data[25][0], ISP_NTE_POOLADDR2_LEN);
        strncpy(isp_nte.PoolAddr3, &tmp_data[26][0], ISP_NTE_POOLADDR3_LEN);
        strncpy(isp_nte.MaxSession, &tmp_data[27][0], ISP_NTE_MAXSESSION_LEN);
        strncpy(isp_nte.DNSInfo, &tmp_data[28][0], ISP_NTE_DNSINFO_LEN);
#if(0)
        strncpy(isp_nte.DNSInfo2, &tmp_data[29][0], ISP_NTE_DNSINFO2_LEN);
#endif
        strncpy(isp_nte.RediusKeys, &tmp_data[29][0], ISP_NTE_REDIUSKEYS_LEN);
        strncpy(isp_nte.VPI_VCI, &tmp_data[30][0], ISP_NTE_VPI_VCI_LEN);
        strncpy(isp_nte.OamPVC, &tmp_data[31][0], ISP_NTE_OAMPVC_LEN);
        strncpy(isp_nte.AddAttribute, &tmp_data[32][0], ISP_NTE_ADDATTRIBUTE_LEN);
        strncpy(isp_nte.ConstDate, &tmp_data[33][0], ISP_NTE_CONSTDATE_LEN);
        strncpy(isp_nte.ServiceStartDate, &tmp_data[34][0], ISP_NTE_SERVICESTARTDATE_LEN);

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
printf("tmp_data[2]=%s\n",tmp_data[2]);
printf("tmp_data[3]=%s\n",tmp_data[3]);
printf("tmp_data[4]=%s\n",tmp_data[4]);
printf("tmp_data[5]=%s\n",tmp_data[5]);
printf("tmp_data[6]=%s\n",tmp_data[6]);
printf("tmp_data[7]=%s\n",tmp_data[7]);
printf("tmp_data[8]=%s\n",tmp_data[8]);
printf("tmp_data[9]=%s\n",tmp_data[9]);
printf("tmp_data[10]=%s\n",tmp_data[10]);
printf("tmp_data[11]=%s\n",tmp_data[11]);
printf("tmp_data[12]=%s\n",tmp_data[12]);
printf("tmp_data[13]=%s\n",tmp_data[13]);
printf("tmp_data[14]=%s\n",tmp_data[14]);
printf("tmp_data[15]=%s\n",tmp_data[15]);
printf("tmp_data[16]=%s\n",tmp_data[16]);
printf("tmp_data[17]=%s\n",tmp_data[17]);
printf("tmp_data[18]=%s\n",tmp_data[18]);
printf("tmp_data[19]=%s\n",tmp_data[19]);
printf("tmp_data[20]=%s\n",tmp_data[20]);
printf("tmp_data[21]=%s\n",tmp_data[21]);
printf("tmp_data[22]=%s\n",tmp_data[22]);
printf("tmp_data[23]=%s\n",tmp_data[23]);
printf("tmp_data[24]=%s\n",tmp_data[24]);
printf("tmp_data[25]=%s\n",tmp_data[25]);
printf("tmp_data[26]=%s\n",tmp_data[26]);
printf("tmp_data[27]=%s\n",tmp_data[27]);
printf("tmp_data[28]=%s\n",tmp_data[28]);
printf("tmp_data[29]=%s\n",tmp_data[29]);
printf("tmp_data[30]=%s\n",tmp_data[30]);
printf("tmp_data[31]=%s\n",tmp_data[31]);
printf("tmp_data[32]=%s\n",tmp_data[32]);
printf("tmp_data[33]=%s\n",tmp_data[33]);
printf("tmp_data[34]=%s\n",tmp_data[34]);
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("isp_nte.BranchCode=%s\n",isp_nte.BranchCode);
printf("isp_nte.ISPCode=%s\n",isp_nte.ISPCode);
printf("isp_nte.NTENumber=%s\n",isp_nte.NTENumber);
printf("isp_nte.ISPName=%s\n",isp_nte.ISPName);
printf("isp_nte.ISPServiceName=%s\n",isp_nte.ISPServiceName);
printf("isp_nte.CircuitType=%s\n",isp_nte.CircuitType);
printf("isp_nte.CircuitSpeed=%s\n",isp_nte.CircuitSpeed);
printf("isp_nte.UserNameType=%s\n",isp_nte.UserNameType);
printf("isp_nte.SendUserIDType=%s\n",isp_nte.SendUserIDType);
printf("isp_nte.ISPDesc1=%s\n",isp_nte.ISPDesc1);
printf("isp_nte.ISPDesc2=%s\n",isp_nte.ISPDesc2);
printf("isp_nte.ProtcolPortNo1=%s\n",isp_nte.ProtcolPortNo1);
printf("isp_nte.ProtcolPortNo2=%s\n",isp_nte.ProtcolPortNo2);
printf("isp_nte.CertifyType=%s\n",isp_nte.CertifyType);
printf("isp_nte.IPAddrType=%s\n",isp_nte.IPAddrType);
printf("isp_nte.ISPIFAddr=%s\n",isp_nte.ISPIFAddr);
printf("isp_nte.ISPRouterIP=%s\n",isp_nte.ISPRouterIP);
printf("isp_nte.NetworkAddr1=%s\n",isp_nte.NetworkAddr1);
printf("isp_nte.Subnetmask1=%s\n",isp_nte.Subnetmask1);
printf("isp_nte.ISPServerAddr1=%s\n",isp_nte.ISPServerAddr1);
printf("isp_nte.ISPServerAddr2=%s\n",isp_nte.ISPServerAddr2);
printf("isp_nte.Timeout=%s\n",isp_nte.Timeout);
printf("isp_nte.RetryCnt=%s\n",isp_nte.RetryCnt);
printf("isp_nte.PassageTime=%s\n",isp_nte.PassageTime);
printf("isp_nte.PoolAddr1=%s\n",isp_nte.PoolAddr1);
printf("isp_nte.PoolAddr2=%s\n",isp_nte.PoolAddr2);
printf("isp_nte.PoolAddr3=%s\n",isp_nte.PoolAddr3);
printf("isp_nte.MaxSession=%s\n",isp_nte.MaxSession);
printf("isp_nte.DNSInfo=%s\n",isp_nte.DNSInfo);
#if(0)
printf("isp_nte.DNSInfo2=%s\n",isp_nte.DNSInfo2);
#endif
printf("isp_nte.RediusKeys=%s\n",isp_nte.RediusKeys);
printf("isp_nte.VPI_VCI=%s\n",isp_nte.VPI_VCI);
printf("isp_nte.OamPVC=%s\n",isp_nte.OamPVC);
printf("isp_nte.AddAttribute=%s\n",isp_nte.AddAttribute);
printf("isp_nte.ConstDate=%s\n",isp_nte.ConstDate);
printf("isp_nte.ServiceStartDate=%s\n",isp_nte.ServiceStartDate);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_ISP_NTE_Update(&isp_nte);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* isp_nte_update */

/* End of File */

