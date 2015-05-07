/****************************************************************************/
/* file name	gw_if.cc                                                    */
/*                                                                          */
/* NTT IP DB P-Agent G/W 生成 I/F管理                                       */
/*                                                                          */
/* Function:                                                                */
/*   gw_accept      : Ｇ／Ｗ接続待ち                                        */
/*   gw_create      : Ｇ／Ｗプロセス生成                                    */
/*   gw_bind        : ソケットアドレス登録                                  */
/*   gw_sigalrm     : SIGALRM 受信関数                                      */
/*   gw_exec        : Ｇ／Ｗプロセス起動                                    */
/*   gw_fdset       : Ｇ／Ｗソケット記述子設定                              */
/*   gw_getvalidfd  : Ｇ／Ｗソケット有効記述子獲得                          */
/*   gw_getgwinfaddr: Ｇ／Ｗ情報テーブルアドレス獲得                        */
/*   gw_kill        : Ｇ／Ｗ終了                                            */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <limits.h>
#include <strings.h>
#include "gw_if.h"
#include "tcp_if.h"
#include "output_log.h"


/****************************************************************************/
/* 変数一覧																	*/
/****************************************************************************/
static GW_INF		gwinf;					/* Ｇ／Ｗ情報テーブル			*/
											/* プロセスＩＤ:0 無効          */
static int			sigalrm_flag = 0;		/* SIGALRM 受信関数	I/F			*/
											/* 0:未使用		1:タイムアウト	*/

