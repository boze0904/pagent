/****************************************************************************/
/* file name  db_pagent.h                                                   */
/*                                                                          */
/*  NTT IP DB P-Agent �p�w�b�_                                              */
/*                                                                          */
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef DB_PAGENT_H
#define DB_PAGENT_H

extern void	processInput( int, fd_set*, int );	/* �v���Z�X�̓��̓f�[�^�U�蕪��   */
extern void	msgtomout();						/* ���b�Z�|�W�o��                 */
extern void	illegalmsgout( char*, int );		/* �s������R�}���h���b�Z�[�W�o�� */


#endif

/* END OF FILE */
