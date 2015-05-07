/****************************************************************************/
/* file nanme  IsdnMain.c                                                   */
/*                                                                          */
/* プチDB ISDN用 GateWayプロセス                                            */
/*                                                                          */
/* Function:                                                                */
/*   createErrorData()                                                      */
/*   processDB()                                                            */
/*   gwp_getarg()                                                           */
/*   gwp_term()                                                             */
/*   gwp_sigterm()                                                          */
/*   set_signal()                                                           */
/*   ready_log()                                                            */
/*   main()                                                                 */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <time.h>
#include <fcntl.h>
#include <signal.h> 
#include <errno.h> 
#include <string.h> 
#include "tcp_if.h"
#include "gw_if.h"
#include "dbpag_if.h"
#include "db_func.h"
#include "common.h"
#include "output_log.h"
#include "read_prop_file.h"
#include "IsdnMain.h"
#include "IsdnFindFreePortNumber.h"
#include "import_dar.h"

#define	MAX_DATA_LEN	GWP_MXPAGDTLEN + 1


/****************************************************************************/
/*	(SCPE I/F)内共通変数													*/
int						dsp_pid;		/* 表示用プロセスＩＤ				*/


/****************************************************************************/
/*	内部変数																*/
static GWP_INF	gwp_inf;				/* DB P-Agent G/W 制御情報          */


/****************************************************************************/
/* Name : createErrorData                                                   */
/*                                                                          */
/* Parameter  : char* send_data: OUT: 送信用データ                          */
/*                                                                          */
/* Return     :  0 以上: 送信データ長                                       */
/*                                                                          */
/* Discripsion: 異常処理時にNetExpertへ異常処理終了通知のデータの作成       */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int createErrorData( char* send_data )
{
	printLog( DEBUGING, "*** createErrorData() ***" );

    sprintf( send_data, "%s,%d,%d,%d,%d,%d", SEND_HEAD, -1, -1, -1, -1, -1 );

    return strlen(send_data);

} /* createErrorData() */


/****************************************************************************/
/* Name : processDB                                                         */
/*                                                                          */
/* Parameter  : char* recv_data:  IN: 受信データ                            */
/*              char* send_data: OUT: 送信用データ                          */
/*                                                                          */
/* Return     :  0 以上: 送信データ長                                       */
/*              -1     : 異常終了                                           */
/*                                                                          */
/* Discripsion: 受信データのヘッダ別にデータ処理用の関数を呼び出す          */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int processDB( char* recv_data, char* send_data )
{
	int		data_len;
	char*	data;

	printLog( DEBUGING, "*** processDB() ***" );

	if( !strncmp( recv_data, FIND_FREE_PORT_NUMBER, strlen(FIND_FREE_PORT_NUMBER) ) ){
		data_len = findFreePortNumber( recv_data, send_data );
		if( data_len == -1 ){
			printLog( WARNING, "処理が異常終了しました" );
			data_len = createErrorData( send_data );
		}
	} else if( !strncmp( recv_data, H_IMPORT_INSERT, strlen(H_IMPORT_INSERT) ) ){
		data = strchr( recv_data, ',' );
		data++;
		data_len = import_dar_insert( data, send_data );
	} else if( !strncmp( recv_data, H_IMPORT_DELETE, strlen(H_IMPORT_DELETE) ) ){
		data = strchr( recv_data, ',' );
		data++;
		data_len = import_dar_delete( data, send_data );
	} else {
		printLog( WARNING, "受信データのヘッダ(%s)に該当する処理は未登録", strtok(recv_data,",") );
		return 0;
	}

    return data_len;

} /* processDB() */


