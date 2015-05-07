/****************************************************************************/
/* file nanme  basic.c  	                                                */
/*                                                                          */
/* NetExpert外部DB用G/W														*/
/*                                                                          */
/* Create : 2001.06.27                                                      */
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
#include "pagent_trace.h"
#include "proc_str.h"
#include "basic.h"
#include "common.h"
#include "output_log.h"
#include "read_prop_file.h"

#define  MAX_DATA_LEN   GWP_MXPAGDTLEN + 1


/****************************************************************************/
/*	内部関数																*/
/*	基本処理系																*/
static int		gwp_getarg(int,char**,GWP_INF*);	/* 起動パラメータ獲得   */
static void		gwp_term();				/* G/W 終了処理                     */
static void		gwp_sigterm(int);			/* G/W プロセス終了処理(SIGNAL受信) */

/****************************************************************************/
/*	(SCPE I/F)内共通変数													*/
int						dsp_pid;		/* 表示用プロセスＩＤ				*/


/****************************************************************************/
/*	内部変数																*/
static GWP_INF	gwp_inf;				/* DB P-Agent G/W 制御情報          */


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
/* Description : DBと接続をして、select文以外のSQLをDBに発行します。        */
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
int main( int argc, char **argv ) {
	fd_set	readers;					/* ファイルディスクリプタ  */
	char	pag_data[GWP_MXPAGDTLEN];	/* P-Agent データ格納域    */
	int		data_len;					/* データ格納域有効長      */
	int		pagent_sd = 0;				/* P-Agent 記述子          */
	int		sel_ret;						/* select() 返り値         */
	int		ret;							/* ワーク                  */
	char	head_buff[GWP_MXPAGDTLEN];
	char	*c_p;
	char	*dbp_conf;
	char	configfile[64];
	char	*lpath;						/* ログパス					*/
	int		lvl;						/* ログレベル				*/
	char	*home;						/* OSI_HOME					*/
	char	logfile[512];				/* ログファイル				*/
	int     err_resp;
	int     con_cnt = 0;

	/* 追加 ここから V01 */
	/* 環境変数取得 */
	if((dbp_conf = getenv("DBP_ENV_PATH")) == NULL){
		printf("システムエラー\n");
		fflush(stdout);
		exit(-1);
	}
	memset(configfile, 0, sizeof(configfile));
	sprintf(configfile, "%s/dbp.conf", dbp_conf);

	/*** ログ出力の準備 ***/
	/* 設定ファイルをオープン */
	ret = openPropFile( configfile );
	if( ret ){
		/* エラーメッセージ表示 */
		printf( "%s:%d コンフィグファイル openPropFile(%s) error:%d\n",
							__FILE__,__LINE__, configfile, errno );
		fflush( stdout );
		/* 異常終了 */
		exit(-1);
    }
	/* ログ関係プロパティ値の取得 */
	lpath = getProperty( LOGPATH );
	c_p = getProperty( LOGLVL );
#ifdef DEBUG
printf( "Log path %s\n", lpath );
printf( "Log level %s\n", c_p );
#endif

	if( lpath == NULL ){
		home = getenv( "OSI_HOME" );
		if( home == NULL ){
			printf("システムエラー\n");
			fflush( stdout );
			exit(1);
		}
		sprintf( logfile, "%s/Log", home );
		lpath = logfile;
	}

	if( c_p == NULL  ){
		lvl = WARNING;
	}
	else{
		lvl = atoi( c_p );
	}

	/* ログファイルの準備 */
	openLog( lpath, lvl );

	/* 設定ファイル処理の後始末 */
	closePropFile();

/********** 追加 ここまで V01 **********/

	printLog(DEBUGING,"%s:%d basic", __FILE__, __LINE__);
#ifdef DEBUG
printf( "%s:%d basic DEBUG\n", __FILE__, __LINE__);
#endif


	/* ＤＥＢＵＧ表示用プロセスＩＤ */
	dsp_pid = getpid();

	/* DB P-Agent G/W 制御情報 初期化 */
	memset( &gwp_inf, 0, sizeof(gwp_inf) );

	/* 起動パラメータ獲得 */
	ret = gwp_getarg( argc, argv, &gwp_inf );
	if( ret == -1 ){
		printLog( ERROR, "引数エラー");
		exit( 1 );
	}

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
											__FILE__, __LINE__, dsp_pid);

	/* DB P-Agent Ｉ／Ｆ接続 */
	sleep(2);
	while(con_cnt < 30){
		sleep(1);
		ret = tcp_conn( &(gwp_inf.tcpinf_pagent), gwp_inf.pagent_host,
						gwp_inf.pagent_port );
		if(ret != -1){
			break;
		}

		con_cnt++;
	}

	if( ret == -1 ){
		printLog(ERROR, "TCP接続ができませんでした");
		/* G/W プロセス終了処理 */
		gwp_term();
		/* 終了 */
		exit( 1 );
	}

	printLog( DEBUGING, "%s:%d(%d) DB P-Agent Ｉ／Ｆ接続完了", 
										__FILE__, __LINE__, dsp_pid);

	/* P-Agent I/F 初期処理 */
	ret = dbp_pagif_init( GWP_MXPAGDTLEN );	/* P-Agent I/F の最大長を指定 */
	if( ret == -1 ){
		printLog(ERROR, "メモリ確保ができませんでした");
		/* G/W プロセス終了処理 */
		gwp_term();
		/* 終了 */
		exit( 1 );
	}

	/* P-Agent 記述子獲得 */
	pagent_sd = tcp_getsd( &(gwp_inf.tcpinf_pagent) );

	/* DB接続 */
	ret = DbConnect(gwp_inf.usr_name, gwp_inf.passwd, gwp_inf.sid);
	if( ret != 0 ){
		printLog(ERROR, "データベースに接続ができませんでした");
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
		sel_ret = select( FD_SETSIZE, &readers, (fd_set *)0, (fd_set *)0, NULL );
		/* P-Agent からの入力を確認 */
		if( FD_ISSET( pagent_sd , &readers ) ){
			/* P-Agentデータ受信 */
			data_len = dbp_pagif_recv( &(gwp_inf.tcpinf_pagent), pag_data );

			printLog(DEBUGING, "%s:%d 受信データ：%s\n", __FILE__, __LINE__, pag_data);

			/*m_BUILDテーブルへデータを挿入する*/
			if(data_len > 0 && data_len <= MAX_DATA_LEN) {

				/* ヘッダーをわける */
				memset(head_buff, 0, sizeof(head_buff));
				if((c_p = strchr(pag_data, ',')) != NULL){
					strncpy(head_buff, pag_data, c_p - pag_data);

					printLog(DEBUGING, "%s:%d ヘッダー : %s\n", __FILE__, __LINE__, head_buff);

					/* ヘッダーにより関数別によびだす */
					if( ! strcmp(head_buff, "BUILD_Insert")){
						ret = build_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "build_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "build_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "build_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "BUILD_Update")){
						ret = build_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "build_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "build_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "build_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "BUILD_Delete")){
						ret = build_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "build_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "build_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "BRANCH_Insert")){
						ret = branch_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "branch_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "branch_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "branch_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "BRANCH_Update")){
						ret = branch_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "branch_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "branch_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "branch_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "BRANCH_Delete")){
						ret = branch_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "branch_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "branch_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_CONTACT_Insert")){
						ret = isp_contact_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_contact_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_contact_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_contact_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_CONTACT_Update")){
						ret = isp_contact_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_contact_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_contact_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_contact_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_CONTACT_Delete")){
						ret = isp_contact_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "isp_contact_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_contact_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_BRANCH_Insert")){
						ret = isp_branch_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_branch_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_branch_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_branch_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_BRANCH_Update")){
						ret = isp_branch_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_branch_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_branch_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_branch_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_BRANCH_Delete")){
						ret = isp_branch_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "isp_branch_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_branch_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_NTE_Insert")){
						ret = isp_nte_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_nte_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_nte_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_nte_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_NTE_Update")){
						ret = isp_nte_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "isp_nte_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "isp_nte_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_nte_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "ISP_NTE_Delete")){
						ret = isp_nte_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "isp_nte_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "isp_nte_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "MEDO_CONTACT_Insert")){
						ret = medo_contact_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "medo_contact_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "medo_contact_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "medo_contact_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "MEDO_CONTACT_Update")){
						ret = medo_contact_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "medo_contact_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "medo_contact_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "medo_contact_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "MEDO_CONTACT_Delete")){
						ret = medo_contact_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "medo_contact_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "medo_contact_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "MAKER_CONTACT_Insert")){
						ret = maker_contact_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "maker_contact_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "maker_contact_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "maker_contact_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "MAKER_CONTACT_Update")){
						ret = maker_contact_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "maker_contact_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "maker_contact_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "maker_contact_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "MAKER_CONTACT_Delete")){
						ret = maker_contact_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "maker_contact_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "maker_contact_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "EQP_Insert")){
						ret = eqp_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "eqp_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "eqp_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "eqp_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "EQP_Update")){
						ret = eqp_update(c_p+1);
						if(ret == 2){
							printLog(WARNING, "eqp_update:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "eqp_update:UPDATE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "eqp_update:UPDATE...OK");
						}
					} else if( ! strcmp(head_buff, "EQP_Delete")){
						ret = eqp_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "eqp_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "eqp_delete:DELETE...OK");
						}
					} else if( ! strcmp(head_buff, "COMPONENT_Insert")){
						ret = component_insert(c_p+1);
						if(ret == 2){
							printLog(WARNING, "component_insert:invalid data");
						} else if(ret != 0){
							printLog(WARNING, "component_insert:INSERT...ORA%d", ret);
						} else {
							printLog(DEBUGING, "component_insert:INSERT...OK");
						}
					} else if( ! strcmp(head_buff, "COMPONENT_Delete")){
						ret = component_delete(c_p+1);
						if(ret != 0){
							printLog(WARNING, "component_delete:DELETE...ORA%d", ret);
						} else {
							printLog(DEBUGING, "component_delete:DELETE...OK");
						}
					} else {
						printLog(WARNING, "P-Agentから受信したデータエラー");
					}
				} else {
					printLog(WARNING, "P-Agentから受信したデータエラー");
				}
			}
		}
	} /* while */

	/* G/W プロセス終了処理 */
	gwp_term();

	exit(1);

} /* main */



