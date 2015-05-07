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


char**	prop_name = NULL;	/* �v���p�e�B�̖��O */
char**	prop_val = NULL;	/* �v���p�e�B�̒l   */
static int		cnt;				/* �v���p�e�B�̌� */


/****************************************************************/
/* Function Name: openPropFile                                  */
/*                                                              */
/* Parameter    : char* fname: �ݒ�t�@�C���t���p�X��           */
/*                                                              */
/* Return       :  0:����                                       */ 
/*                -1:�ُ�                                       */
/*                                                              */
/* Discription  : �ݒ�t�@�C����ǂݍ��݂܂�                    */
/*                Property Name=Value �`���̒l�����������      */
/*                ��荞�݂܂�                                  */
/*                                                              */
/* Create: Jul.09.2001                                          */
/* Modify:                                                      */
/****************************************************************/
int openPropFile( char* fname )
{
	char*	work;
	char	line_buff[1024];	/* �s�o�b�t�@ */
	int     len;				/* ���[�N     */
	FILE	*fp;

#ifdef DEBUG
printf( "*** openPropFile(%s) ***\n", fname );
#endif

	/*** �ݒ�t�@�C���̃I�[�v�� ***/
	fp = fopen( fname, "r" );
	if( fp == NULL ){
		return -1;
	}

	/* �t�@�C���ǂݍ��� */
	for( cnt = 0; fgets( line_buff, sizeof(line_buff), fp ) != NULL; ){
		/* ���s */
		if( line_buff[0] == '\n' )
			continue;

		/* �R�����g�s */
		if( line_buff[0] == '#' )
			continue;

		/* �s�������s�̏ꍇ�k���I�[�ɂ���B */
		len = strlen( line_buff );
		if( line_buff[len-1] == '\n' )
			line_buff[len-1] = 0x00;

		len = strlen( line_buff );
		work = (char*)strtok( line_buff, "=" );
		/* Name=Value �`���łȂ����́A��΂� */
		if( strlen(work) == len )
			continue;

		/*** �v���p�e�B���̐ݒ� ***/
		prop_name = (char**)realloc( prop_name, (cnt+1) * sizeof(char*) );
		prop_name[cnt] = (char*)malloc( (strlen(work) + 1) );
		memset( prop_name[cnt], 0, sizeof( prop_name[cnt] ) );
		strcpy( prop_name[cnt], work );
			
		work = (char*)strtok( NULL, "=" );

		/*** �v���p�e�B�̒l�̐ݒ� ***/
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
/* Parameter    : char* prop: �v���p�e�B��                      */
/*                                                              */
/* Return       : char* �v���p�e�B�̒l                          */ 
/*                �Y���̃v���p�e�B�����Ȃ��ꍇNULL              */
/*                                                              */
/* Discription  : �w�肳�ꂽ�v���p�e�B���̒l���擾���܂�        */
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
			/* �l��Ԃ� */
			return *(prop_val + i);
		}
	}
	/* �Y���Ȃ� */
	return NULL;
}


/****************************************************************/
/* Function Name: closePropFile                                 */
/*                                                              */
/* Parameter    : NONE                                          */
/*                                                              */
/* Return       : NONE                                          */ 
/*                                                              */
/* Discription  : �ݒ�t�@�C���ǂݍ��݂̌㏈�������܂�          */
/*                openPropFile()���Ăяo������́A������        */
/*                �̈�J���̂��߁A�K�����̊֐����Ăяo����      */
/*                ������                                        */
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

