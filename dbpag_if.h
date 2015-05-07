/****************************************************************************/
/* file name  dbpag_if.h                                                    */
/*                                                                          */
/* G/W と DB P-Agent のデータ送受信関数群ヘッダ                             */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef DBPAG_IF_H
#define DBPAG_IF_H

extern int		dbp_pagif_init( int );	/* P-Agent I/F 初期処理 */
extern int		dbp_pagif_send(TCPIF_INF*, char*, char*);	/* P-Agentデータ送信    */
extern int		dbp_pagif_recv(TCPIF_INF*, char*);	/* P-Agentデータ受信    */
extern void		dbp_pagif_term();	/* P-Agent I/F 終了処理 */

#endif

/* END OF FILE */
