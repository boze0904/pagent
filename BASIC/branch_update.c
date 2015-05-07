/****************************************************************************/
/* file nanme branch_update.c                                                */
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

extern int      Proc_BRANCH_Update( BRANCH* );

/****************************************************************************/
/* Name : branch_update                                                     */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int branch_update(char *src_data)
{
	char	tmp_data[3][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    BRANCH	branch;


	memset(&tmp_data[0][0], NULL, 3*MAX_DATA_LEN);
	memset(&branch, NULL, sizeof(branch));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 2 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 2 )
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

#if(0)
	if( i == 2 && strlen(tmp_data[0]) <= BRANCH_BRANCHCODE_LEN &&
					strlen(tmp_data[1]) <= BRANCH_KENNAME_LEN &&
					strlen(tmp_data[2]) <= BRANCH_BRANCHNAME_LEN )
#endif
	if( i == 2 ) {
		strncpy(branch.BranchCode, &tmp_data[0][0], BRANCH_BRANCHCODE_LEN);
		strncpy(branch.KenName, &tmp_data[1][0], BRANCH_KENNAME_LEN);
		strncpy(branch.BranchName, &tmp_data[2][0], BRANCH_BRANCHNAME_LEN);

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
printf("tmp_data[2]=%s\n",tmp_data[2]);
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("branch.BranchCode=%s\n",branch.BranchCode);
printf("branch.KenName=%s\n",branch.KenName);
printf("branch.BranchName=%s\n",branch.BranchName);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_BRANCH_Update(&branch);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* data_update */


/* End of File */