/****************************************************************************/
/* Name:                                                                    */
/*   gw_accept: Ｇ／Ｗ接続待ち                                              */
/*                                                                          */
/* Parameters:                                                              */
/*   GW_INF		*gwinf_p   : IN: Ｇ／Ｗ情報テーブル                         */
/*   GW_EXEINF	*gwexeinf_p: IN: Ｇ／Ｗ個別情報テーブル                     */
/*                                                                          */
/* Return:                                                                  */
/*    0: 正常終了                                                           */
/*   -1: 異常終了（致命的）                                                 */
/*   -2: 異常終了（タイムアウト）                                           */
/*                                                                          */
/* Description:                                                             */
/*   listen()、accept()により、G/Wプロセスからの接続待ちを行う。            */
/*   正常終了時、G/W 個別情報のソケット記述子(GW)が有効となる。             */
/*   異常終了時、Ｇ／Ｗ情報 のソケット記述子(P-Agent) は close処理しない    */
/*   ので呼び出し側で後処理を行う。                                         */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
static int gw_accept( GW_INF *gwinf_p, GW_EXEINF *gwexeinf_p )
{
	int		len;					/* ソッケトアドレス名長	*/
	int		i;						/* ワーク				*/


	/* 受付準備	一度 listen( ) すると close() するまでしない。 */
	if( gwinf_p->listen_flag == GW_INFLISTENOFF ){
		i = listen( gwinf_p->sd, 1 );
		if( i == -1 ){
			printLog( ERROR, "%s %d: -1 = listen(%d,1) errno:%d",
									__FILE__, __LINE__, gwinf_p->sd, errno );	/* V01 */
			/* 異常終了（致命的） */
			return -1;
		}
		/* LISTEN FLAG  ON  */
		gwinf_p->listen_flag = GW_INFLISTENON;
	}

	printLog( DEBUGING, "%s %d: %d = listen(%d,1)",
									__FILE__, __LINE__, i, gwinf_p->sd );	/* V01 */

	/* LISTEN タイマーセット */
	if( gwinf_p->pagentlsnt > 0 ){
		/* SIGALRM 受信関数	I/F		未使用 */
		sigalrm_flag = 0;
		alarm(gwinf_p->pagentlsnt);
	}

	/* 接続待ち */
	len = sizeof(struct sockaddr);
	gwexeinf_p->sd=accept(gwinf_p->sd,(struct sockaddr *)&(gwinf_p->name),&len);
	if( gwexeinf_p->sd == -1 ){
		printLog( ERROR, "%s %d: -1 = accept(%d,,) errno:%d\n",__FILE__, __LINE__,
														gwexeinf_p->sd, errno);
		/* タイマー停止（タイムアウト以外の異常の場合に備えている）			*/
		alarm(0);

		/* ソケット記述子(GW) を無効にする。 */
		gwexeinf_p->sd = 0;

		/* タイムアウト判定 */
		if( (sigalrm_flag == 1) && (errno == EINTR) ){
			printLog( ERROR, "%s %d: タイムアウト(%d sec)",
									__FILE__, __LINE__, gwinf_p->pagentlsnt );	/* V01 */
			/* 異常終了（タイムアウト） */
			return -2;
		}
		else {
			/* 異常終了（致命的） */
			return -1;
		}
	}
	/* タイマー停止 */
	alarm(0);

	printLog( DEBUGING, "%s %d: %d = accept(%d,,)",
						__FILE__, __LINE__, gwexeinf_p->sd, gwinf_p->sd );	/* V01 */

	/* 正常終了 */
	return 0;

} /* gw_accept */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_create                                                              */
/*                                                                          */
/*   Ｇ／Ｗプロセス生成                                                     */
/*                                                                          */
/* Parameters:                                                              */
/*   GW_INF		*gwinf_p   : IN: Ｇ／Ｗ情報テーブル                         */
/*   GW_EXEINF	*gwexeinf_p: IN: Ｇ／Ｗ個別情報テーブル                     */
/*                                                                          */
/* Return:                                                                  */
/*    0: 正常終了                                                           */
/*   -1: 異常終了                                                           */
/*                                                                          */
/* Description:                                                             */
/*   Ｇ／Ｗプロセスを生成し、TCP/IP コネクション接続する。                  */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
static int gw_create( GW_INF *gwinf_p, GW_EXEINF *gwexeinf_p )
{
	char		*arg5_p = NULL;				/* 第五引数 */
	char		*arg6_p = NULL;				/* 第六引数 */
	char		*arg7_p = NULL;				/* 第七引数 */
	char		*arg8_p = NULL;				/* 第八引数 */
	char		*arg9_p = NULL;				/* 第九引数 */
	char		*arg10_p = NULL;			/* 第十引数 */
	int			i;							/* ワーク   */


	printLog( DEBUGING, "%s:%d gw_create()", __FILE__, __LINE__ );	/* V01 */

	/* 子プロセス生成 */
	gwexeinf_p->pid = fork();
	if( gwexeinf_p->pid == -1 ){
		printLog( DEBUGING, "%s:%d -1 = fork( ) errno:%d", __FILE__, __LINE__, errno );	/* V01 */
		/* 異常終了 */
		return -1;
	}
	/* 子プロセス */
	if( gwexeinf_p->pid == 0 ){
		/* 第五～十パラメータ設定 */
		if( gwexeinf_p->arg5[0] != '\0' )
			arg5_p = gwexeinf_p->arg5;
		if( gwexeinf_p->arg6[0] != '\0' )
			arg6_p = gwexeinf_p->arg6;
		if( gwexeinf_p->arg7[0] != '\0' )
			arg7_p = gwexeinf_p->arg7;
		if( gwexeinf_p->arg8[0] != '\0' )
			arg8_p = gwexeinf_p->arg8;
		if( gwexeinf_p->arg9[0] != '\0' )
			arg9_p = gwexeinf_p->arg9;
		if( gwexeinf_p->arg10[0] != '\0' )
			arg9_p = gwexeinf_p->arg10;

/********** 変更 ここから V01 **********/
		printLog( DEBUGING, "%s:%d 子プロセス           pid(%d)",__FILE__,__LINE__,(int)getpid() );
		printLog( DEBUGING, "%s:%d 実行形式ファイル     %s", __FILE__,__LINE__,gwexeinf_p->exefile );
		printLog( DETAIL, "%s:%d 環境ファイルパス     %s", __FILE__,__LINE__,gwinf_p->env_path );
		printLog( DETAIL, "%s:%d P-Agent ホスト       %s", __FILE__,__LINE__,gwinf_p->pagenthost );
		printLog( DETAIL, "%s:%d P-Agent ポート       %s", __FILE__,__LINE__,gwinf_p->pagentport );
		if( arg5_p != NULL )
			printLog( DETAIL, "%s:%d 第五引数             %s", __FILE__, __LINE__, arg5_p );
		if( arg6_p != NULL )
			printLog( DETAIL, "%s:%d 第六引数             %s", __FILE__, __LINE__, arg6_p );
		if( arg7_p != NULL )
			printLog( DETAIL, "%s:%d 第七引数             %s", __FILE__, __LINE__, arg7_p );
		if( arg8_p != NULL )
			printLog( DETAIL, "%s:%d 第八引数             %s", __FILE__, __LINE__, arg8_p );
		if( arg9_p != NULL )
			printLog( DETAIL, "%s:%d 第九引数             %s", __FILE__, __LINE__, arg9_p );
		if( arg10_p != NULL )
			printLog( DETAIL, "%s:%d 第十引数             %s", __FILE__, __LINE__, arg10_p );
/********** 変更 ここまで V01 **********/

		/* Ｇ／Ｗプロセスにオーバーレイする。 V0.4 V0.5 */
		usleep((useconds_t)3000);
		i = execl( gwexeinf_p->exefile, gwexeinf_p->exefile, gwinf_p->env_path,
				gwinf_p->pagenthost, gwinf_p->pagentport,
				arg5_p, arg6_p, arg7_p, arg8_p, arg9_p, arg10_p, NULL );
	}

	/*** 以降、親プロセス ***/

	/* 第五引数を識別名として登録。 */
	strcpy( gwexeinf_p->name, gwexeinf_p->arg5 );

	/* Ｇ／Ｗ接続待ち */
	i = gw_accept( gwinf_p, gwexeinf_p );
	if( i != 0 ){
		/* 異常終了 */
		return -1;
	}

	/* 正常終了 */
	return 0;

} /* gw_create */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_bind: ソケットアドレス登録                                          */
/*                                                                          */
/* Parameters:                                                              */
/*   GW_INF		*gwinf_p: I/O: Ｇ／Ｗ情報テーブル                           */
/*                                                                          */
/* Return:                                                                  */
/*    0: 正常終了                                                           */
/*   -1: 異常終了                                                           */
/*                                                                          */
/* Description:                                                             */
/*   ソケットを生成し、アドレス登録（bind）を行う。                         */
/*   正常終了時、Ｇ／Ｗ情報テーブルのソッケトアドレス名と、ソケット記述子   */
/*   (P-Agent)が有効となる。                                                */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
static int gw_bind( GW_INF *gwinf_p )
{
	struct linger	linger_data;			/* manipulating linger option.	*/
	int				optval;					/* オプション設定用				*/
	int				i;						/* ワーク						*/


	printLog( DEBUGING, "%s %d:gw_bind() pagenthost:%s pagentport:%s", 
				__FILE__, __LINE__, gwinf_p->pagenthost, gwinf_p->pagentport ) ;	/* V01 */

	/* インタネットアドレス、ストリームソケット登録 */
	gwinf_p->sd = socket(AF_INET, SOCK_STREAM, 0);
	if( gwinf_p->sd == -1 ) {
		gwinf_p->sd = 0;
		printLog( ERROR, "%s %d:-1 = socket( ) errno:%d", __FILE__, __LINE__, errno );	/* V01 */
		return -1;
	}

	/* ソケット設定（再使用可） */
	optval = 1;
	i = setsockopt( gwinf_p->sd, SOL_SOCKET, SO_REUSEADDR,
		(char *)&optval, sizeof(optval));
	if ( i < 0 ) {
		printLog( ERROR, "%s %d: %d = setsockopt(%d, SOL_SOCKET, SO_REUSEADDR,, ) errno:%d",
									__FILE__, __LINE__, i, gwinf_p->sd, errno );	/* V01 */
		/* ソケットｃｌｏｓｅ */
		close(gwinf_p->sd);
		gwinf_p->sd = 0;
		return -1;
	}

	/* ソケット設定（リンガー：ＯＦＦ リンガー時間：０） */
	i = sizeof(struct linger) ;
	linger_data.l_onoff = 0;
	linger_data.l_linger = 0 ;
	i = setsockopt( gwinf_p->sd, SOL_SOCKET, SO_LINGER, (char *)&linger_data,
													sizeof(struct linger));
	if( i < 0 ){
		printLog( ERROR, "%s %d: %d = setsockopt(%d, SOL_SOCKET, SO_LINGER,, ) errno:%d",
									__FILE__, __LINE__, i, gwinf_p->sd, errno );	/* V01 */
		/* ソケットｃｌｏｓｅ */
		close( gwinf_p->sd );
		gwinf_p->sd = 0;
		return -1;
	}

	/* ソケットに名前を割り当てる */
	memset( &(gwinf_p->name), 0 ,sizeof(struct sockaddr) );
	gwinf_p->name.sin_family = AF_INET;
	gwinf_p->name.sin_addr.s_addr = inet_addr(hostnametoaddr(gwinf_p->pagenthost));
	gwinf_p->name.sin_port = htons(atol(gwinf_p->pagentport)) ;
	i = bind( gwinf_p->sd, (struct sockaddr *)&(gwinf_p->name),
													sizeof(struct sockaddr) );
	if( i == -1 ) {
		printLog( ERROR, "%s %d: -1 = bind(  ) errno:%d", __FILE__, __LINE__, errno );	/* V01 */
		/* ソケットｃｌｏｓｅ */
		close(gwinf_p->sd);
		gwinf_p->sd = 0;
		return -1;
	}

	/* 正常終了 */
	return 0;

} /* gw_bind */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_sigalrm: SIGALRM 受信関数                                           */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   SIGALRM 受信関数 I/F をタイムアウトに設定する。                        */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify :                                                                 */
/****************************************************************************/
static void		gw_sigalrm( int sig_num )
{
	sigalrm_flag = 1;

} /* gw_sigalrm */

