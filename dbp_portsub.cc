/****************************************************************************/
/* file name  dbp_portsub.cc                                                */
/*																			*/
/* Function:                                                                */
/*   dbpsub_init                                                            */
/*   dbpsub_netx_gw                                                         */
/*   dbpsub_gw_netx                                                         */
/*   dbpsub_term                                                            */
/*                                                                          */
/*   NTT IP DB P-Agent G/W I/F サブ処理                                     */
/*   NetExpert側からのデータを G/W に振り分け送信する。						*/
/*   G/W 側からのデータを NetExpert と G/W に振り分け、                     */
/*   G/W の場合は送信する。                                                 */
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include "dbp_port.h"
#include "gw_if.h"
#include "output_log.h"



/****************************************************************************/
/* 内部変数																	*/
static GW_INF		*gwinf_p = NULL;		/* Ｇ／Ｗ情報テーブルポインタ	*/
#define MASTER		"BASIC"


/****************************************************************************/
/* Name:                                                                    */
/*   dbpsub_init                                                            */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*    0: 正常終了                                                           */
/*   -1: 異常終了                                                           */
/*                                                                          */
/* Description:                                                             */
/*   NTT IP DB P-Agent SUB 初期処理                                         */
/*   Ｇ／Ｗ情報テーブルを参照するため、                                     */
/*   Ｇ／Ｗ情報テーブルアドレス獲得関数を呼ぶ                               */
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int dbpsub_init()
{
	printLog( DEBUGING, "%s:%d dbpsub_init() start", __FILE__, __LINE__ );	/* V01 */

	/* Ｇ／Ｗ情報テーブルアドレス獲得 */
	gwinf_p = gw_getgwinfaddr();

	/* 正常終了 */
	return 0;

} /* dcmsub_init */



/****************************************************************************/
/* Name:																	*/
/*   dbpsub_netx_gw                                                         */
/*																			*/
/* Parameters:																*/
/*   char	*data_p	: IN: NetExpert データ                                  */
/*   int	data_len: IN: NetExpert データ長                                */
/*																			*/
/* Return:																	*/
/*   NONE                                                                   */
/*                                                                          */
/* Description:																*/
/*   NetExpert -> G/W データ処理                                            */
/*   NetExpertデータを解析して宛先を取得し、当該 G/W に送信する。			*/
/*   データは何も加工しないで送信する。										*/
/*   宛先の G/W は生成済みであること。										*/
/*   宛先の G/W が該当しない場合は、データを破棄する。						*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void dbpsub_netx_gw( char *data_p, int data_len )
{
	GW_EXEINF	*gwexeinf_p = NULL;		/* G/W 個別情報ポインタ         */
	GW_PAGIF	pagif;					/* P-Agent - G/W Prossecc I/F   */
	char		*cp;					/* ワーク                       */
	int			write_len;				/* 実際に write() する データ長 */
	int			cnt;					/* カウンタ                     */
	int			i;						/* ワーク                       */


	printLog( DEBUGING, "%s:%d dbpsub_netx_gw() start", __FILE__, __LINE__ );	/* V01 */
	printLog( DEBUGING, "data_p = %s, data_len = %d", data_p, data_len );	/* V01 */

	memset( &pagif, 0, sizeof(pagif) );

	/* 有効 G/W個別情報のＧＷ識別名から宛先を検索 */
	for( cnt = 0 ; cnt < gwinf_p->exeinfsuu; cnt++ ){
		if( !strncmp(data_p,gwinf_p->exeinf[cnt].name,
									strlen(gwinf_p->exeinf[cnt].name)) ){
			/* G/W 個別情報ポインタ設定 */
			gwexeinf_p = &(gwinf_p->exeinf[cnt]);
			/* NetExpert からのメッセージから宛先を除いたデータを取り出す */
			cp = strchr( data_p, '_' );
			cp++;
			break;
		} 
	} /* for */

	if( gwexeinf_p == NULL ){
		for( cnt = 0 ; cnt < gwinf_p->exeinfsuu; cnt++ ){
			if( !strcmp(gwinf_p->exeinf[cnt].name, MASTER)){
				/* G/W 個別情報ポインタ設定 */
				gwexeinf_p = &(gwinf_p->exeinf[cnt]);
				cp = data_p;
				break;
			}
    	} /* for */
		if( gwexeinf_p == NULL ){
			printLog( DEBUGING, "%s:%d 該当するＧＷがありません", __FILE__, __LINE__ ); /* V01 */
			printLog( DEBUGING, "G/W識別名 : %.10s", data_p );  /* V01 */
			return;
		}
	}


	/* 送信データをセット */
	printLog( DETAIL, "コピー前 %s", cp );
	memcpy( pagif.data, cp, strlen(cp) );
