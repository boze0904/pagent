/****************************************************************************/
/* file Name  dbp_port.cc                                                   */
/*                                                                          */
/* Functions:																*/
/*	 Exported:																*/
/*		openPort															*/
/*		closePort															*/
/*		readPort															*/
/*		writePort															*/
/*		breakPort															*/
/*		getDeviceName														*/
/*		getPortName															*/
/*		toAscii																*/
/*		fromAscii															*/
/*		parseArgs															*/
/*																			*/
/* Description:																*/
/*	 This module has all of the functions necessary to send commands to		*/
/*	 a tcp/ip port.															*/
/*                                                                          */
/* Create : 2000.12.15(dcm_pot.cからの流用)                                 */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/

#include <sys/types.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <errno.h>
#include <sys/stat.h>
#include <ctype.h>
#include <string.h>
#include <memory.h>
#include <limits.h>
#include "dbp_port.h"
#include "gw_if.h"
#include "pagent_trace.h"
#include "dbp_portsub.h"
#include "db_pagent.h"
#include "output_log.h"
#include "read_prop_file.h"

#define	OSI_HOME	"OSI_HOME"


/****************************************************************************/
/* Static Global Variables */
static char device[41];				/* デバイス名                        */
DBP_INF		dbp_inf;				/* NTT IP DB P-Agent 制御情報        */
int	port_fd;						/* 読み込み対象fd:processInput() I/F */

/****************************************************************************/
/* Functions */
int	writePort_2(char*,unsigned int);
void	closePort();
void	breakPort();
char	*getDeviceName();
char	*getPortName();
void	checkIPMH();
void	dummy_term(int);

/*** ライブラリ関数 ***/
extern void setFifoReaders(fd_set*);
extern void setSocketReaders(fd_set*);