/****************************************************************************/
/* Name:                                                                    */
/*   gw_exec: Ｇ／Ｗプロセス起動                                            */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*gw_exec_file: IN: Ｇ／Ｗ起動コンフィグレーションファイル       */
/*                                                                          */
/* Return:                                                                  */
/*   >= 0: 起動Ｇ／Ｗ数                                                     */
/*     -1: 異常終了                                                         */
/*                                                                          */
/* Description:                                                             */
/*   コンフィグレーションファイルを読み込み、Ｇ／Ｗ情報テーブルに格納する。 */
/*   P-Agent	側のポート（サーバ）を開設する。                            */
/*   G/W プロセスを生成し、ホスト名/ポートID でLISTEN タイマー秒間          */
/*   TCP/IP接続待ち                                                         */
/*   をする。接続待ちタイムアウト秒内に接続しないと異常終了する。           */
/*   GW= の指定は最後に行う。                                               */
/*   LISTEN タイマーの初期値は０(永久に待つ)で省略可能。                    */
/*                                                                          */
/*   【コンフィグレーションファイルフォーマット】                           */
/*   +----------------------------------------------------------            */
/*   |PAGENTHOST=P-Agent ホスト名                                           */
/*   |PAGENTPORT=P-Agent ポートID                                           */
/*   |PAGENTLTIM=P-Agent LISTEN タイマー                                    */
/*   |GW_EXEPATH=ＧＷ実行形式ディレクトリ                                   */
/*   |GW=実行形式ファイル arg5 arg6 arg7 arg8 arg9 arg10                    */
/*                                                                          */
/*   生成プロセス(実行形式ファイル)への引数は４番目までが固定で、           */
/*   以下の内容が設定される。                                               */
/*   実行形式固有の値は５番目以降に最大６つ指定できる。                     */
/*   ただし６番目の引数は ＧＷ識別名 とする。                               */
/*   P-Agent は、この識別名によりＧＷへ送信するデータを振り分ける。         */
/*                                                                          */
/*   【固定の引数】                                                         */
/*   exefile_name $DBP_ENV_PATH $PAGENTHOST $PAGENTPORT                     */
/*                                          ※ DBP_ENV_PATH：環境変数       */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int	gw_exec( char *gw_exec_file )
{
    FILE	*stream;					/* コンフィグレーションファイル用 */
	char	line_buff[GW_MXLINELEN+1];	/* 行バッファ                     */
	char	c_buff[GW_MXFILELEN+1];		/* ワーク                         */
	int		cnt;						/* G/W起動カウンタ                */
	int		i;							/* ワーク                         */


	printLog( DEBUGING, "%s:%d gwexec(%s)", __FILE__, __LINE__, gw_exec_file );	/* V01 */

	/* SIGALRM 受信関数登録 */
	signal( SIGALRM, gw_sigalrm );

	/* Ｇ／Ｗ情報テーブル初期化 */
	memset( &gwinf, 0, sizeof(gwinf) );

	/* Ｇ／Ｗ起動コンフィグレーションファイルを開く */
	stream = fopen( gw_exec_file, "r" );
	if( stream == NULL ){
		printLog( DEBUGING, "%s:%d fopen(%s, r)", __FILE__, __LINE__, gw_exec_file );	/* V01 */
		return -1;
	}

	/* ファイル読み込み */
	for( cnt = 0;fgets( line_buff, sizeof(line_buff), stream ) != NULL; ){
		/* 改行 */
		if( line_buff[0] == '\n' )
			continue;

		/* コメント行 */
		if( line_buff[0] == '#' )
			continue;

		/* 行末が改行の場合ヌル終端にする。 */
		i = strlen(line_buff);
		if( line_buff[i-1] == '\n' )
			line_buff[i-1] = 0x00;

		printLog( DEBUGING, "%s:%d %s(%s)", 
							__FILE__, __LINE__, gw_exec_file, line_buff );	/* V01 */

		/* P-Agent ホスト名獲得 */
		if ( !strncmp(line_buff,GW_CNFPAGHST,sizeof(GW_CNFPAGHST)-1) ){
			strcpy( gwinf.pagenthost,&line_buff[sizeof(GW_CNFPAGHST)-1] );
			/* ソケット記述子が無効で、ホスト名／ポートIDが獲得済みの場合 */
			if( gwinf.sd == 0 && gwinf.pagentport[0] != 0x00 ){
				/* ソケットアドレス登録 */
				i = gw_bind( &gwinf );
				if( i == -1 ){
					fclose( stream );
					return -1;
				}
			}
		}
		/* P-Agent ポートID獲得 */
		else if( !strncmp(line_buff,GW_CNFPAGPRT,sizeof(GW_CNFPAGPRT)-1) ){
			strcpy(gwinf.pagentport, &line_buff[sizeof(GW_CNFPAGPRT)-1]);
			/* ソケット記述子が無効で、ホスト名／ポートIDが獲得済みの場合 */
			if( gwinf.sd == 0 && gwinf.pagenthost[0] != 0x00 ){
				/* ソケットアドレス登録 */
				i = gw_bind( &gwinf );
				if( i == -1 ){
					fclose( stream );
					return -1;
				}
			}
		}
		/* P-Agent LISTEN タイマー獲得 */
		else if( !strncmp(line_buff,GW_CNFPAGLTM,sizeof(GW_CNFPAGLTM)-1) ){
			gwinf.pagentlsnt = atoi( &line_buff[sizeof(GW_CNFPAGLTM)-1]);
		}
		/* NTT IP 環境ファイルパス獲得 */
		else if( !strncmp(line_buff,GW_CNFPAGENV,sizeof(GW_CNFPAGENV)-1) ){
			strcpy( gwinf.env_path, &line_buff[sizeof(GW_CNFPAGENV)-1] );
		}
		/* G/W 実行形式ファイルディレクトリ獲得 */
		else if( !strncmp(line_buff,GW_CNFGWEXED,sizeof(GW_CNFGWEXED)-1) ){
			strcpy(gwinf.exe_dir, &line_buff[sizeof(GW_CNFGWEXED)-1]);
			/* 環境ファイルは使ってないので、とりあえず         */
			/* G/W 実行形式ファイルディレクトリと同じにしておく */
			strcpy( gwinf.env_path, &line_buff[sizeof(GW_CNFGWEXED)-1] );
		}
		/* G/W 起動形式 プロセス起動 */
		else if( !strncmp(line_buff,GW_CNFGW,strlen(GW_CNFGW)) ){
			/* 環境設定チェック */
			if( gwinf.pagenthost == NULL || gwinf.pagentport == NULL
				|| gwinf.pagentlsnt == 0 || gwinf.exe_dir == NULL ){
/********** 変更 ここから V01 **********/
				printLog( DEBUGING, "%s:%d 環境設定が不充分です", __FILE__, __LINE__ );
				printLog( DEBUGING, "PagentHost：%s", gwinf.pagenthost );
				printLog( DEBUGING, "PagentPort：%s", gwinf.pagentport );
				printLog( DEBUGING, "ListenTimer：%d", gwinf.pagentlsnt );
				printLog( DEBUGING, "GW DIR：%s", gwinf.exe_dir );
/********** 変更 ここまで V01 **********/
				return -1;
			}
			printLog( DEBUGING, "%s:%d プロセス起動 %s", __FILE__, __LINE__, line_buff );	/* V01 */
			/* Ｇ／Ｗ情報テーブル設定 V0.4 V0.5 */
			sscanf( &line_buff[sizeof(GW_CNFGW)-1],"%s %s %s %s %s %s %s", c_buff,
				gwinf.exeinf[cnt].arg5, gwinf.exeinf[cnt].arg6,
				gwinf.exeinf[cnt].arg7, gwinf.exeinf[cnt].arg8,
				gwinf.exeinf[cnt].arg9, gwinf.exeinf[cnt].arg10 );
			sprintf( gwinf.exeinf[cnt].exefile, "%s/%s", gwinf.exe_dir, c_buff );

			/* Ｇ／Ｗ生成 */
			i = gw_create( &gwinf, &(gwinf.exeinf[cnt]) );
			if( i == -1 ){
				fclose( stream );
				return -1;
			}
			/* 有効 G/W個別情報数、G/W起動カウンタインクリメント */
			gwinf.exeinfsuu++;
			printLog( DEBUGING, "%s:%d G/W起動カウンタ %d", __FILE__, __LINE__, gwinf.exeinfsuu );	/* V01 */
			cnt++;
		}
		/* 無効な行 */
		else{
			printLog( WARNING, "%s:%d file:%s 無効な行:%s", 
										__FILE__, __LINE__, gw_exec_file, line_buff);	/* V01 */
		}
	}/* for */

	/* G/W起動コンフィグレーションファイルを閉じる */
	fclose( stream );

	/* 正常終了 */
	return cnt;
} /* gw_exec() */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_fdset: Ｇ／Ｗソケット記述子設定                                     */
/*																			*/
/* Parameters:                                                              */
/*   fd_set		*readers_p: OUT: 記述子セット                               */
/*                                                                          */
/* Return:																	*/
/*   NONE                                                                   */
/*																			*/
/* Description:                                                             */
/*   Ｇ／Ｗとのソケット記述子を記述子セットに設定する。                     */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void gw_fdset( fd_set *readers_p )
{
	int		cnt;					/* カウンタ */

	printLog( DEBUGING, "%s:%d gw_fdset(%d)", __FILE__,__LINE__, *readers_p );	/* V01 */

	/* 有効 G/W個別情報のソケット記述子(GW)を確認 */
	for ( cnt = 0 ; cnt < gwinf.exeinfsuu; cnt++ ){
		/* ソケット記述子(GW)確認 */
		if( gwinf.exeinf[cnt].sd != 0 ){
			/* 有効の場合、ソケット記述子を追加 */
			FD_SET( gwinf.exeinf[cnt].sd, readers_p );
		}
	}
} /* gw_fdset() */


