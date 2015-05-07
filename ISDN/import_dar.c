/****************************************************************************/
/* file nanme  import_dar.c                                                 */
/*                                                                          */
/*  �v�`DB�A�g                                                              */
/*  �C���|�[�g                                                              */
/*                                                                          */
/* Function:                                                                */
/*          import_dar_insert()                                             */
/*          import_dar_delete()                                             */
/*                                                                          */
/* Create : Jul.02.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "output_log.h"
#include "import_dar.h"


/****************************************************************************/
/* Name : import_dar_insert                                                 */
/*                                                                          */
/* Parameter : char  *recv_data     :  IN: DAR�e�[�u�����	                */
/*                                                                          */
/* Return : 0  ����I��                                                     */
/*         -1  �ُ�I��                                                     */
/*                                                                          */
/*                                                                          */
/* Create : Jul.02.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int import_dar_insert( char *recv_data, char *send_data ) {

	int			ret;
	char		*p_tok;
	IMPORT_DAR	dar;

	printLog( DEBUGING, "*** import_dar_insert() ***" );

	memset( &dar, 0, sizeof(dar) );

	/* DAR���M�ԍ� */
	p_tok = strtok( recv_data, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.dar_no, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d DAR���M�ԍ� : %s", __FILE__, __LINE__, dar.dar_no );

	/* �x�X�R�[�h */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.branch_c, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d �x�X�R�[�h : %s", __FILE__, __LINE__, dar.branch_c );

	/* �r���R�[�h */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.build_c, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d �r���R�[�h : %s", __FILE__, __LINE__, dar.build_c );

	/* ���j�b�g�R�[�h */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.unit_c, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d ���j�b�g�R�[�h : %s", __FILE__, __LINE__, dar.unit_c );

	/* ����ԍ� */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.circuit_no, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d ����ԍ� : %s", __FILE__, __LINE__, dar.circuit_no );

	/* ����|�[�g�� */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.port_max, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d ����|�[�g�� : %s", __FILE__, __LINE__, dar.port_max );

	/* �T�[�r�X�J�n�\��N���� */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.dar_hope_day, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d �T�[�r�X�J�n�\��N���� : %s", __FILE__, __LINE__, dar.dar_hope_day );

	/* �^�p�L�� */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.status_memo, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d �^�p�L�� : %s", __FILE__, __LINE__, dar.status_memo );

	/* ���o���p�r */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.das_use, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d ���o���p�r : %s", __FILE__, __LINE__, dar.das_use );

	/* DAR�^�p��� */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.dar_status, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d DAR�^�p��� : %s", __FILE__, __LINE__, dar.dar_status );

	ret = proc_import_dar_insert( &dar );

	if( ret != 0){
		printLog( WARNING, "import_dar_insert().....ORA %d\n", ret );
		strcpy(send_data, "INSERT_IMPORT_DAR_NG");
	} else {
		printLog( DEBUGING, "import_dar_insert().....OK\n" );
		strcpy(send_data, "INSERT_IMPORT_DAR_OK");
	}

	return strlen( send_data );

} /* import_dar_insert() */


/****************************************************************************/
/* Name : import_dar_delete                                                 */
/*                                                                          */
/* Parameter : char *recv_dar_no     :  IN: DAR���M�ԍ�                     */
/*                                                                          */
/* Return : 0  ����I��                                                     */
/*         -1  �ُ�I��                                                     */
/*                                                                          */
/*                                                                          */
/* Create : Jul.02.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/

int import_dar_delete( char *recv_dar_no, char *send_data ) {

	int		ret;

	printLog( DEBUGING, "*** import_dar_delete() ***" );
	printLog( DEBUGING, "%s : %d DAR���M�ԍ� : %s", __FILE__, __LINE__, recv_dar_no );

	ret = proc_import_dar_delete( recv_dar_no );

	if( ret != 0){
		printLog( WARNING, "import_dar_delete().....ORA %d\n", ret );
		strcpy(send_data, "DELETE_IMPORT_DAR_NG");
	} else {
		printLog( DEBUGING, "import_dar_delete().....OK\n" );
		strcpy(send_data, "DELETE_IMPORT_DAR_OK");
	}

	return strlen( send_data );
}
/* End Of file */

