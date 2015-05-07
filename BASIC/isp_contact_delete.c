/****************************************************************************/
/* file nanme  isp_contact_delete.c                                         */
/*                                                                          */
/* Vantive用データベースGateWayプロセス                                     */
/*                                                                          */
/* Create : 2000.12.15                                                      */
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

#define  DEL_DATA_LEN 3 

extern int      Proc_ISP_CONTACT_Delete( char* );


/****************************************************************************/
/* Name : isp_contact_delete                                                */
/*                                                                          */
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
int isp_contact_delete( char *pag_data ) {

	int     del_resp = 0;
	char	tmp_data[DEL_DATA_LEN + 1];


	memset(tmp_data, 0, sizeof(tmp_data));
	strncpy(tmp_data, pag_data, DEL_DATA_LEN);  /* 指定された固定長にきる   */

	del_resp = Proc_ISP_CONTACT_Delete(tmp_data);

	return del_resp;

} /* isp_contact_delete */

/* End of File */

