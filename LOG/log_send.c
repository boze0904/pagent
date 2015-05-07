/****************************************************************/
/*	ファイル名:													*/
/*		log_send.c												*/
/*	関数名:														*/
/*		log_send												*/
/*	パラメータ:													*/
/*		char *apl		: 実行アプリケーション名				*/
/*		char *file		: ディレクトリ/ログファイル				*/
/*		char *message	: メッセージ							*/
/*	戻り値:														*/
/*		0		: 成功											*/
/*		0以外	: 失敗											*/
/*	概要:														*/
/*		ログファイルの場所、及びメッセージを引数に				*/
/*		セットし、コールする									*/
/*		ログフォーマットは以下のものとする						*/
/*		日付 時間 ホスト名 実行アプリケーション メッセージ		*/
/*	作成日:														*/
/*		2001/02/14												*/
/*	作成者:														*/
/*		長島													*/
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

int log_send( char *file, int priority, char *apl, char *message ) {

	FILE		*fp;								/*** ファイルポインタ	*/
	char		host[64];							/*** ホスト変数			*/
	time_t		ct = time( NULL );					/*** TIME				*/
	struct 		tm *st = localtime( &ct );			/*** TIME				*/
	char		time_string[64];					/*** TIME				*/
	
	int			i = -1;								/*** ワーク				*/
	
	/********************************************/
	/*	ファイルのオープン						*/
	if( ( fp = fopen( file, "a" ) ) == NULL ) {
		return( -1 );
	}

	/********************************************/
	/*	ホスト名取得							*/
	memset( host, 0, sizeof( host ) );					/*** host変数クリアー */
	if( i = gethostname( host, 63 ) != 0 ) {
		return( -1 );	
	}

	/********************************************/
	/*	日付/時間を取得							*/
	memset( time_string, 0, sizeof( time_string ) );		/*** time_string変数をクリアー */
	sprintf( time_string, "%004d-%02d-%02d %02d:%02d:%02d", 
							1900+st->tm_year, 1+st-> tm_mon, st->tm_mday,
							st->tm_hour, st->tm_min, st->tm_sec );

	/********************************************/
	/*	ログセット								*/
	fprintf( fp, "%s %d %s %s : %s\n", time_string, priority, host, apl, message);

	fclose( fp );

	return 0;

}

/*** End Of File */