/****************************************************************************/
/* Name:																	*/
/*   openPort                                                               */
/*																			*/
/* Parameters:																*/
/*   char	*host_config: ホスト名△コンフィグファイル名					*/
/*   char	*sspeed		: baud rate to be used			NO USED				*/
/*   int	tandem		: set XON/XOFF flow control if > 0					*/
/*																			*/
/* Return:																	*/
/*   file descriptor, otherwise -1											*/
/*																			*/
/* Description:																*/
/*   It opens the port in read/write mode and sets the characteristics.		*/
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int openPort( char *host_config, char *sspeed, int tandem )
{
	int		i;								/* ワーク               */
	int		ret;
	int		lvl;							/* ログレベル           */
	char	*c_p;							/* ワーク               */
	char	*lpath;							/* ログパス             */
	char	*home;							/* OSI_HOME パス        */
	char	configfile[DCM_MXCNFLEN+1];		/* コンフィグファイル名 */
	char	logfile[512];


	sighold( SIGUSR2 );


	/* 引数からホスト名とコンフィグファイル名を獲得 */
	sscanf( host_config, "%s %s ", dbp_inf.hostname, configfile );
	/* しかもＥＯＬをここで決定！ */
	dbp_inf.eol[0] = DCM_EOLCODE;
	dbp_inf.eol[1] = 0x00;
	dbp_inf.eol_len = 1;

	/* コンフィグファイル名	（フルパス）獲得 */
	c_p = getenv( DBP_ENVPATH );
	if( c_p == NULL ){
		/* エラーメッセージ表示 */
		printf( "%s:%d 環境変数(%s)未設定\n", __FILE__, __LINE__, DBP_ENVPATH );
		fflush( stdout );
		/* 異常終了 */
		return -1;
	}
	sprintf( dbp_inf.configfile, "%s/%s", c_p, configfile );

	/* 存在確認 */
	ret = access( dbp_inf.configfile, R_OK );
	if( ret ){
		/* エラーメッセージ表示 */
		printf( "%s:%d コンフィグファイル access(%s) error:%d\n",
								__FILE__,__LINE__, dbp_inf.configfile, errno );
		fflush( stdout );
		/* 異常終了 */
		return -1;
	}
#ifdef DEBUG
printf( "%s:%d コンフィグファイル access(%s)\n",
                                __FILE__,__LINE__, dbp_inf.configfile );
#endif
/********** 追加 ここから V01 **********/
	/*** ログ出力の準備 ***/
	/* 設定ファイルをオープン */
	ret = openPropFile( dbp_inf.configfile );
	if( ret ){
		/* エラーメッセージ表示 */
		printf( "%s:%d コンフィグファイル openPropFile(%s) error:%d\n",
								__FILE__, __LINE__, dbp_inf.configfile, errno );
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

	if( lpath == NULL ){
		home = getenv( OSI_HOME );
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


	/*	他からのデータ入力なしに readPort( )にイベントが起こすため、FIFO を	*/
	/*	作成してデータを書き込み、その記述子を main( ) に返却する。			*/
	/*	readPort( ) で最初の呼び出しを判断するために、TEMP FIFO ファイル記	*/
	/*	述子の無効を確認している。											*/

	/* TMP FIFOファイル作成 */
	c_p = (char*)tempnam( "/tmp", "dbp" );
	strcpy( dbp_inf.tmpfifo, c_p );
	free( c_p );	/* tempnam() 使用後はfree()する */

	/* FIFOファイル作成 */
	i = mkfifo( dbp_inf.tmpfifo, 0666 );
	if( i == -1 ){
		/* エラーメッセージ表示 */
		printLog( ERROR, "%s:%d -1 = mkfifo(%s, 0666) errno:%d",__FILE__,__LINE__,
														dbp_inf.tmpfifo, errno);
		return -1;
	}

	/* FIFOをOPEN し、"TEMP FIFO ファイル記述子" を有効にする */
	dbp_inf.tmpfifo_fd = open( dbp_inf.tmpfifo, O_NDELAY | O_RDWR, 0666 );
	if( dbp_inf.tmpfifo_fd == -1 ){
		/* エラーメッセージ表示 */
		printLog( ERROR, "%s:%d -1 = open(%s, O_NDELAY | O_RDWR, 0666) errno:%d",
								__FILE__,__LINE__,	dbp_inf.tmpfifo, errno);

		/* "TEMP FIFO ファイル記述子" を無効にする */
		dbp_inf.tmpfifo_fd = 0;

		return -1;
	}

	/* FIFOファイルに、適当なデータを入力 */
	i = write( dbp_inf.tmpfifo_fd, "dbp", 3 );
	if ( i != 3 ){
		/* エラーメッセージ表示 */
		printLog( ERROR, "%s:%d %d = write(%d, \"dbp\", 3 ) errno:%d",
						__FILE__, __LINE__,	i, dbp_inf.tmpfifo_fd, errno );
		return -1;
	}

	/* シグナル無視:SIGCHLD	(子供の死に様に興味はない) */
	signal( SIGCHLD, SIG_IGN );

	signal( SIGSEGV, dummy_term );

	/* デバイス名初期化 */
	device[0] = 0x00;

	/* TEMP FIFO ファイル記述子" を返却する */
	return dbp_inf.tmpfifo_fd;

	sigrelse( SIGUSR2 );

	printLog( DEBUGING, "%s:%d openPort(%d) 正常終了", 
									__FILE__, __LINE__, dbp_inf.tmpfifo_fd );

} /* openPort */



/****************************************************************************/
/* Name:                                                                    */
/*   closePort                                                              */
/*																			*/
/* Parameters:																*/
/*	 NONE																	*/
/*																			*/
/* Return:																	*/
/*	 NONE																	*/
/*																			*/
/* Description:																*/
/*	Closes the ttyport and sets the port to -1 as an indication.			*/
/*	TMP FIFO が有効な場合後処理をする。										*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
void closePort()
{
	printLog( DEBUGING, "%s:%d closePort() call", __FILE__, __LINE__ ); /* V01 */

	/* TEMP FIFO ファイルが開いている場合 */
	if( dbp_inf.tmpfifo_fd != 0 ){
		/* TEMP FIFO ファイル を閉じる */
		close( dbp_inf.tmpfifo_fd );
	}
	/* TEMP FIFO ファイル有効の場合 */
	if( dbp_inf.tmpfifo[0] != 0x00 ){
		/* TEMP FIFO ファイル削除 */
		unlink( dbp_inf.tmpfifo );
	}

	/* Ｇ／Ｗ終了 */
	gw_kill();

	/* NTT IP DB P-Agent G/W サブ処理 終了処理 */
	dbpsub_term();

} /* closePort */



/****************************************************************************/
/* Name:                                                                    */
/*   writePort                                                              */
/*																			*/
/* Parameters:																*/
/*   char	*data_buff: NetExpertからの入力データ                           */
/*	 int	data_len  : length of data if set                               */
/*																			*/
/* Return:																	*/
/*   number of characters actually written							    	*/
/*																			*/
/* Description:																*/
/*   NetExpert からのメッセージを処理します                                 */
/*   Writes the contents of buff to the ttyport.							*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int writePort( char *data_buff, unsigned int data_len )
{
	static char sbuf[DCM_MXSNDLEN+1] ;				/* SAVEバッファ			*/
	static int	slen = 0;						 	/* SAVEバッファ長		*/
	int			i,i2;								/* ワーク 				*/
	int			cnt;								/* カウンタ 			*/
	char		*c_p, *c_p2;						/* ワーク				*/
	char		c_buff[DCM_MXSNDLEN+DCM_MXEOLLEN+sizeof(DCM_TRACE_DATA_TERM)+1];
													/* ワーク				*/
	printLog( DEBUGING, "*** writePort() ***" );
	
	printDump( data_buff );

	c_p2 = data_buff;
	while( 1 ){
		/* 残りデータ長を求める	 -> i */
		i = (int)( (unsigned int)c_p2 - (unsigned int)data_buff );
		i = data_len - i;
		printLog( DEBUGING, "%s:%d 残データサイズ %d byte", __FILE__, __LINE__, i );	/* V01 */
		/* EOL検索 */
		c_p = strstr( c_p2, dbp_inf.eol );

		/* EOLなし */
		if( c_p == NULL ){
			printLog( DEBUGING, "%s:%d (writePort) EOL みつからない", __FILE__, __LINE__ );	/* V01 */
			/* 保存データ長確認 */
			if( (slen + i) > sizeof(sbuf) ){
				printLog( DEBUGING, "%s:%d 保存データ長:(%d + NEW:%d) > 保存バッファ長:%d",
							__FILE__, __LINE__, slen, i, sizeof(sbuf) );	/* V01 */
				/* SAVEデータクリア */
				memset( sbuf, 0, sizeof(sbuf) );
				slen = 0;
				return -1;
			}

			/* SAVEバッファにデータを保存 */
			strncpy( &sbuf[slen], c_p2, i );		/* 追加         */
			slen += i;								/* 追加分の加算 */
			printLog( DEBUGING, "%s:%d (writePort) SAVEバッファにデータを保存 slen:%d data_len:%d", __FILE__, __LINE__, slen, data_len );	/* V01 */
			/* 次のデータに今回のデータの続きがある・・・・ */
			return data_len;
		}

		/* string関数を使用するため文字列可する。 */
		*c_p = 0;

		/* 今回データ長の確認 */
		i = strlen(c_p2) + strlen(sbuf);
		printLog( DEBUGING, "%s:%d 最大送信データ長    :%d byte", __FILE__, __LINE__, DCM_MXSNDLEN );	/* V01 */
		printLog( DEBUGING, "%s:%d 今回処理するデータ長:%d byte", __FILE__, __LINE__, i );	/* V01 */
		if( sizeof(c_buff) < (i+DCM_MXEOLLEN) ){ /* バッファより大きい	*/
			printLog( DEBUGING, "%s:%d ワークサイズ:%d byte 今回データ廃棄", 
											__FILE__, __LINE__, sizeof(c_buff) );	/* V01 */
			/* SAVEデータクリア */
			memset( sbuf, 0, sizeof(sbuf) );
			slen = 0;
		}
		else{	/* 作業バッファに収まる */
			/* 作業用バッファに保存データをコピー */
			strcpy( c_buff, sbuf );

			/* 今回データを追加（合成データと呼称） */
			strcat( c_buff, c_p2 );

			/* SAVEデータクリア */
			memset( sbuf, 0, sizeof(sbuf) );
			slen = 0;

			/* 合成データ長を得る */
			i2 = strlen( c_buff );

			/* メッセージデータの処理 */
			i =  writePort_2( c_buff, i2 );
			printLog( DEBUGING, "%s:%d %d = writePort_2(, %d)", __FILE__, __LINE__, i, i2 );	/* V01 */

			/* ？ */
			usleep( 10 );
		}

		/* 今回データの EOL の次（MIN からの２つめのデータの先頭）を */
		/* ポイントする。                                            */
		c_p2 = c_p + dbp_inf.eol_len;

		printLog( DEBUGING, "%s:%d(writePort) c_p2:%d - data_buff:%d == data_len:%d",
							__FILE__, __LINE__, c_p2, data_buff, data_len );	/* V01 */

		/* 今回データからポートに出力した分が今回データ長と同じ場合、		*/
		/* つまり、いま扱ったデータの最後が今回データの最後なら終了する		*/
		if( (int)(c_p2 - data_buff) == data_len )
			break;
	}
	return data_len ;

} /* writePort */



