/****************************************************************/
/* File Name  : output_log.h									*/
/*                                                              */
/* Discription: output_log.c �p�̃w�b�_�t�@�C���@�@�@�@�@�@�@ �@*/
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/

#ifndef OUTPUT_LOG_H
#define OUTPUT_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

/*** �O���A�N�Z�X�֐� ***/
extern void openLog( char*, int );
extern int printLog( int , char*, ... );
extern int printDump( char* );

/*** ���O���x�� ***/
#define DETAIL		1
#define DEBUGING	2
#define WARNING		3
#define	ERROR		4

/*** ���O�p�ݒ�t�@�C���v���p�e�B�� ***/
#define	LOGPATH	"LOG_PATH"
#define	LOGLVL	"LOG_LEVEL" 

#ifdef __cplusplus
}
#endif

#endif


/*** End Of File ***/

