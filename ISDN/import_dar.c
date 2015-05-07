/****************************************************************************/
/* file nanme  import_dar.c                                                 */
/*                                                                          */
/*  プチDB連携                                                              */
/*  インポート                                                              */
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
/* Parameter : char  *recv_data     :  IN: DARテーブル情報	                */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
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

	/* DAR着信番号 */
	p_tok = strtok( recv_data, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.dar_no, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d DAR着信番号 : %s", __FILE__, __LINE__, dar.dar_no );

	/* 支店コード */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.branch_c, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d 支店コード : %s", __FILE__, __LINE__, dar.branch_c );

	/* ビルコード */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.build_c, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d ビルコード : %s", __FILE__, __LINE__, dar.build_c );

	/* ユニットコード */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.unit_c, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d ユニットコード : %s", __FILE__, __LINE__, dar.unit_c );

	/* 回線番号 */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.circuit_no, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d 回線番号 : %s", __FILE__, __LINE__, dar.circuit_no );

	/* 回線ポート数 */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.port_max, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d 回線ポート数 : %s", __FILE__, __LINE__, dar.port_max );

	/* サービス開始予定年月日 */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.dar_hope_day, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d サービス開始予定年月日 : %s", __FILE__, __LINE__, dar.dar_hope_day );

	/* 運用記事 */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.status_memo, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d 運用記事 : %s", __FILE__, __LINE__, dar.status_memo );

	/* 払出し用途 */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.das_use, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d 払出し用途 : %s", __FILE__, __LINE__, dar.das_use );

	/* DAR運用状態 */
	p_tok = strtok( NULL, "," );
	if( p_tok == NULL ){
		return -1;
	}
	strncpy( dar.dar_status, p_tok, strlen(p_tok));
	printLog( DEBUGING, "%s : %d DAR運用状態 : %s", __FILE__, __LINE__, dar.dar_status );

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
/* Parameter : char *recv_dar_no     :  IN: DAR着信番号                     */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/*                                                                          */
/* Create : Jul.02.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/

int import_dar_delete( char *recv_dar_no, char *send_data ) {

	int		ret;

	printLog( DEBUGING, "*** import_dar_delete() ***" );
	printLog( DEBUGING, "%s : %d DAR着信番号 : %s", __FILE__, __LINE__, recv_dar_no );

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