/****************************************************************************/
/* Name : gwp_getarg                                                        */
/*                                                                          */
/* Parameters : int		argc       ：IN  main argc                          */
/*              char	**argv     ：IN  main argv                          */
/*              GWP_INF	*gwp_inf_p ：OUT G/W プロセス制御情報テーブル       */
/*                                                                          */
/* Return : 0 ：正常終了                                                    */
/*          -1：異常終了                                                    */
/*                                                                          */
/* Description : 起動パラメータ獲得                                         */
/*               main() の引数から G/W プロセス制御情報テーブルの以下の項目 */
/*               に値を設定する。不正な引数の場合は異常終了する。           */
/*              ・Docomo環境ファイルパス名                                  */
/*              ・P-Agent ホスト名                                          */
/*              ・P-Agent ポート番号                                        */
/*              ・自 識別名                                                 */
/*              ・DB ユーザー名                                             */
/*              ・DB SID                                                    */
/*              ・パスワード                                                */
/*              ・pipeパス名（省略可）                                      */
/*                                                                          */
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
static int gwp_getarg(int argc, char **argv, GWP_INF *gwpinf_p )
{
	int		dbg_cnt;

	printLog( DEBUGING, "%s:%d(%d) MAIN の引数の数", __FILE__, __LINE__, dsp_pid);
	for(dbg_cnt = 0; dbg_cnt < argc; dbg_cnt++)
		printLog(DEBUGING, "\t[%d]:%s",dbg_cnt,*(argv+dbg_cnt));

	/* パラメータ数確認 */
	if( argc < 7 ){
		printLog( ERROR, "%s:%d(%d) MAIN の引数の数(%d)が規定数()でない",
										__FILE__, __LINE__, dsp_pid, argc);
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

	printLog( DEBUGING, "%s:%d 環境ファイル名     :%s",__FILE__,__LINE__,
													gwpinf_p->dcmenvpath );
	printLog( DEBUGING, "%s:%d P-Agent            :%s %d",__FILE__,__LINE__,
							gwpinf_p->pagent_host, gwpinf_p->pagent_port );
	printLog( DEBUGING, "%s:%d 自 識別名          :%s",__FILE__,__LINE__,
													gwpinf_p->owneqpname );
	printLog( DEBUGING, "%s:%d ユーザ名           :%s",__FILE__,__LINE__,
													gwpinf_p->usr_name );
	printLog( DEBUGING, "%s:%d パスワード         :%s",__FILE__,__LINE__,
														gwpinf_p->passwd );
	/* SID */
	if( argc > 7 ){
		strcpy( gwpinf_p->sid, *(argv+7) );
		printLog( DEBUGING, "%s:%d SID         :%s",__FILE__,__LINE__,
														gwpinf_p->sid );
	}

	/* 正常終了 */
	return 0;

} /* gwp_getarg */



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
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
static void gwp_term()
{

	/* DBとの切断 */
	DbDisconnect();
	
	printLog(WARNING, "basic データベース切断");

	/* Pagent TCP/IPソケットを閉じる */
	tcp_close( &(gwp_inf.tcpinf_pagent) );

	/* P-Agent I/F 終了処理 */
	dbp_pagif_term();

} /* gwp_term */



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
/* Create : 2000.12.01                                                      */
/* modify :                                                                 */
/****************************************************************************/
static void gwp_sigterm( int sig_num )
{
	printLog(DEBUGING, "%s:%d(%d) gwp_sigterm(%d)",__FILE__,__LINE__,dsp_pid, sig_num);

	if( sig_num != SIGTERM )
		printLog( WARNING, "%s %d(%d):不正シグナル(%d)受信", __FILE__, __LINE__,
															dsp_pid, sig_num);
	else
		printLog( DEBUGING, "%s %d(%d):終了シグナル(%d)受信", __FILE__, __LINE__,
															dsp_pid, sig_num);

	/* G/W プロセス終了処理 */
	gwp_term();

	/* プロセスの終了値を返す */
	if( sig_num == SIGTERM )
		/* プロセス正常終了 */
		exit(0);
	else
		/* プロセス異常終了 */
		exit(1);
} /* gwp_sigterm */
/* End of File */

