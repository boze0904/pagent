/****************************************************************************/
/* file nanme  dbpag_if.c                                                   */
/*                                                                          */
/* TCP/IP I/F関数群を使用して、G/W が DB P-Agent とデータ送受信を行う。     */
/*                                                                          */
/* Function:                                                                */
/*   dbp_pagif_init()                                                       */
/*   dbp_pagif_send()                                                       */
/*   dbp_pagif_recv()                                                       */
/*   dbp_pagif_term()                                                       */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h> 
#include <errno.h> 
#include <unistd.h>
#include "tcp_if.h"
#include "gw_if.h"
#include "output_log.h"



/****************************************************************************/
/*	内部変数																*/
static int			dsp_pid;			/* 表示用プロセスＩＤ				*/
static GW_PAGIF		*dcmpagif_p = NULL;	/*  P-Agent I/F データ				*/
static int			data_max;



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_init                                                         */
/*                                                                          */
/* Parameters:                                                              */
/*   int	max_len: IN: 最大データ長                                       */
/*                                                                          */
/* Return:                                                                  */
/*    0: 正常終了                                                           */
/*   -1：異常終了                                                           */
/*                                                                          */
/* Description:                                                             */
/*   P-Agent I/F 初期処理                                                   */
/*   P-Agent I/F データ領域を確保初期化する。                               */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int dbp_pagif_init( int max_len )
{
	printLog( DEBUGING, "*** dbp_pagif_init() ***" );	/* V01 */

	/* 表示用プロセスＩＤ獲得 */
	dsp_pid = getpid();

	printLog( DEBUGING, "%d=getpid()", dsp_pid );		/* V01 */

	/* P-Agent I/F データ領域確保 */
	dcmpagif_p = (GW_PAGIF*)malloc( sizeof(dcmpagif_p->data_len)+max_len );
	if( dcmpagif_p == NULL ){
		printLog( ERROR, "%s:%d(%d): NULL = malloc( %d ) errno:%d", __FILE__,
			__LINE__, dsp_pid, sizeof(dcmpagif_p->data_len)+max_len, errno);
		return -1;
	}

	data_max = max_len;

	/* 正常終了 */
	return 0;

} /* dbp_pagif_init */



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_send                                                         */
/*																			*/
/* Parameters:																*/
/*   TCPIF_INF	*tcpinf_p: IN: TCP/IP 制御情報                              */
/*   char		*to_buff : IN: 自識別名                                     */
/*   char		*data_p  : IN: データ                                       */
/*																			*/
/* Return:																	*/
/*    0: 正常終了                                                           */
/*   -1: 異常終了                                                           */
/*																			*/
/* Description:																*/
/*   NTT IP DB P-Agent にデータを送信する。                                 */
/*   TCP/IP 制御情報は、既に接続済みであること。                            */
/*   P-Agent I/F は(データ長＋データ)なので、まず先頭４バイトにデータ長を	*/
/*   コピーし、続けてデータ長分のデータをコピーして一気に送信する。			*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int dbp_pagif_send( TCPIF_INF *tcpinf_p, char *to_buff, char *data_p ) 
{
	int		i;				/* ワーク         */
	int		len;			/* 送信データ長   */


	/* P-Agent I/F データ 初期化確認 */
	if( dcmpagif_p == NULL ){
		printLog( ERROR, "%s:%d(%d)  P-Agent I/F データ 未初期化", 
											__FILE__, __LINE__, dsp_pid );	/* V01 */
		/* 異常終了 */
		return -1;
	}

	memset( dcmpagif_p, 0, data_max+sizeof(dcmpagif_p->data_len) );

	/* P-Agent I/F データ設定 */
	/* 自識別名・データ設定 */
	if( strlen(to_buff) != 0 ){
		sprintf( dcmpagif_p->data, "%s,%s", to_buff, data_p );
	}
	else{
		sprintf( dcmpagif_p->data, "%s", data_p );
	}	
	dcmpagif_p->data[strlen(dcmpagif_p->data)] = '\012';
	dcmpagif_p->data[strlen(dcmpagif_p->data)] = '\0';
	dcmpagif_p->data_len = strlen( dcmpagif_p->data );	/* データ長 */

	/* 送信データ長獲得（データ長格納領域分＋本体のデータ長） */
	len = ( sizeof(dcmpagif_p->data_len) + dcmpagif_p->data_len );

