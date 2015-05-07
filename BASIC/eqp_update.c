/****************************************************************************/
/* file nanme  eqp_update.c                                                 */
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

extern int      Proc_EQP_Update( EQP* );


/****************************************************************************/
/* Name : eqp_update                                                       */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int eqp_update(char *src_data) {

	char	tmp_data[5][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    EQP	eqp;


	memset(&tmp_data[0][0], NULL, 5*MAX_DATA_LEN);
	memset(&eqp, NULL, sizeof(eqp));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 4 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 4 )
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
	if(i == 4 && strlen(tmp_data[0]) <= EQP_BRANCHCODE_LEN &&
					strlen(tmp_data[1]) <= EQP_HOSTNAME_LEN &&
					strlen(tmp_data[2]) <= EQP_PRODUCTNAME_LEN &&
					strlen(tmp_data[3]) <= EQP_MAKERCONTACTID_LEN &&
					strlen(tmp_data[4]) <= EQP_COMPONENTID_LEN )
#endif
	if(i == 4) 
	{
		strncpy(eqp.BranchCode, &tmp_data[0][0], EQP_BRANCHCODE_LEN);
		strncpy(eqp.HostName, &tmp_data[1][0], EQP_HOSTNAME_LEN);
		strncpy(eqp.ProductName, &tmp_data[2][0], EQP_PRODUCTNAME_LEN);
		strncpy(eqp.MakerContactid, &tmp_data[3][0], EQP_MAKERCONTACTID_LEN);
		strncpy(eqp.Componentid, &tmp_data[4][0], EQP_COMPONENTID_LEN);

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
printf("tmp_data[2]=%s\n",tmp_data[2]);
printf("tmp_data[3]=%s\n",tmp_data[3]);
printf("tmp_data[4]=%s\n",tmp_data[4]);
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("eqp.BranchCode=%s\n",eqp.BranchCode);
printf("eqp.HostName=%s\n",eqp.HostName);
printf("eqp.ProductName=%s\n",eqp.ProductName);
printf("eqp.MakerContactid=%s\n",eqp.MakerContactid);
printf("eqp.Componentid=%s\n",eqp.Componentid);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_EQP_Update(&eqp);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* eqp_update */

/* End of File */

