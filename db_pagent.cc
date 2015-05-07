/****************************************************************************/
/* file nanme  db_pagent.cc                                                 */
/*                                                                          */
/* NTT IP DB P-Agent用 サブ関数                                             */
/*                                                                          */
/* Function:                                                                */
/*   processInput :                                                         */
/*   msgtomout    : メッセージ出力                                          */
/*   illegalmsgout: 不正制御コマンドメッセージ出力                          */
/*                                                                          */
/* Create : 2000.12.15(dcm_pagent.cからの流用)                              */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
#include <stdio.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/time.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <unistd.h>
#include "dbp_port.h"
#include "db_pagent.h"
#include "gw_if.h"
#include "output_log.h"


/****************************************************************************/
/* External Value 															*/
extern char 	*processName;
extern DBP_INF	dbp_inf;				/* Docomo P-Agent 制御情報			*/
extern int		port_fd;				/* 読み込み対象fd：readPort( ) I/F	*/

/* Externally Defined Functions 											*/
extern void	writeDataToFifos(fd_set*,int*,char*,int);
extern void	writeDataToSocket(char*,int);
extern char	*readDataFromFifo(fd_set*,int*,int*);
extern char	*readDataFromSocket(fd_set*,int*,int*);
extern int readPort(char*,unsigned int);
extern int writePort(char*,unsigned int);


/****************************************************************************/
/* Name:                                                                    */
/*   processInput                                                           */
/*                                                                          */
/* Parameters:                                                              */
/*   int	selected    : value returned from the last select               */
/*   fd_set	*readers    : fd set of all possible descriptors to read from   */
/*   int	select_errno: value of errno after the last select              */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   This is the main dispatching routine for input into a p_agent.         */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void processInput( int select_ret, fd_set *readers_p, int select_errno )
{
	char		raw[DCM_MXMSGLEN+sizeof(DCM_TRACE_DATA_TERM)];
											/* ポ－トからの受信データ   */
    int 		raw_len;					/* ポートからの受信データ長 */
	int			*fds_p;						/* 有効記述子ポインタ       */
	int			fds_suu;					/* 有効記述子数             */
	int			cnt;						/* カウンタ                 */
    char		*sptr;						/* FIFO 又は 端末のデータ   */
	int			s_len;						/* FIFO 又は 端末のデータ長 */
    int			i;							/* ワ－ク                   */
	int		d_cnt;


	printLog( DEBUGING, "%s:%d processInput(select_ret:%d, ,select_errno:%d)",
								__FILE__,__LINE__, select_ret, select_errno );	/* V01 */
	printLog( DEBUGING, "%s:%d processName:%s port_fd:%d", __FILE__,__LINE__,
													processName, port_fd );	/* V01 */

	/* 受信データ領域初期化 */
	memset( raw, 0, sizeof(raw) );
	/* select( )関数値チェック */
	if( select_ret < 0 ){
		/* select failed */
		printf("%s %d: %d = select( ) errno %d\n", __FILE__, __LINE__,
												select_ret, select_errno );
		return  ;
	}
	else if ( select_ret == 0 ) {
		/* タイムアウト（未使用なので来ないはずだ） */
		printf("%s %d: %d = select( ) timed out errno %d\n", __FILE__, __LINE__,
												select_ret, select_errno );
		return  ;
	}

	/* Ｇ／Ｗソケット有効記述子獲得 */
	fds_p = gw_getvalidfd(readers_p, &fds_suu );
	if( fds_suu ){	/* 有効記述子ありの場合 */
		/* ポートのディスクリプタをチェック */
		for( cnt = 0; cnt < fds_suu; cnt++ ){
			/* ポート読み込み */
			port_fd = *(fds_p+cnt);		/* readPort I/F */
			raw_len = readPort( raw, sizeof(raw) );
			if( raw_len > 0 ) {


				/* トレースファイルへ書き込む */
				memcpy(&raw[raw_len],DCM_TRACE_DATA_TERM,
												sizeof(DCM_TRACE_DATA_TERM));
				/* ＦＩＦＯに書き出す */
				writeDataToFifos( NULL, NULL, raw, raw_len );
				/* 端末にNETEXPERT用データを書き込む */
				writeDataToSocket( raw, raw_len );
			}
			else if( raw_len == 0 ){
				printLog( DEBUGING, "%s:%d 0 = readPort( ) 他の子供行データの受信かな?",
														__FILE__, __LINE__ );	/* V01 */
			}
			else {
				printLog( DEBUGING, "%s:%d -1 = readPort( ) 終了", 
													__FILE__, __LINE__ );	/* V01 */
				/* 受信異常は、いまのところ終了とする。 */
				kill( getpid( ), SIGTERM );
				pause();
			}
		} /* for end */
		/* select数から、有効ポート数を削除 */
		select_ret -= fds_suu;
	}

	/* ＦＩＦＯの入力をチェック */
	if( select_ret ){
		/* ＦＩＦＯから入力する */
    	sptr = readDataFromFifo( readers_p, &select_ret, &s_len );
		/* 入力をチェック */
    	if( sptr ){
			printLog( DEBUGING, "%s:%d readDataFromFifo() からの入力", 
													__FILE__, __LINE__ );	/* V01 */
			/* ポートに出力する */
			i = writePort( sptr, s_len );	/* sendToPort() はやめちゃう */
			printLog( DEBUGING, "%s:%d %d = writePort( , len:%d)", 
										__FILE__, __LINE__, i, s_len );	/* V01 */
			/* 出力結果チェック */
			if( i != s_len ){
				printLog( DEBUGING, "%s:%d writePort( ) 異常終了 保留", 
													__FILE__, __LINE__ );	/* V01 */
			}
		}
	}

	/* 端末からの入力をチェック */
	if( select_ret ){
		/* 端末から入力する */
		sptr = readDataFromSocket( readers_p, &select_ret, &s_len );
		/* 入力をチェック */
		if( sptr ){
			/* ポートに出力する */
			printLog( DEBUGING, "%s:%d readDataFromSocket( ) からの入力", 
													__FILE__, __LINE__ );	/* V01 */
			i = writePort( sptr, s_len );	/* sendToPort() はやめちゃう */
			/* 出力結果チェック */
			if( i != s_len ){
				printLog( DEBUGING, "%s:%d writePort( ) 異常終了 保留", 
													__FILE__, __LINE__);	/* V01 */
			}
		}
	}

	/* 入力残りのチェック */
	if( select_ret ){
		printLog( DEBUGING, "%s:%d select_ret:%d", 
								__FILE__, __LINE__, select_ret );	/* V01 */
	}
} /* processInput */



