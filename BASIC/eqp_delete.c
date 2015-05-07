/****************************************************************************/
/* file nanme  eqp_delete.c                                                 */
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

extern int      Proc_EQP_Delete( char*, char* );

/****************************************************************************/
/* Name : eqp_delete                                                       */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int eqp_delete(char *src_data) {

	char	tmp_data[2][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
	char	BranchCode[9];
	char	HostName[12];


	memset(&tmp_data[0][0], NULL, 2*MAX_DATA_LEN);
	memset(BranchCode, NULL, sizeof(BranchCode));
	memset(HostName, NULL, sizeof(HostName));

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
		strncpy(BranchCode, &tmp_data[0][0], EQP_BRANCHCODE_LEN);
		strncpy(HostName, &tmp_data[1][0], EQP_HOSTNAME_LEN);

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("BranchCode=%s\n",BranchCode);
printf("HostName=%s\n",HostName);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_EQP_Delete(BranchCode,HostName);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* data_delete */

/* End of File */

