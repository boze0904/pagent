/****************************************************************************/
/* file nanme  maker_contact_insert.c                                       */
/*                                                                          */
/* Vantive用データベースGateWayプロセス                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h> 
#include "tcp_if.h"
#include "gw_if.h"
#include "dbpag_if.h"
#include "db_func.h"
#include "pagent_trace.h"
#include "proc_str.h"
#include "common.h"

#define  MAX_DATA_LEN   GWP_MXPAGDTLEN + 1

extern int      Proc_MAKER_CONTACT_Insert( MAKER_CONTACT* );


/****************************************************************************/
/* Name : maker_contact_insert                                              */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int maker_contact_insert(char *src_data) {

	char	tmp_data[9][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    MAKER_CONTACT	maker_contact;


	memset(&tmp_data[0][0], NULL, 9*MAX_DATA_LEN);
	memset(&maker_contact, NULL, sizeof(maker_contact));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 8 && (data - p_data) < MAX_DATA_LEN)
		{
			strncpy(tmp_data[i], p_data, data - p_data);
			p_data = data + 1;
			i ++;
		}
		else
		{
			i = 99;
			break;
		}
	}

	if( i == 8 )
	{
		if(strlen(p_data) < MAX_DATA_LEN)
		{
			strcpy(tmp_data[i], p_data);
		}
		else
		{
			return 2;
		}
	}else{
		return 2;
	}

#if(0)
	if(i == 8 && strlen(tmp_data[0]) <= MAKER_CONTACT_MAKERCONTACTID_LEN && 
					strlen(tmp_data[1]) <= MAKER_CONTACT_MAKERNAME_LEN &&
					strlen(tmp_data[2]) <= MAKER_CONTACT_MAKERARRANGE_LEN && 
					strlen(tmp_data[3]) <= MAKER_CONTACT_MAKERCONTACTDAY_LEN &&
					strlen(tmp_data[4]) <= MAKER_CONTACT_MAKERPERSONDAY_LEN && 
					strlen(tmp_data[5]) <= MAKER_CONTACT_FAXNUMBERDAY_LEN &&
					strlen(tmp_data[6]) <= MAKER_CONTACT_MAKERCONTACTNIGHT_LEN &&
					strlen(tmp_data[7]) <= MAKER_CONTACT_MAKERPERSONNIGHT_LEN &&
					strlen(tmp_data[8]) <= MAKER_CONTACT_FAXNUMBERNIGHT_LEN )
#endif
	if(i == 8) 
	{
		strncpy(maker_contact.MakerContactid, &tmp_data[0][0], MAKER_CONTACT_MAKERCONTACTID_LEN);
		strncpy(maker_contact.MakerName, &tmp_data[1][0], MAKER_CONTACT_MAKERNAME_LEN);
		strncpy(maker_contact.MakerArrange, &tmp_data[2][0], MAKER_CONTACT_MAKERARRANGE_LEN);
		strncpy(maker_contact.MakerContactDay, &tmp_data[3][0], MAKER_CONTACT_MAKERCONTACTDAY_LEN);
		strncpy(maker_contact.MakerPersonDay, &tmp_data[4][0], MAKER_CONTACT_MAKERPERSONDAY_LEN);
		strncpy(maker_contact.FaxNumberDay, &tmp_data[5][0], MAKER_CONTACT_FAXNUMBERDAY_LEN);
		strncpy(maker_contact.MakerContactNight, &tmp_data[6][0], MAKER_CONTACT_MAKERCONTACTNIGHT_LEN);
		strncpy(maker_contact.MakerPersonNight, &tmp_data[7][0], MAKER_CONTACT_MAKERPERSONNIGHT_LEN);
		strncpy(maker_contact.FaxNumberNight, &tmp_data[8][0], MAKER_CONTACT_FAXNUMBERNIGHT_LEN);
#if(0)  /*  前ばーじょん名残り 今は使わない */
#endif

#ifdef DEBUG
printf("############ NETEXPERTから受け取りデータ分割 ############\n");
printf("tmp_data[0]=%s\n",tmp_data[0]);
printf("tmp_data[1]=%s\n",tmp_data[1]);
printf("tmp_data[2]=%s\n",tmp_data[2]);
printf("tmp_data[3]=%s\n",tmp_data[3]);
printf("tmp_data[4]=%s\n",tmp_data[4]);
printf("tmp_data[5]=%s\n",tmp_data[5]);
printf("tmp_data[6]=%s\n",tmp_data[6]);
printf("tmp_data[7]=%s\n",tmp_data[7]);
printf("tmp_data[8]=%s\n",tmp_data[8]);
fflush( stdout );
#if(0)  /*  前ばーじょん名残り 今は使わない */
#endif
printf("############ PROC受け取りデータ分割 #####################\n");
printf("maker_contact.MakerContactid=%s\n",maker_contact.MakerContactid);
printf("maker_contact.MakerName=%s\n",maker_contact.MakerName);
printf("maker_contact.MakerArrange=%s\n",maker_contact.MakerArrange);
printf("maker_contact.MakerContactDay=%s\n",maker_contact.MakerContactDay);
printf("maker_contact.MakerPersonDay=%s\n",maker_contact.MakerPersonDay);
printf("maker_contact.FaxNumberDay=%s\n",maker_contact.FaxNumberDay);
printf("maker_contact.MakerContactNight=%s\n",maker_contact.MakerContactNight);
printf("maker_contact.MakerPersonNight=%s\n",maker_contact.MakerPersonNight);
printf("maker_contact.FaxNumberNight=%s\n",maker_contact.FaxNumberNight);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#if(0)  /*  前ばーじょん名残り 今は使わない */
#endif
#endif

		resp = Proc_MAKER_CONTACT_Insert(&maker_contact);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* maker_contact_insert */

/* End of File */

