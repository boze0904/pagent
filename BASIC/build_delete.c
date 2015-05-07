/****************************************************************************/
/* file nanme  BUILD_Delete.c                                                 */
/*                                                                          */
/* Vantive�p�f�[�^�x�[�XGateWay�v���Z�X                                     */
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

#define  DEL_DATA_LEN  10

extern int      Proc_BUILD_Delete( char* );

/****************************************************************************/
/* Name : build_delete.c                                                    */
/*                                                                          */
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
int build_delete( char *pag_data  ) {

	int     del_resp = 0;
	char	tmp_data[DEL_DATA_LEN + 1];

	memset(tmp_data, 0, sizeof(tmp_data));

	/* strlen(pag_data) > DEL_DATA_LEN */
	strncpy(tmp_data, pag_data, DEL_DATA_LEN);	/* �w�肳�ꂽ�Œ蒷�ɂ���	*/

	/*�t���b�c���e�[�u���փf�[�^���폜����*/
	del_resp = Proc_BUILD_Delete(tmp_data);

	return del_resp;

} /* build_delete */

/* End of File */