/****************************************************************************/
/* Name : gwp_getarg                                                        */
/*                                                                          */
/* Parameters : int		argc       ：IN  main argc                          */
/*              char	**argv     ：IN  main argv                          */
/*              GWP_INF	*gwp_inf_p ：OUT G/W プロセス制御情報テーブル       */
/*                                                                          */
/* Return :  0: 正常終了                                                    */
/*          -1: 異常終了                                                    */
/*                                                                          */
/* Description: 起動パラメータ獲得                                          */
/*              main() の引数から G/W プロセス制御情報テーブルの以下の項目  */
/*              に値を設定する。不正な引数の場合は異常終了する。            */
/*              ・Docomo環境ファイルパス名                                  */
/*              ・P-Agent ホスト名                                          */
/*              ・P-Agent ポート番号                                        */
/*              ・自 識別名                                                 */
/*              ・DB ユーザー名                                             */
/*              ・DB SID                                                    */
/*              ・パスワード                                                */
/*              ・pipeパス名（省略可）                                      */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int gwp_getarg(int argc, char **argv, GWP_INF *gwpinf_p )
{
	int		dbg_cnt;

	printLog( DEBUGING, "%s:%d(%d) MAIN の引数の数", __FILE__, __LINE__, dsp_pid );
	for(dbg_cnt = 0; dbg_cnt < argc; dbg_cnt++)
		printLog( DEBUGING, "\t[%d]:%s", dbg_cnt, *(argv+dbg_cnt) );

	/* パラメータ数確認 */
	if( argc < 7 ){
		printLog( ERROR, "%s:%d(%d) MAIN の引数の数(%d)が規定数でない",
										__FILE__, __LINE__, dsp_pid, argc );
		/* 異常終了 */
		return -1;
	}

	/* DB G/W環境ファイルパス名獲得 */
	strcpy( gwpinf_p->dcmenvpath, *(argv+1) );

	/* P-Agent ホスト名獲得 */
	strcpy( gwpinf_p->pagent_host, *(argv+2) );

	/* P-Agent ポート番号獲得 */
	gwpinf_p->pagent_port = atol( *(argv+3) );

	/* 自 識別名獲得 */
	strcpy( gwpinf_p->owneqpname, *(argv+4) );

	/* ユーザー名 */
	strcpy( gwpinf_p->usr_name, *(argv+5) );

	/* パスワード */
	strcpy( gwpinf_p->passwd, *(argv+6) );

	printLog( DEBUGING, "%s:%d 環境ファイル名     :%s", 
							__FILE__, __LINE__, gwpinf_p->dcmenvpath );
	printLog( DEBUGING, "%s:%d P-Agent            :%s %d", 
						__FILE__, __LINE__, gwpinf_p->pagent_host, gwpinf_p->pagent_port );
	printLog( DEBUGING, "%s:%d 自 識別名          :%s", 
							__FILE__, __LINE__, gwpinf_p->owneqpname );
	printLog( DEBUGING, "%s:%d ユーザ名           :%s", 
							__FILE__, __LINE__, gwpinf_p->usr_name );
	printLog( DEBUGING, "%s:%d パスワード         :%s", 
							__FILE__, __LINE__, gwpinf_p->passwd );
	/* SID */
	if( argc > 7 ){
		strcpy( gwpinf_p->sid, *(argv+7) );
		printLog( DEBUGING, "%s:%d SID                :%s", 
							__FILE__, __LINE__, gwpinf_p->sid );
	}

	/* 正常終了 */
	return 0;

} /* gwp_getarg() */


/****************************************************************************/
/* Name : gwp_term                                                          */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return : NONE                                                            */
/*                                                                          */
/* Description : G/W 終了処理                                               */
/*               DBとの接続を切断する                                       */
/*               "Pagent TCP/IPソケット" を閉じる。                         */
/*               Docomo P-Agent I/F 終了処理。                              */
/*               当関数でプロセスの終了（exit( ) の呼出し）はしない。       */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
static void gwp_term()
{
	printLog( DEBUGING, "*** gwp_term() ***" );
	
    /* DBとの切断 */
    DbDisconnect();

	printLog(WARNING, "isdnMain データベース切断");

	/* Pagent TCP/IPソケットを閉じる */
	tcp_close( &(gwp_inf.tcpinf_pagent) );

	/* P-Agent I/F 終了処理 */
	dbp_pagif_term();

} /* gwp_term() */