/****************************************************************************/
/* Name:                                                                    */
/*   writePort_2                                                            */
/*																			*/
/* Parameters:																*/
/*   char	*data_buff: NetExpertからの入力データ                           */
/*	 int	data_len  : length of data if set                               */
/*																			*/
/* Return:																	*/
/*      -1: 終了コマンド受信                                                */
/*   other: number of characters actually written                           */
/*																			*/
/* Description:																*/
/*   Writes the contents of buff to the ttyport.							*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int	writePort_2( char *data_buff, unsigned int data_len )
{
	int		i;	/* ワーク */


	printLog( DEBUGING, "*** writePort_2() ***" );

	/**** NetExpert からのメッセージの解析 ****/
	/*** P-Agent終了コマンド ***/
	if( !strncmp(data_buff, DBP_TERM, strlen(DBP_TERM)) ){
		/* 終了コマンドをG/Wに送信する */
		return -1;
	}
	/*** DBP P-Agent制御コマンド以外（G/W コマンド）***/
	else {
		/* NTT IP DB P-Agent SUB NetExpert -> G/W */
		dbpsub_netx_gw( data_buff, data_len );
	}

	/* 絶対正常終了 */
	return data_len;

} /* writePort_2 */



/****************************************************************************/
/* Name:																	*/
/*   readPort                                                               */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*data_buff: G/Wからの入力データ                                 */
/*   int	len       : maximun len of data to read                         */
/*																			*/
/* Return:																	*/
/*   number of characters actually read									    */
/*																			*/
/* Description:																*/
/*   Writes the contents of buff to the ttyport.							*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
int readPort( char *data_buff, unsigned int data_len )
{
	fd_set		readers;			/* ファイルディスクリプタ  */
	int			rcv_len=0;			/* 受信データ長            */
	int			rc;					/* select() リターンコード */
	int 		cnt;				/* カウンタ                */
	int 		i;					/* ワーク                  */
	char		c_buff[5];			/* ワーク                  */
	char		*c_p;				/* ワーク                  */

	/****************************/
	/*	SIGUSR2をホールドする	*/
	sighold( SIGUSR2 );

	printLog( DEBUGING, "%s:%d readPort(buff,len:%d)", __FILE__, __LINE__, data_len );	/* V01 */
	memset( c_buff, 0, sizeof(c_buff) );

	/* TMP FIFO の有無を確認する */
	if( dbp_inf.tmpfifo_fd != 0 ){	/* 初期呼び出し */
		/* TEMP FIFOファイル消去 */
		read( dbp_inf.tmpfifo_fd, c_buff, 3 );
		close( dbp_inf.tmpfifo_fd );
		unlink( dbp_inf.tmpfifo );
		/* TEMP FILE 情報無効化 */
		dbp_inf.tmpfifo_fd = 0;
		dbp_inf.tmpfifo[0] = 0x00;

		/* Ｇ／Ｗプロセス起動 */
		i = gw_exec( dbp_inf.configfile );
		if( i == -1 ){
			sigrelse(SIGUSR2);
			/* -1 で return しても死なないので自分で SIGTERM 送信*/
			kill(getpid( ), SIGTERM);
			pause( );
		}

		/* NTT IP DB P-Agent G/W サブ処理 初期処理 */
		i = dbpsub_init();
		if( i == -1 ){
			printLog( DEBUGING, "%s:%d -1 = dbpsub_init()", __FILE__, __LINE__ );	/* V01 */
			/* 異常終了 */
			sigrelse(SIGUSR2);
			/* -1 で return しても死なないので自分で SIGTERM 送信 */
			kill(getpid( ), SIGTERM);
			pause( );
		}

		/* 無制限一本ループ */
		while( 1 ){
			/* ｆｄ設定 */
			FD_ZERO( &readers );
			setFifoReaders( &readers );
			setSocketReaders( &readers );
			gw_fdset( &readers );
			printLog( DETAIL, "%s:%d call select()", __FILE__, __LINE__ );	/* V01 */

			sigrelse( SIGUSR2 );
			rc = select( FD_SETSIZE, &readers, (fd_set *)0, (fd_set *)0,
													(struct timeval *)0 );
			/* 入力ふりわけ */
			if( rc == -1 ) {
				checkIPMH();
			} else {
				sighold( SIGUSR2 );
				processInput( rc, &readers, errno );
			}
		} /* 無限ループ */
	}


	/* 以降、ＧＷプロセスからデータを読み込む処理                   */
	/* readPort( )のパラメータは、バッファとバッファ長 固定なので、 */
	/* port_fd 外部変数を I/F にする。                              */
	/* データ長読み込み（４バイト）                                 */
	for( cnt = 0, i = 0, c_p = (char *)&rcv_len; cnt < 4 ; cnt += i ){
		i = read( port_fd, (c_p+cnt), 4 - cnt );
		printLog( DEBUGING, "%s:%d %d = read( %d, (c_p+%d), %d)",
							__FILE__, __LINE__, i, port_fd, cnt, 4 - cnt );	/* V01 */
		if( i <= 0 ){
			printLog( DEBUGING, "%s:%d %d = read( %d, (c_p+%d), %d) errno:%d",
								__FILE__, __LINE__, i, port_fd, cnt, 4 - cnt, errno );	/* V01 */
			/* 異常終了 */
			return -1;
		}
	} /* for */

	printLog( DEBUGING, "%s:%d データ長獲得 %d", __FILE__, __LINE__, rcv_len );	/* V01 */

	/* データ長チェック */
	if( rcv_len > (int)data_len ){
		printLog( DEBUGING, "%s:%d データ長 %d > バッファ長 %d",
							 __FILE__, __LINE__, rcv_len, data_len );	/* V01 */
		return 0;
	}

	/* データ読み込み */
	for( cnt = 0, i = 0; cnt < rcv_len; cnt += i ){
		i = read( port_fd, (data_buff+cnt), rcv_len - cnt );
		if( i <= 0 ){
			printLog( ERROR, "%s:%d %d = read( %d, (c_p+%d), %d) errno:%d",
					__FILE__, __LINE__, i, port_fd, cnt, rcv_len - cnt, errno );	/* V01 */
			/* 異常終了 */
			return -1;
		}
	} /* for */

	printLog( DEBUGING, "recv_data = %s", data_buff );	/* V01 */

	/* NTT IP DB P-Agent SUB G/W -> NetExpert        */
	/* NetExpertデータの場合は、受信データ長を返し、 */
	/* 他の G/W データの場合は、0 を返す。           */
	i = dbpsub_gw_netx( data_buff, rcv_len );

	/* NetExpert へのデータ長 */
	return i;

} /* readPort */



