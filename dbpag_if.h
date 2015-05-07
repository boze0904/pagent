/****************************************************************************/
/* file name  dbpag_if.h                                                    */
/*                                                                          */
/* G/W �� DB P-Agent �̃f�[�^����M�֐��Q�w�b�_                             */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef DBPAG_IF_H
#define DBPAG_IF_H

extern int		dbp_pagif_init( int );	/* P-Agent I/F �������� */
extern int		dbp_pagif_send(TCPIF_INF*, char*, char*);	/* P-Agent�f�[�^���M    */
extern int		dbp_pagif_recv(TCPIF_INF*, char*);	/* P-Agent�f�[�^��M    */
extern void		dbp_pagif_term();	/* P-Agent I/F �I������ */

#endif

/* END OF FILE */