/****************************************************************************/
/* Name : gwp_sigterm                                                       */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return : 0：正常終了                                                     */
/*          1：異常終了                                                     */
/*                                                                          */
/* Description : G/W プロセスシグナル終了処理		SIGTERM 登録            */
/*               G/W はシグナルによる終了しか行なわない。                   */
/*               G/W 終了処理を行なう。                                     */
/*               SIGTERM の受信を正常の終了とし、不正なシグナル受信は       */
/*               異常終了とする。                                           */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
void gwp_sigterm( int sig_num )
{
	printLog( DEBUGING, "%s:%d(%d) gwp_sigterm(%d)", 
							__FILE__, __LINE__, dsp_pid, sig_num );

	if( sig_num != SIGTERM )
		printLog( ERROR, "%s %d(%d):不正シグナル(%d)受信",
										__FILE__, __LINE__, dsp_pid, sig_num );
	else
		printLog( DEBUGING, "%s %d(%d):終了シグナル(%d)受信", 
										__FILE__, __LINE__, dsp_pid, sig_num );

	/* G/W プロセス終了処理 */
	gwp_term();

	/* プロセスの終了値を返す */
	if( sig_num == SIGTERM )
		/* プロセス正常終了 */
		exit(0);
	else
		/* プロセス異常終了 */
		exit(1);
} /* gwp_sigterm() */

 
/****************************************************************************/
/* Name : set_signal                                                        */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return     : NONE                                                        */
/*                                                                          */
/* Description: プロセスの終了シグナルを登録する                            */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
void set_signal()
{
	printLog( DEBUGING, "*** set_signal() ***" );

	/* 終了シグナル登録（G/W プロセス終了処理） */
	signal( SIGTERM, gwp_sigterm );
	/* 通常処理で受信しないシグナルの登録 */
	signal( SIGHUP,  gwp_sigterm );
	signal( SIGINT,  gwp_sigterm );
	signal( SIGQUIT, gwp_sigterm );
	signal( SIGILL,  gwp_sigterm );
	signal( SIGALRM, gwp_sigterm );
	signal( SIGEMT,  gwp_sigterm );
	signal( SIGFPE,  gwp_sigterm );
	signal( SIGBUS,  gwp_sigterm );
	signal( SIGSEGV, gwp_sigterm );
	signal( SIGSYS,  gwp_sigterm );
	signal( SIGUSR1, gwp_sigterm );
	signal( SIGPWR,  gwp_sigterm );
	signal( SIGPIPE, gwp_sigterm );
	printLog( DEBUGING, "%s:%d(%d) 通常処理で受信しないシグナルの登録", 
											__FILE__, __LINE__, dsp_pid );
} /* set_signal() */


/****************************************************************************/
/* Name : ready_log                                                         */
/*                                                                          */
/* Parameters : NONE                                                        */
/*                                                                          */
/* Return     :  0: 正常                                                    */
/*              -1: 異常                                                    */
/*                                                                          */
/* Description: ログの準備をする                                            */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int ready_log()
{
	int		ret;
	int		lvl;
	char	configful_path[512];
	char    *home;
    char    logfile[512];
	char	*c_p;
	char	*lpath;


	printLog( DEBUGING, "*** read_log() ***" );

	/* コンフィグファイル名 （フルパス）獲得 */
	c_p = getenv( DBP_ENVPATH );
    if( c_p == NULL ){
		/* エラーメッセージ表示 */
		printf( "%s:%d 環境変数(%s)未設定\n", __FILE__, __LINE__, DBP_ENVPATH );
		fflush( stdout );
		/* 異常終了 */
		return -1;
    }
	sprintf( configful_path, "%s/%s", c_p, DBP_CONF );

    /* 存在確認 */
	ret = access( configful_path, R_OK );
    if( ret ){
		/* エラーメッセージ表示 */
		printf( "%s:%d コンフィグファイル access(%s) error:%d\n",
		        	__FILE__,__LINE__, configful_path, errno );
		fflush( stdout );

		/* 異常終了 */
		return -1;
    }
#ifdef DEBUG
printf( "%s:%d コンフィグファイル access(%s)\n",
		        	__FILE__, __LINE__, configful_path );
#endif

	/*** ログ出力の準備 ***/
	/* 設定ファイルをオープン */
	ret = openPropFile( configful_path );
    if( ret ){
		/* エラーメッセージ表示 */
		printf( "%s:%d コンフィグファイル openPropFile(%s) error:%d\n",
		        	__FILE__,__LINE__, configful_path, errno );
		fflush( stdout );
		/* 異常終了 */
		return -1;
    }

	/* ログ関係プロパティ値の取得 */
	lpath = getProperty( LOGPATH );
	c_p = getProperty( LOGLVL );
#ifdef DEBUG
printf( "Log path %s\n", lpath );
printf( "Log level %s\n", c_p );
#endif
    if( c_p == NULL  ){
		lvl = WARNING;
    }
    else{
		lvl = atoi( c_p );
    }
	
	if( lpath == NULL ){
		home = getenv( OSI_HOME );
		if( home == NULL ){
			printf("システムエラー\n");
			fflush( stdout );
			return -1;
		}
		sprintf( logfile, "%s/Log", home );
		lpath = logfile;
	}	

	/* ログファイルの準備 */
	openLog( lpath, lvl );

	/* 設定ファイル処理の後始末 */
    closePropFile();

	return 0;

} /* ready_log() */


