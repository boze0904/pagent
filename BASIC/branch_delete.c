/****************************************************************************/
/* file nanme  BRANCH_Delete.c                                              */
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

#define  DEL_DATA_LEN  8

extern int      Proc_BRANCH_Delete( char* );

/****************************************************************************/
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
int branch_delete( char *pag_data ) {

	int     del_resp = 0;

	char    tmp_data[DEL_DATA_LEN + 1];

	memset(tmp_data, 0, sizeof(tmp_data));
	strncpy(tmp_data, pag_data, DEL_DATA_LEN);  /* �w�肳�ꂽ�Œ蒷�ɂ���   */

	/*�t���b�c���e�[�u���փf�[�^���폜����*/
	del_resp = Proc_BRANCH_Delete(tmp_data);

	return del_resp;


} /* brahch_delete */


/* End of File */

