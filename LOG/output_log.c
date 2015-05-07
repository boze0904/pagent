/****************************************************************/
/* File Name  : output_log.c									*/
/*                                                              */
/* Function   : openLog()										*/
/*              printLog()                                      */
/*              printDump()                                     */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdarg.h>
#include "output_log.h"

#define LOG_DIR		"/tmp/"		/* �f�t�H���g�̃��O�t�@�C���p�X */
#define LOG_NAME	"dbpagent"	/* ���O�t�@�C����               */
#define LOG_EXT		".log"		/* ���O�t�@�C���g���q           */

static int		log_lvl = WARNING;	/* ���胍�O���x�� */
static char		log_dir[512];		/* ���O�p�X       */


/****************************************************************/
/* Function Name: openLog										*/
/*                                                              */
/* Parameter    : char* fpath: ���O�o�͗p�t�@�C���p�X           */
/*                             NULL����                         */
/*                int   lvl  : ���O���x��                       */
/*                                                              */
/* Return       : NONE                                          */
/*                                                              */
/* Discription  : ���O�̏����������܂�                          */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/
void openLog( char* fpath, int lvl )
{
#ifdef DEBUG
printf( "*** openLog() ***\n" );
#endif

	/*** ���O�p�X�̐ݒ� ***/
	if( fpath == NULL ){
		strcpy( log_dir, LOG_DIR );
	}
	else{
		/* �w��p�X���̍Ō�ɃX���b�V��������H */
		if( !strncmp( fpath+strlen(fpath), "/", 1 ) ){
			strcpy( log_dir, fpath );
		}
		else{
			sprintf( log_dir, "%s/", fpath );
		}
	}
	
	/*** ���O���x����l����`�͈͓̔���������ݒ� ***/
	if( DETAIL <= lvl || ERROR >= lvl ){
		log_lvl = lvl;
	}

#ifdef DEBUG
printf( "log path %s\n", fpath );
printf( "log level %d\n", log_lvl );
#endif
} /* openLog() */


/****************************************************************/
/* Function Name: printLog										*/
/*                                                              */
/* Parameter    : int   lvl    : ���O���x��                     */
/*                char* message: ���O�o�̓t�H�[�}�b�g           */
/*                ...          : �σp�����[�^                 */
/*                                                              */
/* Discription  : ���O��W���o�͂ƃt�@�C���ɏo�͂��܂�          */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/
int printLog( int lvl, char *message, ... )
{
	char		msg[2048];
	char		fname[256];
	char		ful_fname[512];
	char		log_date[10];
	time_t		ct = time( NULL );				/* TIME */
	struct 		tm *st = localtime( &ct );		/* TIME */
	va_list		ap;
	FILE		*fp;

	/*** ����o�̓��x�� > �w�背�x�� �Ȃ� ���O�o�͂��Ȃ� ***/
	if( log_lvl > lvl ){
		return 0;
	}

	/*** �t�@�C���� ���t���擾 ***/
	memset( log_date, 0, sizeof(log_date) );
	ascftime( log_date, "%Y%m%d", st );

	/*** �t�@�C�����̐��� ***/
	sprintf( fname, "%s%s%s", LOG_NAME, log_date, LOG_EXT );

	/*** ���O�t���p�X�� ***/
	sprintf( ful_fname, "%s%s", log_dir, fname );

	/*** ���O���b�Z�[�W�p ���t/���Ԃ��擾 */
	memset( msg, 0, sizeof(msg) );
	ascftime( msg, "%Y-%m-%d %T ", st );

	/*** ���O���b�Z�[�W�t�H�[�}�b�g�̍쐬 ***/
	sprintf( msg, "%s%s\n", msg, message );

	/*** �t�@�C���̃I�[�v�� ***/
	fp = fopen( ful_fname, "a" );
	if( fp == NULL ){
		return -1;
	}

	/*** �ψ��������J�n ****/
	va_start( ap, message );
	
	/* �t�@�C���ւ̏o�� */
	vfprintf( fp, msg, ap );
	fflush( fp );
#ifdef DEBUG
	/* �W���o�͂ւ̏o�� */
	vprintf( msg, ap );
	fflush( stdout );
#endif

	/* �ψ��������I�� */
	va_end(ap);

	fclose( fp );

	return 0;
} /* printLog() */


/****************************************************************/
/* Function Name: printDump                                     */
/*                                                              */
/* Parameter    : char* fpath: ���O�o�͗p�t�@�C���p�X           */
/*                                                              */
/* Discription  : �f�[�^�̃_���v�o�͂����܂�                    */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:														*/
/****************************************************************/
int printDump( char *data )
{
	int			i;
	int			data_len;
	char		msg[2048];
	char		fname[256];
	char		ful_fname[512];
	char		log_date[10];
	time_t		ct = time( NULL );				/* TIME */
	struct 		tm *st = localtime( &ct );		/* TIME */
	FILE		*fp;

	/*** ����o�̓��x�� > DETAIL �Ȃ� ���O�o�͂��Ȃ� ***/
	if( log_lvl > DETAIL ){
		return 0;
	}

	data_len = strlen( data );

	/*** �t�@�C���� ���t���擾 ***/
	memset( log_date, 0, sizeof(log_date) );
	ascftime( log_date, "%Y%m%d", st );

	/*** �t�@�C�����̐��� ***/
	sprintf( fname, "%s%s%s", LOG_NAME, log_date, LOG_EXT );

	/*** ���O�t���p�X�� ***/
	sprintf( ful_fname, "%s%s", log_dir, fname );

	/*** ���O���b�Z�[�W�p ���t/���Ԃ��擾 */
	memset( msg, 0, sizeof(msg) );
	ascftime( msg, "%Y-%m-%d %T ", st );

	/*** �t�@�C���̃I�[�v�� ***/
	fp = fopen( ful_fname, "a" );
	if( fp == NULL ){
		return -1;
	}

	/* �t�@�C���ւ̏o�� */
	fprintf( fp, "\n%sdata dump ----------------- start -\n", msg );
	fprintf( fp, "| data_len:%4d byte (0x%04x byte)                    |", data_len, data_len );
	for( i = 0; i < data_len; i++ ){
        if( !(i % 16) ){
			if( i == 0 ){
				fprintf( fp, "\n|%04x:", i );
			}
			else{
				fprintf( fp, "|\n|%04x:", i );
			}
		}
        fprintf( fp, "%02x ", 0x000000ff & *(data+i) );
		fflush( fp );
    }
	for( ; i % 16; i++ ){
		fprintf( fp, "   " );
	}
	fprintf( fp, "|\n%sdata dump ----------------- end   -\n\n", msg );
	fflush( fp );

#ifdef DEBUG
	/* �W���o�͂ւ̏o�� */
	printf( "\n%sdata dump ----------------- start -\n", msg );
	printf( "| data_len:%4d byte (0x%04x byte)                    |", data_len, data_len );
	for( i = 0; i < data_len; i++ ){
        if( !(i % 16) ){
			if( i == 0 ){
				printf( "\n|%04x:", i );
			}
			else{
				printf( "|\n|%04x:", i );
			}
		}
        printf( "%02x ", 0x000000ff & *(data+i) );
		fflush( stdout );
    }
	for( ; i % 16; i++ ){
		printf( "   " );
	}
	printf( "|\n%sdata dump ----------------- end   -\n\n", msg );
	fflush( stdout );
#endif

	fclose( fp );

	return 0;
} /* printDump() */

/*** End Of File ***/