/****************************************************************************/
/* Name:                                                                    */
/*   breakPort                                                              */
/*                                                                          */
/* Parameters:                                                              */
/*   NONE                                                                   */
/*                                                                          */
/* Return:                                                                  */
/*   NONE                                                                   */
/*                                                                          */
/* Description:                                                             */
/*   Sends a break to the ttyport.                                          */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
void breakPort()
{
	 return;
}



/****************************************************************************/
/* Name:                                                                    */
/*   getDeviceName                                                          */
/*                                                                          */
/* Parameters:                                                              */
/*   char *port: device name                                                */
/*                                                                          */
/* Return:                                                                  */
/*   char *device upon success, otherwiae NULL                              */
/*                                                                          */
/* Description:                                                             */
/*   Returns a pointer to the absolute path of the port name given.	        */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
char *getDeviceName( char *port )
{
	printLog( DEBUGING, "%s:%d getDeviceName(port:%s device:%s)", 
								__FILE__, __LINE__, port, device );	/* V01 */

	if( *device )
		return device;

	if( !port ){		/* requestor only, can't set the device name */
		return( (char *) 0 );
	 }

	strcpy( device, port );

	printLog( DEBUGING, "%s:%d device:%s", __FILE__, __LINE__, device );	/* V01 */

	return device;
}



