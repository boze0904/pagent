/****************************************************************************/
/* file nanme  medo_contact_delete.c                                        */
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

#define  DEL_DATA_LEN 40

extern int      Proc_MEDO_CONTACT_Delete( char* );

/****************************************************************************/
/* file   : medo_contact_delete												*/
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
int medo_contact_delete( char *pag_data ) {

	int     del_resp = 0;
	char    tmp_data[DEL_DATA_LEN + 1];

	memset(tmp_data, 0, sizeof(tmp_data));
	strncpy(tmp_data, pag_data, DEL_DATA_LEN);  /* 指定された固定長にきる   */

	del_resp = Proc_MEDO_CONTACT_Delete(tmp_data);

	return del_resp;

} /* main */

/* End of File */

