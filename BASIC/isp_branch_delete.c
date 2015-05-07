/****************************************************************************/
/* file nanme  isp_branch_delete.c                                          */
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

#define  MAX_DATA_LEN	GWP_MXPAGDTLEN + 1

extern int      Proc_ISP_BRANCH_Delete( char*, char* );


/****************************************************************************/
/* Name : isp_branch_delete                                                 */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int isp_branch_delete(char *src_data) {

	char	tmp_data[2][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
	char	BranchCode[9];
	char	ISPCode[4];


	memset(&tmp_data[0][0], NULL, 2*MAX_DATA_LEN);
	memset(BranchCode, NULL, sizeof(BranchCode));
	memset(ISPCode, NULL, sizeof(ISPCode));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 1 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 1 )
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

	if(i == 1) { 
		strncpy(BranchCode, &tmp_data[0][0], ISP_BRANCH_BRANCHCODE_LEN);
		strncpy(ISPCode, &tmp_data[1][0], ISP_BRANCH_ISPCODE_LEN);

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("BranchCode=%s\n",BranchCode);
printf("ISPCode=%s\n",ISPCode);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_ISP_BRANCH_Delete(BranchCode,ISPCode);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* isp_branch_delete */

/* End of File */

