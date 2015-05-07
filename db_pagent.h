/****************************************************************************/
/* file name  db_pagent.h                                                   */
/*                                                                          */
/*  NTT IP DB P-Agent 用ヘッダ                                              */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef DB_PAGENT_H
#define DB_PAGENT_H

extern void	processInput( int, fd_set*, int );	/* プロセスの入力データ振り分け   */
extern void	msgtomout();						/* メッセ－ジ出力                 */
extern void	illegalmsgout( char*, int );		/* 不正制御コマンドメッセージ出力 */


#endif

/* END OF FILE */