/********** 変更 ここから V01 **********/
	printLog( DEBUGING, "G/W -> P-Agent Data : %s", dcmpagif_p->data );
	printLog( DEBUGING, "Data Length : %d", dcmpagif_p->data_len );
	printLog( DEBUGING, "Send Data Length : %d", len );
/********** 変更 ここから V01 **********/

	/* データ送信 */
/***
	i = tcp_send( tcpinf_p, dcmpagif_p->data, len );
***/
	i = tcp_send( tcpinf_p, (char*)dcmpagif_p, len );
	if( i == -1 ){
		/* 異常終了 */
		return -1;
	}

	/* 正常終了 */
	return 0;

} /* dbp_pagif_send */



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_recv                                                         */
/*                                                                          */
/* Parameters:																*/
/*   TCPIF_INF	*tcpinf_p:  IN: TCP/IP 制御情報                             */
/*   char		*data_p  : OUT: データ                                      */
/*																			*/
/* Return:																	*/
/*	 etc		：受信データ長												*/
/*	 -1			：異常終了													*/
/*																			*/
/* Description:																*/
/*   NTT IP DB P-Agent からデータを受信する。                               */
/*   TCP/IP 制御情報は、既に接続済みであること。                            */
/*   P-Agent I/F (データ長＋データ)に合わせて、まず先頭４バイトを受信して   */
/*   以降のデータを受信する。                                               */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int dbp_pagif_recv( TCPIF_INF *tcpinf_p, char *data_p )
{
	int		len;		/* データ長       */
	int		len_len;	/* データ長の長さ */
	int		cnt;		/* カウンタ       */
	int		i;			/* ワーク         */
	char	*c_p;		/* ワーク         */

	printLog( DEBUGING, "*** dbp_pagif_recv() ***" );	/* V01 */

	/* P-Agent I/F データ 初期化確認 */
	if( dcmpagif_p == NULL ){
		printLog( DEBUGING, "%s:%d(%d) P-Agent I/F未初期化", 
									__FILE__, __LINE__, dsp_pid );
		/* 異常終了 */
		return -1;
	}

	/* データ長読み込み	 -> len */
	c_p = (char *)&len;
	len_len = sizeof(dcmpagif_p->data_len);
	for( cnt = 0, i = 0; cnt < len_len ; cnt += i ){
		i = tcp_recv( tcpinf_p, (c_p+cnt), len_len - cnt );
		if( i == -1 ){
			/* 異常終了 */
			return -1;
		}
	} /* for */
	printLog( DEBUGING, "%s:%d(%d) ->P-Agent データ長:0x%08x(%d)", 
									__FILE__, __LINE__, dsp_pid, len,len );	/* V01 */

	/* データ長分のデータを読み込む */
	for( cnt = 0, i = 0; cnt < len ; cnt += i ){
		i = tcp_recv( tcpinf_p, (data_p+cnt), len - cnt );
		if( i == -1 ){
			/* 異常終了 */
			return -1;
		}
	} /* for */

	/* 正常終了 */
	return len;

} /* dbp_pagif_recv */



/****************************************************************************/
/* Name:                                                                    */
/*   dbp_pagif_term                                                         */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   P-AgentＩ/F 終了処理                                                   */
/*   P-Agent I/F データ領域を解放する                                       */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void dbp_pagif_term()
{
	printLog( DEBUGING, "%s:%d(%d)  dbp_pagif_term 呼び出し", 
									__FILE__, __LINE__, dsp_pid );	/* V01 */

	/*  P-Agent I/F データ 確認 */
	if( dcmpagif_p == NULL ) 				/* 初期状態 */
		return;

	/* P-Agent I/F データ領域解放 */
	free( dcmpagif_p );

	/* P-Agent I/F データ初期化 */
	dcmpagif_p = NULL;

} /* dbp_pagif_term */

/* End of File */