/****************************************************************************/
/* Name : main                                                              */
/*                                                                          */
/* Parameters : exe_name     ：実行形式ファイルパス名                       */
/*              dcm_env_path ：Docomo環境ファイルパス名                     */
/*              pagent_host  ：P-Agentホスト名                              */
/*              pagent_port  ：P-Agentポート番号                            */
/*              own_name     ：自 識別名                                    */
/*              usr_name     ：DBユーザー名                                 */
/*              password     ：Pass Word                                    */
/*              sid          ：DB SID                                       */
/*              [pipe_path]  ：パイプファイルパス名(省略可)                 */
/*                                                                          */
/* Return : NONE                                                            */
/*                                                                          */
/* Description : DBと接続をして、SQLをプチDBに発行します。                  */
/*               NetExpertへ返信があれば、それを送信します                  */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
int main( int argc, char **argv )
{
	fd_set	readers;					/* ファイルディスクリプタ   */
	char	pag_data[GWP_MXPAGDTLEN];	/* P-Agent データ格納域     */
	char	send_data[GWP_MXPAGDTLEN];	/* P-Agentへの データ格納域 */
	int		data_len;					/* データ格納域有効長       */
	int		pagent_sd = 0;				/* P-Agent 記述子           */
	int		sel_ret;					/* select() 返り値          */
	int		ret;						/* ワーク                   */
	int     con_cnt = 0;


	/*** ログの準備 ***/
	ret = ready_log();
	if( ret ){
		return -1;
 	}

	printLog( DEBUGING, "***ISDN MAIN START ***" );

	/* ＤＥＢＵＧ表示用プロセスＩＤ */
	dsp_pid = getpid();

	/* DB P-Agent G/W 制御情報 初期化 */
	memset( &gwp_inf, 0, sizeof(gwp_inf) );

	/* 起動パラメータ獲得 */
	ret = gwp_getarg( argc, argv, &gwp_inf );
	if( ret == -1 ){
		printLog( ERROR, "引数エラー" );
		/* 終了 */
		exit( 1 );
	}

	/*** シグナル関数の登録 ***/
	set_signal();

	/* DB P-Agent Ｉ／Ｆ接続 */
	sleep(2);
	while( con_cnt < 30 ){
		sleep(1);
		ret = tcp_conn( &(gwp_inf.tcpinf_pagent), gwp_inf.pagent_host,
						gwp_inf.pagent_port );
		if(ret != -1){
			break;
		}
		con_cnt++;
	}

	if( ret == -1 ){
		printLog( ERROR, "TCP接続ができませんでした" );
		/* G/W プロセス終了処理 */
		gwp_term();
		/* 終了 */
		exit( 1 );
	}

	printLog( DEBUGING, "%s:%d(%d) DB P-Agent Ｉ／Ｆ接続完了", 
										__FILE__, __LINE__, dsp_pid );

	/* P-Agent I/F 初期処理 */
	ret = dbp_pagif_init( GWP_MXPAGDTLEN );	/* P-Agent I/F の最大長を指定 */
	if( ret == -1 ){
		printLog( ERROR, "メモリ確保ができませんでした" );
		/* G/W プロセス終了処理 */
		gwp_term();
		/* 終了 */
		exit( 1 );
	}

	/* P-Agent 記述子獲得 */
	pagent_sd = tcp_getsd( &(gwp_inf.tcpinf_pagent) );

	/*** DB への接続 ***/ 
    ret = DbConnect( gwp_inf.usr_name, gwp_inf.passwd, gwp_inf.sid );
    if( ret != 0 ){
        printLog( ERROR, "データベースに接続ができませんでした" );

        /* G/W プロセス終了処理 */
        gwp_term();

        /* 終了 */
        exit( 1 );
    }
	printLog(WARNING,"%s データベース接続完了", argv[0]);

	/* 通常処理 */
	while(1){
		FD_ZERO( &readers );
		FD_SET( pagent_sd, &readers );
		memset(pag_data, 0, sizeof(pag_data));
		sel_ret = select( FD_SETSIZE, &readers, (fd_set *)0, (fd_set *)0,
														NULL );
		/* P-Agent からの入力を確認 */
		if( FD_ISSET( pagent_sd , &readers ) ){
			/* P-Agentデータ受信 */
			data_len = dbp_pagif_recv( &(gwp_inf.tcpinf_pagent), pag_data );

			printLog( DEBUGING, "受信データ: %s", pag_data );

			if( data_len > 0 && data_len <= GWP_MXPAGDTLEN )
			{
				/* データベース処理関数への振り分け */ 
				data_len = processDB( pag_data, send_data );

				if( data_len > 0 ){
					/* 送信データ長が0より大きい場合、データを送信する */
					ret = dbp_pagif_send( &(gwp_inf.tcpinf_pagent), "", send_data );	
				}
			}
			else if( data_len == -1 ) {
				printLog( ERROR, "データ受信エラー" );
				break;
			}
			else {
				printLog( WARNING, "P-Agentから受信したデータエラー: %s", pag_data );
			}
		}
	} /* while */

	/* G/W プロセス終了処理 */
	gwp_term();

	exit(1);

} /* main() */

/* End of File */

