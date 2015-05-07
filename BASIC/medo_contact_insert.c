/****************************************************************************/
/* file nanme  medo_contact_insert.c                                        */
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

extern int      Proc_MEDO_CONTACT_Insert( MEDO_CONTACT* );

/****************************************************************************/
/* Name : medo_contact_insert                                               */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int medo_contact_insert(char *src_data) {

	char	tmp_data[11][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    MEDO_CONTACT	medo_contact;


	memset(&tmp_data[0][0], NULL, 11*MAX_DATA_LEN);
	memset(&medo_contact, NULL, sizeof(medo_contact));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 10 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 10 )
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
	if(i == 10 && strlen(tmp_data[0]) <= MEDO_CONTACT_MECONTACTID_LEN && 
					strlen(tmp_data[1]) <= MEDO_CONTACT_MEMAINTENANCEAREA_LEN &&
					strlen(tmp_data[2]) <= MEDO_CONTACT_MECENTERNAME_LEN && 
					strlen(tmp_data[3]) <= MEDO_CONTACT_MEBASENAME_LEN &&
					strlen(tmp_data[4]) <= MEDO_CONTACT_MEDIVISION_LEN && 
					strlen(tmp_data[5]) <= MEDO_CONTACT_MEPERSONDAY_LEN &&
					strlen(tmp_data[6]) <= MEDO_CONTACT_MECONTACTDAY_LEN &&
					strlen(tmp_data[7]) <= MEDO_CONTACT_NIGHTSERVICEFLAG_LEN &&
					strlen(tmp_data[8]) <= MEDO_CONTACT_MECONTACTNIGHT_LEN &&
					strlen(tmp_data[9]) <= MEDO_CONTACT_FAXNUMBERDAY_LEN &&
					strlen(tmp_data[10]) <= MEDO_CONTACT_PORTABLETELEPHONE_LEN)
#endif
	if(i == 10) 
	{
		strncpy(medo_contact.MeContactid, &tmp_data[0][0], MEDO_CONTACT_MECONTACTID_LEN);
		strncpy(medo_contact.MeMaintenanceArea, &tmp_data[1][0], MEDO_CONTACT_MEMAINTENANCEAREA_LEN);
		strncpy(medo_contact.MeCenterName, &tmp_data[2][0], MEDO_CONTACT_MECENTERNAME_LEN);
		strncpy(medo_contact.MeBaseName, &tmp_data[3][0], MEDO_CONTACT_MEBASENAME_LEN);
		strncpy(medo_contact.MeDivision, &tmp_data[4][0], MEDO_CONTACT_MEDIVISION_LEN);
		strncpy(medo_contact.MePersonDay, &tmp_data[5][0], MEDO_CONTACT_MEPERSONDAY_LEN);
		strncpy(medo_contact.MeContactDay, &tmp_data[6][0], MEDO_CONTACT_MECONTACTDAY_LEN);
		strncpy(medo_contact.NightServiceFlag, &tmp_data[7][0], MEDO_CONTACT_NIGHTSERVICEFLAG_LEN);
		strncpy(medo_contact.MeContactNight, &tmp_data[8][0], MEDO_CONTACT_MECONTACTNIGHT_LEN);
		strncpy(medo_contact.FaxNumberDay, &tmp_data[9][0], MEDO_CONTACT_FAXNUMBERDAY_LEN);
		strncpy(medo_contact.PortableTelephone, &tmp_data[10][0], MEDO_CONTACT_PORTABLETELEPHONE_LEN);
#if(0)  /*  前バージョンの名残り    今は使わない    */
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
printf("tmp_data[9]=%s\n",tmp_data[9]);
printf("tmp_data[10]=%s\n",tmp_data[10]);
fflush( stdout );
#if(0)  /*  前バージョンの名残り    今は使わない    */
#endif
printf("############ PROC受け取りデータ分割 #####################\n");
printf("medo_contact.MeContactid=%s\n",medo_contact.MeContactid);
printf("medo_contact.MeMaintenanceArea=%s\n",medo_contact.MeMaintenanceArea);
printf("medo_contact.MeCenterName=%s\n",medo_contact.MeCenterName);
printf("medo_contact.MeBaseName=%s\n",medo_contact.MeBaseName);
printf("medo_contact.MeDivision=%s\n",medo_contact.MeDivision);
printf("medo_contact.MePersonDay=%s\n",medo_contact.MePersonDay);
printf("medo_contact.MeContactDay=%s\n",medo_contact.MeContactDay);
printf("medo_contact.NightServiceFlag=%s\n",medo_contact.NightServiceFlag);
printf("medo_contact.MeContactNight=%s\n",medo_contact.MeContactNight);
printf("medo_contact.FaxNumberDay=%s\n",medo_contact.FaxNumberDay);
printf("medo_contact.PortableTelephone=%s\n",medo_contact.PortableTelephone);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#if(0)  /*  前バージョンの名残り    今は使わない    */
#endif
#endif

		resp = Proc_MEDO_CONTACT_Insert(&medo_contact);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* medo_contact_insert */

/* End of File */

