/****************************************************************************/
/* file nanme  IsdnFindFreePortNumber.c                                     */
/*                                                                          */
/*  プチDB連携                                                              */
/*  DASチャネル空き情報検索                                                 */
/*                                                                          */
/* Function:                                                                */
/*   createSendData():                                                      */
/*   findFreePortNumber():                                                  */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> 
#include "output_log.h"
#include "IsdnFindFreePortNumber.h"
#include "ProcFindFreePortNumber.h"


/*** パラメータ長 ***/
#define		BRANCH_CD	8	/* 支店コード     */
#define		BUILD_CD	10	/* ビルコード     */
#define		UNIT_CD		7	/* ユニットコード */



/****************************************************************************/
/* Name : createSendData                                                    */
/*                                                                          */
/* Parameter : struct PORT_INFO port     :  IN: ポート数情報                */
/*             char*            send_data: OUT: 送信データ                  */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Description : NetExpertへ送るメッセージの作成                            */
/*               メッセージヘッダもここで付加する                           */
/*               メッセージ形式は以下とする                                 */
/*               INS_PORT_INFO,空きポート数,運用中実装数,工事中ポート数,    */
/*                             工事中実装数,廃止ポート                      */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int createSendData( struct PORT_INFO port, char* send_data )
{
	printLog( DEBUGING, "*** createSendData() ***" );

	sprintf( send_data, "%s,%d,%d,%d,%d,%d", SEND_HEAD, 
											port.unused_port, port.all_free_port, 
											port.work_port, port.all_work_port, 
											port.wait_port );

	printLog( DEBUGING, "create message %s", send_data );
	printLog( DEBUGING, "create message length %d", strlen(send_data) );

	return strlen(send_data);

} /* createSendData() */


/****************************************************************************/
/* Name : findFreePortNumber                                                */
/*                                                                          */
/* Parameters: char* recv_data:  IN: 受信データ                             */
/*             char* send_data: OUT: 送信データ                             */
/*                                                                          */
/* Return    : int: 送信データ長                                            */
/*                                                                          */
/* Description : 受信データから支店コード、ビルコード、ユニットコード       */
/*               を切り出す                                                 */
/*               空きポート数、工事中ポート数、廃止ポート数を取得し、       */
/*               NetExpertへ送信するメッセージを作成する                    */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int findFreePortNumber( char* recv_data, char* send_data )
{
	int					ret;
	int					data_len;
	char				*work_p, *data;
	char				branch_cd[BRANCH_CD+1];
	char				build_cd[BUILD_CD+1];
	char				unit_cd[UNIT_CD+1];
	struct PORT_INFO	port_info;


	printLog( DEBUGING, "*** findFreePortNumber() ***" );

	data = strchr( recv_data, ',' );
	if( data == NULL ){
		/* 受信データがヘッダのみ */
		printLog( ERROR, "受信データがヘッダのみ" );
		return -1;
	}
	data++;

	/*** 念のため初期化 ***/
	memset( branch_cd, 0, BRANCH_CD+1 );
	memset( build_cd,  0, BUILD_CD +1 );
	memset( unit_cd,   0, UNIT_CD  +1 );

	/*** 支店コード ***/
	work_p = strtok( data, "," );
	if( work_p == NULL ){
		printLog( ERROR, "支店コードがありません" );
		return -1;
	}
	strncpy( branch_cd, work_p, BRANCH_CD );

	/*** ビルコード ***/
	work_p = strtok( NULL, "," );
	if( work_p == NULL ){
		printLog( ERROR, "ビルコードがありません" );
		return -1;
	}
	strncpy( build_cd, work_p, BUILD_CD );

	/*** ユニットコード ***/
	work_p = strtok( NULL, "," );
	if( work_p != NULL ){
		strncpy( unit_cd, work_p, UNIT_CD );
	}

	printLog( DEBUGING, "支店コード    : %s", branch_cd );
	printLog( DEBUGING, "ビルコード    : %s", build_cd );
	printLog( DEBUGING, "ユニットコード: %s", unit_cd );

	/*** 空きポート数の検索 ***/
	ret = getFreePort( branch_cd, build_cd, unit_cd, &port_info );
	if( ret ){
		return -1;
	}

	/*** 工事中ポート数の検索 ***/
	ret = getWorkPort( branch_cd, build_cd, unit_cd, &port_info );
	if( ret ){
		return -1;
	}

	/*** 廃止ポート数の検索 ***/
	ret = getWaitPort( branch_cd, build_cd, unit_cd, &port_info );
	if( ret ){
		return -1;
	}

	/*** 送信データの作成 ***/
	data_len = createSendData( port_info, send_data );

	return data_len;

} /* findFreePortNumber() */

/* End of File */