/****************************************************************************/
/* Name:																	*/
/*	gw_getvalidfd		Ｇ／Ｗソケット有効記述子獲得						*/
/*																			*/
/* Parameters:																*/
/*	fd_set		*readers_p		：IN	記述子セット						*/
/*	int			*fdsuu_p		：OUT	有効記述子数格納領域アドレス		*/
/*																			*/
/* Return:																	*/
/*		ret						：有効記述子格納領域アドレス				*/
/*																			*/
/* Description:																*/
/*	読み込み準備の整ったソケット記述子を返す。								*/
/*	返値の示すアドレスから "有効記述子数" の記述子が有効。					*/
/*	記述子セットは、FD_CLR を行う。											*/
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int	*gw_getvalidfd( fd_set *readers_p, int *fdsuu_p )
{
	static int		fds[GW_MXGWSUU];		/* 有効記述子格納領域			*/
	int				cnt;					/* カウンタ						*/


	printLog( DEBUGING, "%s:%d gw_getvalidfd(%d,%d)",
						__FILE__,__LINE__, *readers_p, *fdsuu_p );	/* V01 */

	/* 有効 G/W個別情報のソケット記述子(GW)を確認							*/
	for ( cnt = 0, *fdsuu_p = 0; cnt < gwinf.exeinfsuu; cnt++ ) {
		/* ソケット記述子(GW)確認											*/
		if( gwinf.exeinf[cnt].sd != 0 ) {
			/* 準備ＯＫか確認する											*/
			if( FD_ISSET( gwinf.exeinf[cnt].sd, readers_p ) ){
				FD_CLR( gwinf.exeinf[cnt].sd, readers_p );
				/* 準備ＯＫなソケット記述子を追加							*/
				fds[(*fdsuu_p)++] = gwinf.exeinf[cnt].sd;
			}
		}
	} /* for */

	/* 有効記述子格納領域アドレスを返す */
	return fds;

} /* gw_getvalidfd() */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_getgwinfaddr                                                        */
/*                                                                          */
/* Parameters:																*/
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   GW_INF *ret: Ｇ／Ｗ情報テーブルアドレス                                */
/*                                                                          */
/* Description:																*/
/*   Ｇ／Ｗ情報テーブルアドレスを得る                                       */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
GW_INF	*gw_getgwinfaddr()
{
	printLog( DEBUGING, "%s:%d gw_getgwinfaddr()", __FILE__,__LINE__ );	/* V01 */

	return &gwinf;

} /* gw_getgwinfaddr() */