/*
 	pagif.data[strlen(pagif.data)-1] = '\0';
*/
 	pagif.data[strlen(pagif.data)] = '\0';
	printLog( DETAIL, "コピー後 %s", pagif.data );

	/* 実データ長をセット */
	pagif.data_len = strlen( pagif.data );

	/* 送信データ長をセット */
	write_len = sizeof(pagif.data_len) + pagif.data_len;

	/* 当該Ｇ/Ｗにデータを送信する */
	i = write( gwinf_p->exeinf[cnt].sd, &pagif, write_len );
	if( i != write_len ){
		/* エラーメッセージ表示 */
		printLog( ERROR, "%s:%d %d = write(%d,,%d ) errno:%d", __FILE__, __LINE__, i,
				gwinf_p->exeinf[cnt].sd, write_len, errno );	/* V01 */
		return;
	}

/********** 変更 ここから V01 **********/
	printLog( DEBUGING, "%s:%d 当該Ｇ/Ｗにデータ送信 (%d byte)", __FILE__, __LINE__, i );
	printLog( DEBUGING, "G/W : %s", gwinf_p->exeinf[cnt].name );
	printLog( DEBUGING, "実データ長 : %d, 送信データ長 : %d", pagif.data_len, write_len );
	printLog( DEBUGING, "送信データ : %s", pagif.data );
/********** 変更 ここまで V01 **********/

	return;

} /* dbpsub_netx_gw */



/****************************************************************************/
/* Name:																	*/
/*   dbpsub_gw_netx                                                         */
/*																			*/
/* Parameters:																*/
/*   char	*data_p		：IN	G/W データ                                  */
/*   int	data_len	：IN	G/W データ長                                */
/*																			*/
/* Return:																	*/
/*     0: G/W データ送信                                                    */
/*    -1: 異常終了(致命的)                                                  */
/*   etc: NetExpert データ長                                                */
/*																			*/
/* Description:																*/
/*   G/W -> NetExpert データ処理                                            */
/*   NetExpert データの場合は、G/W データから宛先を除いて                   */
/*   そのデータ長を返却する。                                               */
/*   宛先の G/W が該当しない場合は、データを破棄する。                      */
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int dbpsub_gw_netx( char *data_p, int data_len )
{
/********** 変更 ここから V01 **********/
	printLog( DEBUGING, "%s:%d dbpsub_gw_netx() start", __FILE__, __LINE__ );
	printLog( DEBUGING, "data_p = %s", data_p );
	printLog( DEBUGING, "data_len = %d", data_len );
/********** 変更 ここから V01 **********/

	return data_len;

} /* dbpsub_gw_netx */



/****************************************************************************/
/* Name:																	*/
/*   dbpsub_term                                                            */
/*																			*/
/* Parameters:																*/
/*   NONE                                                                   */
/*																			*/
/* Return:																	*/
/*   NONE                                                                   */
/*																			*/
/* Description:																*/
/*   NTT IP DB P-Agent SUB  終了処理                                        */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void dbpsub_term()
{
	printLog( DEBUGING, "%s:%d dbpsub_term() start", __FILE__, __LINE__ );	/* V01 */
	return;
} /* dbpsub_term */

/* End of File */
