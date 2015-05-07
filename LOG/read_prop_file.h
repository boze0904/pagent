/****************************************************************/
/* File Name  : read_prop_file.h                                */
/*                                                              */
/* Discription: read_prop_file.c 用のヘッダファイル             */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:                                                      */
/****************************************************************/

#ifndef READ_PROP_FILE_H
#define READ_PROP_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

/*** 外部アクセス関数 ***/
extern int		openPropFile( char* );
extern char*	getProperty( char* );
extern void		closePropFile();

#ifdef __cplusplus
}
#endif

#endif

/* END OF FILE */