/****************************************************************************/
/* Name:                                                                    */
/*   gw_kill: Ｇ／Ｗ終了                                                    */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   Ｇ／Ｗとのコネクションを閉じる。                                       */
/*   Ｇ／Ｗプロセスに終了シグナルを送信する。                               */
/*                                                                          */
/*   ※Ｇ／Ｗ情報テーブルの参照／更新を行う。                               */
/*                                                                          */
/* Create : 2000.12.15                                                      */ 
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void gw_kill()
{
	int				cnt;						/* カウンタ					*/

	printLog( DEBUGING, "%s:%d gw_kill( )", __FILE__, __LINE__ );	/* V01 */

	/* Ｇ／Ｗプロセスのコネクションを閉じる */
	for( cnt = 0; cnt < GW_MXGWSUU; cnt++ ){
		/* ソケット記述子(GW)が有効か判断する */
		if( gwinf.exeinf[cnt].sd != 0 ){
			/* コネクションｃｌｏｓｅ */
			close( gwinf.exeinf[cnt].sd );
			gwinf.exeinf[cnt].sd = 0;
		}
	}

	/* Ｇ／Ｗプロセスに終了シグナルを送信する */
	for( cnt = 0; cnt < GW_MXGWSUU; cnt++ ){
		/* プロセスＩＤが有効か判断する */
		if( gwinf.exeinf[cnt].pid != 0 ){
			/* プロセスの生死を確認 */
			if( kill(gwinf.exeinf[cnt].pid, 0) == 0 ){
				/* 終了シグナル送信 */
				kill( gwinf.exeinf[cnt].pid, SIGTERM );
				printLog( DEBUGING, "%s:%d kill(%d, SIGTERM)", 
										__FILE__, __LINE__, gwinf.exeinf[cnt].pid );	/* V01 */
				gwinf.exeinf[cnt].pid = 0;
			}
		}
	}

	/* 有効 G/W個別情報数リセット */
	gwinf.exeinfsuu = 0;

	/* P-Agent 側のコネクションを閉じる */
	close( gwinf.sd );
	gwinf.sd = 0;
	gwinf.listen_flag = GW_INFLISTENOFF;	 /* LISTEN FLAG=OFF */

} /* gw_kill() */

/* END OF FILE  */