/****************************************************************************/
/* Name:                                                                    */
/*   getPortName                                                            */
/*                                                                          */
/* Parameters:                                                              */
/*   char *port: IN	ホスト名△コンフィグファイル名	（△：スペース）        */
/*                                                                          */
/* Return:                                                                  */
/*   char *hostconfig: "ホスト名＿コンフィグファイル名"                     */
/*                                                                          */
/* Description:                                                             */
/*   引数の port から、"hostname_confgifile" を作成する。                   */
/*   P-Agent 制御情報の以下の項目を設定する。                               */
/*      自ホスト名                                                          */
/*      コンフィグファイル名                                                */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify : Jul.09.2001: V01: ログ出力の変更                                */
/****************************************************************************/
char *getPortName( char *port )
{
	static char	host_config[DCM_MXHSTLEN+1+DCM_MXCNFLEN+1] = {0x00};
							/* ホスト名＿コンフィグファイル名格納領域	*/

	char		configfile[DCM_MXCNFLEN+1];	/* コンフィグファイル名		*/
	char		hostname[DCM_MXHSTLEN+1];	/* ホスト名					*/


	sighold( SIGUSR2 );

	printLog( DEBUGING, "%s:%d getPortName(port:%s)", __FILE__, __LINE__, port );	/* V01 */

	/* 初回呼び出しを判断する */
	if( host_config[0] == 0x00 ) {	/* 起動時一回のみ 真 */
		/* 引数からホスト名とコンフィグファイル名を獲得 */
		sscanf( port, "%s %s ",hostname, configfile );
		/* 更に "ホスト名＿コンフィグファイル名" を作成 */
		sprintf( host_config, "%s_%s", hostname, configfile);
		printLog( DEBUGING, "%s:%d %s = getPortName()",__FILE__,__LINE__,host_config );	/* V01 */
	}

	/* "ホスト名＿コンフィグファイル名" を返す */
	sigrelse( SIGUSR2 );
	return host_config;
} /* getPortName */