/************************************************************************/
/* P-Agent メッセージ出力												*/
/************************************************************************/

/****************************************************************************/
/* Name:                                                                    */
/*   msgtomout                                                              */
/*                                                                          */
/* Prameters:                                                               */
/*   char	*msg_val: メッセージ可変部                                      */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   P-Agent メッセージをＭＯＵＴに出力する。                               */
/*   メッセージ識別は msgtomout_ini()で指定した内容を、日付・時刻及び       */
/*   ＥＯＬは当関数で付加する。                                             */
/*                                                                          */
/*   MOUT 出力時フォーマット                                                */
/*   メッセージ識別＋日付・時刻＋メッセージ可変部                           */
/*                                                                          */
/* Create : 2000.12.15(dcm_pagent.cからの流用)                              */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void msgtomout( char *msg_val )
{
	time_t		now_time;			/* 現在時刻（通算秒）				*/
	struct tm	*tm_p;				/* 現在時刻（詳細）					*/
	int			tm_year;			/* 年（１９００から）				*/
	int			len;				/* 文字列長							*/
	int			i;					/* ワ－ク							*/

	printLog( DEBUGING, "%s %d:dbp_inf.msgdisc_len %d", 
								__FILE__, __LINE__, dbp_inf.msgdisc_len );	/* V01 */
	printLog( DEBUGING, "%s %d:dbp_inf.msgdata     %s", 
								__FILE__, __LINE__, dbp_inf.msgdata );	/* V01 */

	/* 日付・時刻設定													*/
	time(&now_time);			    /* 現在時刻獲得						*/
	tm_p = localtime(&now_time);	/* 現在時刻の詳細獲得				*/
	tm_year = tm_p->tm_year;
	if( tm_year > 99 ){
		/* 2000年超の場合												*/
		tm_year -= 100;
	}
	sprintf(&dbp_inf.msgdata[dbp_inf.msgdisc_len],
		"%02d/%02d/%02d %02d:%02d:%02d ",tm_year,tm_p->tm_mon+1,
				tm_p->tm_mday,tm_p->tm_hour,tm_p->tm_min,tm_p->tm_sec);

	printLog( DEBUGING, "%s %d: data      %s", 
								__FILE__, __LINE__, dbp_inf.msgdata);	/* V01 */

	/* メッセージ可変部設定												*/
	strcat( dbp_inf.msgdata, msg_val );
	len = strlen( dbp_inf.msgdata );

	printLog( DEBUGING, "%s %d: len:%d  data %s", 
							__FILE__, __LINE__, len, dbp_inf.msgdata );	/* V01 */

	/* P-Agent出力メッセージ用ＥＯＬ設定								*/
	memcpy(&dbp_inf.msgdata[len],dbp_inf.eol, dbp_inf.eol_len);
	len += dbp_inf.eol_len;

	/* トレース出力 */
	memcpy(&(dbp_inf.msgdata[len]),DCM_TRACE_DATA_TERM,
											sizeof(DCM_TRACE_DATA_TERM));

    /* メッセージ出力 */
    writeDataToSocket(dbp_inf.msgdata, len);
    writeDataToFifos(NULL, NULL, dbp_inf.msgdata, len);

	printLog( DEBUGING, "%s %d: writeDataToFifos( )", __FILE__, __LINE__ );	/* V01 */

} /* msgtomout */



