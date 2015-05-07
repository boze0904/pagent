/****************************************************************************/
/* file name  ProcFindFreePortInfo.h                                        */
/*                                                                          */
/* ISDN G/W ProcFindFreePortInfo.c �p�w�b�_�t�@�C��                         */
/*                                                                          */
/* Create : Jul.09.2001                                                     */
/* Modify :                                                                 */
/****************************************************************************/
#ifndef PROC_FIND_FREE_PORT_INFO_H
#define PROC_FIND_FREE_PORT_INFO_H


/*** �|�[�g����� �\���� ***/
struct PORT_INFO{
    int     all_free_port;
    int     unused_port;
    int     all_work_port;
    int     work_port;
    int     wait_port;
};

extern int getFreePort( char*, char*, char*, struct PORT_INFO* );
extern int getWorkPort( char*, char*, char*, struct PORT_INFO* );
extern int getWaitPort( char*, char*, char*, struct PORT_INFO* );


#endif

/* END OF FILE */
