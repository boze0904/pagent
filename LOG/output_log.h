/****************************************************************/
/* File Name  : output_log.h									*/
/*                                                              */
/* Discription: output_log.c 用のヘッダファイル　　　　　　　 　*/
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/

#ifndef OUTPUT_LOG_H
#define OUTPUT_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*** 外部アクセス関数 ***/
extern void openLog( char*, int );
extern int printLog( int , char*, ... );
extern int printDump( char* );

/*** ログレベル ***/
#define DETAIL		1
#define DEBUGING	2
#define WARNING		3
#define	ERROR		4

/*** ログ用設定ファイルプロパティ名 ***/
#define	LOGPATH	"LOG_PATH"
#define	LOGLVL	"LOG_LEVEL" 

#ifdef __cplusplus
}
#endif

#endif


/*** End Of File ***/