/****************************************************************************/
/* Name:                                                                    */
/*   illegalmsgout	不正制御コマンドメッセージ出力                          */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*cmd_p : IN: 不正な制御コマンド                                 */
/*   char	cmd_len: IN: 制御コマンド長                                     */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/* Description:                                                             */
/*   不正な制御コマンドを丸ごと受けて、制御コマンド不正メッセージを編集     */
/*   し、msgtomout()により メッセージを出力する。                           */
/*                                                                          */
/*   不正コマンドの最大コピー長は、（MML - (MHL + CIL + MEL )）             */
/*                                                                          */
/*   MML：最大メッセージ長                                                  */
/*   MHL：メッセージヘッダ長                                                */
/*   CIL：制御コマンド不正メッセージ固定部長                                */
/*   MEL：メッセージ用 EOL長                                                */
/*                                                                          */
/* Create : 2000.12.15(dcm_pagent.cからの流用)                              */
/* Modify :                                                                 */
/****************************************************************************/
void illegalmsgout( char *cmd_p, int cmd_len )
{
	int 	cnt, cnt2;				/* カウンタ							*/
	int		max_len;				/* 最大コピー長						*/
    int		i;						/* ワ－ク							*/
	char	msgbuf[DCM_MXMSGLEN+1];	/* メッセージ編集領域				*/


	/*	最大メッセージ長設定											*/
	max_len = DCM_MXMSGLEN;

	/*	メッセージヘッダと時分秒（９）と制御コマンド不正メッセージ固定	*/
	/*	部とＥＯＬ長の差分（最大不正制御コマンド設定数）を求める。		*/
	max_len -= (strlen(DCM_MSG_HEADER) + 9 + strlen(DCM_MSG_COMMANDERR) +
				dbp_inf.eol_len);

	/* 制御コマンド不正メッセージ固定部設定								*/
	strcpy(msgbuf,DCM_MSG_COMMANDERR);

	/*	上で求めた最大長より可変部に付加するコマンドが短い場合は、その	*/
	/*	長さ分だけ付加する。											*/
	i = cmd_len - dbp_inf.eol_len;	/* EOLを除くとなんバイトか。		*/
	if( max_len > i )
		max_len = i;

	cnt2 = strlen(DCM_MSG_COMMANDERR);
	for( cnt = 0; cnt < max_len;cnt++,cnt2++ ){
		msgbuf[cnt2] = *(cmd_p+cnt);
	}

	/*	終端に NULL をつけて（msgtomout()のインタフェース）出力。		*/
	msgbuf[cnt2] = '\0';

	/* 制御コマンド不正メッセージ出力	*/
	msgtomout(msgbuf);
} /* illegalmsgout */

/* END OF FILE */