/****************************************************************************/
/* Name:                                                                    */
/*   parseArgs                                                              */
/*                                                                          */
/* Parameters:                                                              */
/*   char *agrs: pointer to list of additional program arguments            */
/*                                                                          */
/* Return:                                                                  */
/*   0 on success, otherwise -1                                             */
/*                                                                          */
/* Description:                                                             */
/*   This function is intended any additional arguments need for the        */
/*   interface to a device greater than the number normally sent to the     */
/*   p_agent by the parent.                                                 */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int parseArgs( char *args )
{
	 if ( !args || !*args )
		return -1;

	 return 0;
}



/****************************************************************************/
/* Name:                                                                    */
/*   toAscii （未使用）                                                     */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*source  : pointer to input data                                */
/*   int	sourcelen: length of souce bytes                                */
/*   char	*dest    : pointer to output data                               */
/*   int	destlen  : maximun bytes possible in dest                       */
/*                                                                          */
/* Return:                                                                  */
/*   number of source bytes translated                                      */
/*                                                                          */
/* Description:                                                             */
/*   This module translates source data into ascii equivalents in dest.     */
/*   For the serial port we will use the toascii conversion function.       */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int toAscii( char *source, int srclen, char *dest, int destlen )
{
	 register int i = 0;
	 register char *src, *dst;

	 src = source, dst = dest;

	 while ( i < srclen && i < destlen ) {
		*dst++ = *src++;
		++i;
	 }

	 return i;
}



/****************************************************************************/
/* Name:                                                                    */
/*   fromAscii（未使用）                                                    */
/*                                                                          */
/* Parameters:                                                              */
/*   char	*source  : pointer to input data                                */
/*   int	sourcelen: length of souce bytes                                */
/*   char	*dest    : pointer to output data                               */
/*   int	*destlen : value-result parameter                               */
/*   input - maximun bytes possible in dest                                 */
/*   output - number of characters put into dest                            */
/*                                                                          */
/* Return:                                                                  */
/*   number of source bytes translated                                      */
/*                                                                          */
/* Description:                                                             */
/*   This module translates source data into raw equivalents in dest.       */
/*   For the serial port we will simply put out what we have gotten in.     */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int fromAscii( char *source, int srclen, char *dest, int *destlen )
{
	if ( srclen > *destlen )
		srclen = *destlen;
	else
		*destlen = srclen;

	memcpy( dest, source, *destlen );

	return srclen;
}


/****************************************************************************/
/* Name:																	*/
/*   dummy_term                                                             */
/*																			*/
/* Parameters:																*/
/*   int	sig_num: シグナル番号                                           */
/*																			*/
/* Return:																	*/
/*   NONE                                                                   */
/*																			*/
/* Description:																*/
/*   signal()関数の引数に登録するためのダミー終了関数                       */
/*   dcm_port.c では、closePort()が登録されていたが、C++では、              */
/*   型チェックが厳しいため、直接closePort(void)を登録できないために作成    */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
void dummy_term( int sig_num )
{
	closePort();
}

/* END OF FILE */
