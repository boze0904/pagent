/****************************************************************************/
/* file name	:dbp_portsub.h												*/
/*																			*/
/*	NTT IP P-Agent G/W �T�u�����p�w�b�_										*/
/*																			*/
/* Create : 2000.12.15                                                      */
/* Modify :                                                                 */ 
/****************************************************************************/
#ifndef DBP_PORTSUB_H
#define DBP_PORTSUB_H

extern int		dbpsub_init();				/* ��������               */
extern void		dbpsub_netx_gw(char*,int);	/* NetExpert -> G/W       */
extern int		dbpsub_gw_netx(char*,int);	/* G/W       -> NetExpert */
extern void		dbpsub_term();				/* �I������               */


#endif

/* END OF FILE */
