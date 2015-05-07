/****************************************************************/
/* File Name  : read_prop_file.c                                */
/*                                                              */
/* Function   : openPropFile                                    */
/*                                                              */
/* Discription:                                                 */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:                                                      */
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include "read_prop_file.h"


char**	prop_name = NULL;	/* プロパティの名前 */
char**	prop_val = NULL;	/* プロパティの値   */
static int		cnt;				/* プロパティの個数 */


/****************************************************************/
/* Function Name: openPropFile                                  */
/*                                                              */
/* Parameter    : char* fname: 設定ファイルフルパス名           */
/*                                                              */
/* Return       :  0:正常                                       */ 
/*                -1:異常                                       */
/*                                                              */
/* Discription  : 設定ファイルを読み込みます                    */
/*                Property Name=Value 形式の値をメモリ上に      */
/*                取り込みます                                  */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:                                                      */
/****************************************************************/
int openPropFile( char* fname )
{
	char*	work;
	char	line_buff[1024];	/* 行バッファ */
	int     len;				/* ワーク     */
	FILE	*fp;

#ifdef DEBUG
printf( "*** openPropFile(%s) ***\n", fname );
#endif

	/*** 設定ファイルのオープン ***/
	fp = fopen( fname, "r" );
	if( fp == NULL ){
		return -1;
	}

	/* ファイル読み込み */
	for( cnt = 0; fgets( line_buff, sizeof(line_buff), fp ) != NULL; ){
		/* 改行 */
		if( line_buff[0] == '\n' )
			continue;

		/* コメント行 */
		if( line_buff[0] == '#' )
			continue;

		/* 行末が改行の場合ヌル終端にする。 */
		len = strlen( line_buff );
		if( line_buff[len-1] == '\n' )
			line_buff[len-1] = 0x00;

		len = strlen( line_buff );
		work = (char*)strtok( line_buff, "=" );
		/* Name=Value 形式でない時は、飛ばす */
		if( strlen(work) == len )
			continue;

		/*** プロパティ名の設定 ***/
		prop_name = (char**)realloc( prop_name, (cnt+1) * sizeof(char*) );
		prop_name[cnt] = (char*)malloc( (strlen(work) + 1) );
		memset( prop_name[cnt], 0, sizeof( prop_name[cnt] ) );
		strcpy( prop_name[cnt], work );
			
		work = (char*)strtok( NULL, "=" );

		/*** プロパティの値の設定 ***/
		prop_val = (char**)realloc( prop_val, (cnt+1) * sizeof(char*) );
		prop_val[cnt] = (char*)malloc( (strlen(work) + 1) );
		memset( prop_val[cnt], 0, sizeof( prop_val[cnt] ) );
		strcpy( prop_val[cnt], work );

#ifdef DEBUG
printf( "prop_name size %d\n", sizeof(prop_name) );
printf( "prop_val size %d\n", sizeof(prop_val) );
printf( "prop name %s\n", *(prop_name + cnt) );
printf( "prop val %s\n", *(prop_val + cnt) );
#endif
			
		cnt++;		
	}
		
	fclose( fp );

	return 0;

} /* openPropFile() */


/****************************************************************/
/* Function Name: getProperty                                   */
/*                                                              */
/* Parameter    : char* prop: プロパティ名                      */
/*                                                              */
/* Return       : char* プロパティの値                          */ 
/*                該当のプロパティ名がない場合NULL              */
/*                                                              */
/* Discription  : 指定されたプロパティ名の値を取得します        */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:                                                      */
/****************************************************************/
char* getProperty( char* prop )
{
	int	i;


	if( prop_name == NULL || prop_val == NULL ){
		return NULL;
	}

	for( i = 0; i < cnt; i++ ){
		if( !strcmp( prop, *(prop_name+i) ) ){
			/* 値を返す */
			return *(prop_val + i);
		}
	}
	/* 該当なし */
	return NULL;
}


/****************************************************************/
/* Function Name: closePropFile                                 */
/*                                                              */
/* Parameter    : NONE                                          */
/*                                                              */
/* Return       : NONE                                          */ 
/*                                                              */
/* Discription  : 設定ファイル読み込みの後処理をします          */
/*                openPropFile()を呼び出した後は、メモリ        */
/*                領域開放のため、必ずこの関数を呼び出して      */
/*                下さい                                        */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:                                                      */
/****************************************************************/
void closePropFile()
{
	int	i;	/* loop counter */
 
	if( prop_name != NULL ){	
		for( i = 0; i < cnt; i++ )
			free( prop_name + i );
		free( prop_name );
	}

	if( prop_val != NULL ){
		for( i = 0; i < cnt; i++ )
			free( prop_val + i );
		free( prop_val );
	}

	prop_name = NULL;
	prop_val = NULL;

	cnt = 0;

} /* closeFile() */

/* END OF FILE */

