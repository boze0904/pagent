/****************************************************************/
/* File Name  : output_log.c									*/
/*                                                              */
/* Function   : openLog()										*/
/*              printLog()                                      */
/*              printDump()                                     */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include "output_log.h"

#define LOG_DIR		"/tmp/"		/* デフォルトのログファイルパス */
#define LOG_NAME	"dbpagent"	/* ログファイル名               */
#define LOG_EXT		".log"		/* ログファイル拡張子           */

static int		log_lvl = WARNING;	/* 既定ログレベル */
static char		log_dir[512];		/* ログパス       */


/****************************************************************/
/* Function Name: openLog										*/
/*                                                              */
/* Parameter    : char* fpath: ログ出力用ファイルパス           */
/*                             NULLも可                         */
/*                int   lvl  : ログレベル                       */
/*                                                              */
/* Return       : NONE                                          */
/*                                                              */
/* Discription  : ログの初期化をします                          */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/
void openLog( char* fpath, int lvl )
{
#ifdef DEBUG
printf( "*** openLog() ***\n" );
#endif

	/*** ログパスの設定 ***/
	if( fpath == NULL ){
		strcpy( log_dir, LOG_DIR );
	}
	else{
		/* 指定パス名の最後にスラッシュがある？ */
		if( !strncmp( fpath+strlen(fpath), "/", 1 ) ){
			strcpy( log_dir, fpath );
		}
		else{
			sprintf( log_dir, "%s/", fpath );
		}
	}
	
	/*** ログレベル基準値が定義の範囲内だったら設定 ***/
	if( DETAIL <= lvl || ERROR >= lvl ){
		log_lvl = lvl;
	}

#ifdef DEBUG
printf( "log path %s\n", fpath );
printf( "log level %d\n", log_lvl );
#endif
} /* openLog() */


/****************************************************************/
/* Function Name: printLog										*/
/*                                                              */
/* Parameter    : int   lvl    : ログレベル                     */
/*                char* message: ログ出力フォーマット           */
/*                ...          : 可変パラメータ                 */
/*                                                              */
/* Discription  : ログを標準出力とファイルに出力します          */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/
int printLog( int lvl, char *message, ... )
{
	char		msg[2048];
	char		fname[256];
	char		ful_fname[512];
	char		log_date[10];
	time_t		ct = time( NULL );				/* TIME */
	struct 		tm *st = localtime( &ct );		/* TIME */
	va_list		ap;
	FILE		*fp;

	/*** 既定出力レベル > 指定レベル なら ログ出力しない ***/
	if( log_lvl > lvl ){
		return 0;
	}

	/*** ファイル名 日付を取得 ***/
	memset( log_date, 0, sizeof(log_date) );
	ascftime( log_date, "%Y%m%d", st );

	/*** ファイル名の生成 ***/
	sprintf( fname, "%s%s%s", LOG_NAME, log_date, LOG_EXT );

	/*** ログフルパス名 ***/
	sprintf( ful_fname, "%s%s", log_dir, fname );

	/*** ログメッセージ用 日付/時間を取得 */
	memset( msg, 0, sizeof(msg) );
	ascftime( msg, "%Y-%m-%d %T ", st );

	/*** ログメッセージフォーマットの作成 ***/
	sprintf( msg, "%s%s\n", msg, message );

	/*** ファイルのオープン ***/
	fp = fopen( ful_fname, "a" );
	if( fp == NULL ){
		return -1;
	}

	/*** 可変引数処理開始 ****/
	va_start( ap, message );
	
	/* ファイルへの出力 */
	vfprintf( fp, msg, ap );
	fflush( fp );
#ifdef DEBUG
	/* 標準出力への出力 */
	vprintf( msg, ap );
	fflush( stdout );
#endif

	/* 可変引数処理終了 */
	va_end(ap);

	fclose( fp );

	return 0;
} /* printLog() */


/****************************************************************/
/* Function Name: printDump                                     */
/*                                                              */
/* Parameter    : char* fpath: ログ出力用ファイルパス           */
/*                                                              */
/* Discription  : データのダンプ出力をします                    */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/
int printDump( char *data )
{
	int			i;
	int			data_len;
	char		msg[2048];
	char		fname[256];
	char		ful_fname[512];
	char		log_date[10];
	time_t		ct = time( NULL );				/* TIME */
	struct 		tm *st = localtime( &ct );		/* TIME */
	FILE		*fp;

	/*** 既定出力レベル > DETAIL なら ログ出力しない ***/
	if( log_lvl > DETAIL ){
		return 0;
	}

	data_len = strlen( data );

	/*** ファイル名 日付を取得 ***/
	memset( log_date, 0, sizeof(log_date) );
	ascftime( log_date, "%Y%m%d", st );

	/*** ファイル名の生成 ***/
	sprintf( fname, "%s%s%s", LOG_NAME, log_date, LOG_EXT );

	/*** ログフルパス名 ***/
	sprintf( ful_fname, "%s%s", log_dir, fname );

	/*** ログメッセージ用 日付/時間を取得 */
	memset( msg, 0, sizeof(msg) );
	ascftime( msg, "%Y-%m-%d %T ", st );

	/*** ファイルのオープン ***/
	fp = fopen( ful_fname, "a" );
	if( fp == NULL ){
		return -1;
	}

	/* ファイルへの出力 */
	fprintf( fp, "\n%sdata dump ----------------- start -\n", msg );
	fprintf( fp, "| data_len:%4d byte (0x%04x byte)                    |", data_len, data_len );
	for( i = 0; i < data_len; i++ ){
        if( !(i % 16) ){
			if( i == 0 ){
				fprintf( fp, "\n|%04x:", i );
			}
			else{
				fprintf( fp, "|\n|%04x:", i );
			}
		}
        fprintf( fp, "%02x ", 0x000000ff & *(data+i) );
		fflush( fp );
    }
	for( ; i % 16; i++ ){
		fprintf( fp, "   " );
	}
	fprintf( fp, "|\n%sdata dump ----------------- end   -\n\n", msg );
	fflush( fp );

#ifdef DEBUG
	/* 標準出力への出力 */
	printf( "\n%sdata dump ----------------- start -\n", msg );
	printf( "| data_len:%4d byte (0x%04x byte)                    |", data_len, data_len );
	for( i = 0; i < data_len; i++ ){
        if( !(i % 16) ){
			if( i == 0 ){
				printf( "\n|%04x:", i );
			}
			else{
				printf( "|\n|%04x:", i );
			}
		}
        printf( "%02x ", 0x000000ff & *(data+i) );
		fflush( stdout );
    }
	for( ; i % 16; i++ ){
		printf( "   " );
	}
	printf( "|\n%sdata dump ----------------- end   -\n\n", msg );
	fflush( stdout );
#endif

	fclose( fp );

	return 0;
} /* printDump() */

/*** End Of File ***/

