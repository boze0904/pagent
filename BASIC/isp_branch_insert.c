/****************************************************************************/
/* file nanme  isp_branch_insert.c                                          */
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

extern int      Proc_ISP_BRANCH_Insert( ISP_BRANCH* );


/****************************************************************************/
/* Name : isp_branch_insert                                                 */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int isp_branch_insert(char *src_data) {

	char	tmp_data[12][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    ISP_BRANCH	isp_branch;


	memset(&tmp_data[0][0], NULL, 12*MAX_DATA_LEN);
	memset(&isp_branch, NULL, sizeof(isp_branch));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 11 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 11 )
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

	if(i == 11) 
	{
		strncpy(isp_branch.ISPCorpName, &tmp_data[0][0], ISP_BRANCH_ISPCORPPNAME_LEN);
		strncpy(isp_branch.ISPServiceName, &tmp_data[1][0], ISP_BRANCH_ISPSERVICENAME_LEN);
		strncpy(isp_branch.BranchCode, &tmp_data[2][0], ISP_BRANCH_BRANCHCODE_LEN);
		strncpy(isp_branch.ISPCode, &tmp_data[3][0], ISP_BRANCH_ISPCODE_LEN);
		strncpy(isp_branch.ACENumber, &tmp_data[4][0], ISP_BRANCH_ACENUMBER_LEN);
		strncpy(isp_branch.ISPIFAddr, &tmp_data[5][0], ISP_BRANCH_ISPIFADDR_LEN);
		strncpy(isp_branch.ISPRouterIP, &tmp_data[6][0], ISP_BRANCH_ISPROUTERIP_LEN);
		strncpy(isp_branch.NTEIFAddr, &tmp_data[7][0], ISP_BRANCH_NTEIFADDR_LEN);
		strncpy(isp_branch.ISPNetAddress, &tmp_data[8][0], ISP_BRANCH_ISPNETADDRESS_LEN);
		strncpy(isp_branch.ISPSubnetmask, &tmp_data[9][0], ISP_BRANCH_ISPSUBNETMASK_LEN);
		strncpy(isp_branch.CircuitType, &tmp_data[10][0], ISP_BRANCH_CIRCUITTYPE_LEN);
		strncpy(isp_branch.CircuitSpeed, &tmp_data[11][0], ISP_BRANCH_CIRCUITSPEED_LEN);

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
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("isp_branch.ISPCorpName=%s\n",isp_branch.ISPCorpName);
printf("isp_branch.ISPServiceName=%s\n",isp_branch.ISPServiceName);
printf("isp_branch.BranchCode=%s\n",isp_branch.BranchCode);
printf("isp_branch.ISPCode=%s\n",isp_branch.ISPCode);
printf("isp_branch.ACENumber=%s\n",isp_branch.ACENumber);
printf("isp_branch.ISPIFAddr=%s\n",isp_branch.ISPIFAddr);
printf("isp_branch.ISPRouterIP=%s\n",isp_branch.ISPRouterIP);
printf("isp_branch.NTEIFAddr=%s\n",isp_branch.NTEIFAddr);
printf("isp_branch.ISPNetAddress=%s\n",isp_branch.ISPNetAddress);
printf("isp_branch.ISPSubnetmask=%s\n",isp_branch.ISPSubnetmask);
printf("isp_branch.CircuitType=%s\n",isp_branch.CircuitType);
printf("isp_branch.CircuitSpeed=%s\n",isp_branch.CircuitSpeed);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_ISP_BRANCH_Insert(&isp_branch);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* isp_branch_insert */

/* End of File */

