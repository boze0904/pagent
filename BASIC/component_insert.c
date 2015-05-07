/****************************************************************************/
/* file nanme  component_insert.c                                           */
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

extern int      Proc_COMPONENT_Insert( COMPONENT* );

/****************************************************************************/
/* Name : component_insert                                                  */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int component_insert(char *src_data) {

	char	tmp_data[5][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    COMPONENT	component;


	memset(&tmp_data[0][0], NULL, 5*MAX_DATA_LEN);
	memset(&component, NULL, sizeof(component));

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
	if(i == 4 && strlen(tmp_data[0]) <= COMPONENT_COMPONENTID_LEN && 
					strlen(tmp_data[1]) <= COMPONENT_COMPONENTNO_LEN &&
					strlen(tmp_data[2]) <= COMPONENT_COMPONENTKATANO_LEN && 
					strlen(tmp_data[3]) <= COMPONENT_COMPONENTKANANAME_LEN &&
					strlen(tmp_data[4]) <= COMPONENT_COMPONENTNUM_LEN ) 
#endif
	if(i == 4) 
	{
		strncpy(component.Componentid, &tmp_data[0][0], COMPONENT_COMPONENTID_LEN);
		strncpy(component.ComponentNo, &tmp_data[1][0], COMPONENT_COMPONENTNO_LEN);
		strncpy(component.ComponentKataNo, &tmp_data[2][0], COMPONENT_COMPONENTKATANO_LEN);
		strncpy(component.ComponentName, &tmp_data[3][0], COMPONENT_COMPONENTKANANAME_LEN);
		strncpy(component.ComponentNum, &tmp_data[4][0], COMPONENT_COMPONENTNUM_LEN);

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
printf("tmp_data[2]=%s\n",tmp_data[2]);
printf("tmp_data[3]=%s\n",tmp_data[3]);
printf("tmp_data[4]=%s\n",tmp_data[4]);
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("component.Componentid=%s\n",component.Componentid);
printf("component.ComponentNo=%s\n",component.ComponentNo);
printf("component.ComponentKataNo=%s\n",component.ComponentKataNo);
printf("component.ComponentName=%s\n",component.ComponentName);
printf("component.ComponentNum=%s\n",component.ComponentNum);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_COMPONENT_Insert(&component);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* component_insert */

/* End of File */

