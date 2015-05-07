/****************************************************************************/
/* file nanme  isp_contact_update.c                                         */
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

extern int      Proc_ISP_CONTACT_Update( ISP_CONTACT* );


/****************************************************************************/
/* Name : isp_contact_update                                                */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int isp_contact_update(char *src_data) {

	char	tmp_data[14][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    ISP_CONTACT	isp_contact;


	memset(&tmp_data[0][0], NULL, 14*MAX_DATA_LEN);
	memset(&isp_contact, NULL, sizeof(isp_contact));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 13 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 13 )
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
	if(i == 13 && strlen(tmp_data[0]) <= ISP_CONTACT_ISPCODE_LEN &&
                    strlen(tmp_data[1]) <= ISP_CONTACT_ISPCORPNAME_LEN &&
                    strlen(tmp_data[2]) <= ISP_CONTACT_ISPSERVICENAME_LEN &&
                    strlen(tmp_data[3]) <= ISP_CONTACT_MAINTENAME_LEN &&
                    strlen(tmp_data[4]) <= ISP_CONTACT_MAINTENUMBER_LEN &&
                    strlen(tmp_data[5]) <= ISP_CONTACT_MAINTEREASON_LEN &&
                    strlen(tmp_data[6]) <= ISP_CONTACT_MAINTEMAILTO_LEN &&
                    strlen(tmp_data[7]) <= ISP_CONTACT_MAINTETIME_LEN &&
                    strlen(tmp_data[8]) <= ISP_CONTACT_CONSTNAME_LEN &&
                    strlen(tmp_data[9]) <= ISP_CONTACT_CONSTNUMBER_LEN &&
                    strlen(tmp_data[10]) <= ISP_CONTACT_CONSTREASON_LEN &&
                    strlen(tmp_data[11]) <= ISP_CONTACT_CONSTMAILTO_LEN &&
                    strlen(tmp_data[12]) <= ISP_CONTACT_CONSTTIME_LEN &&
                    strlen(tmp_data[13]) <= ISP_CONTACT_ISPMEMO_LEN )
#endif
	if(i == 13) 
    {
		strncpy(isp_contact.ISPCode, &tmp_data[0][0], ISP_CONTACT_ISPCODE_LEN);
        strncpy(isp_contact.ISPCorpName, &tmp_data[1][0], ISP_CONTACT_ISPCORPNAME_LEN);
        strncpy(isp_contact.ISPServiceName, &tmp_data[2][0], ISP_CONTACT_ISPSERVICENAME_LEN);
		strncpy(isp_contact.MainteName, &tmp_data[3][0], ISP_CONTACT_MAINTENAME_LEN);
		strncpy(isp_contact.MainteNumber, &tmp_data[4][0], ISP_CONTACT_MAINTENUMBER_LEN);
		strncpy(isp_contact.MainteReason, &tmp_data[5][0], ISP_CONTACT_MAINTEREASON_LEN);
		strncpy(isp_contact.MainteMailTo, &tmp_data[6][0], ISP_CONTACT_MAINTEMAILTO_LEN);
		strncpy(isp_contact.MainteTime, &tmp_data[7][0], ISP_CONTACT_MAINTETIME_LEN);
		strncpy(isp_contact.ConstName, &tmp_data[8][0], ISP_CONTACT_CONSTNAME_LEN);
		strncpy(isp_contact.ConstNumber, &tmp_data[9][0], ISP_CONTACT_CONSTNUMBER_LEN);
		strncpy(isp_contact.ConstReason, &tmp_data[10][0], ISP_CONTACT_CONSTREASON_LEN);
		strncpy(isp_contact.ConstMailTo, &tmp_data[11][0], ISP_CONTACT_CONSTMAILTO_LEN);
		strncpy(isp_contact.ConstTime, &tmp_data[12][0], ISP_CONTACT_CONSTTIME_LEN);
		strncpy(isp_contact.IspMemo, &tmp_data[13][0], ISP_CONTACT_ISPMEMO_LEN);
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
printf("tmp_data[9]=%s\n",tmp_data[9]);
printf("tmp_data[10]=%s\n",tmp_data[10]);
printf("tmp_data[11]=%s\n",tmp_data[11]);
printf("tmp_data[12]=%s\n",tmp_data[12]);
printf("tmp_data[13]=%s\n",tmp_data[13]);
fflush( stdout );
#if(0)  /*  前ばーじょん名残り 今は使わない */
#endif
printf("############ PROC受け取りデータ分割 #####################\n");
printf("isp_contact.ISPCode=%s\n",isp_contact.ISPCode);
printf("isp_contact.ISPCorpName=%s\n",isp_contact.ISPCorpName);
printf("isp_contact.ISPServiceName=%s\n",isp_contact.ISPServiceName);
printf("isp_contact.MainteName=%s\n",isp_contact.MainteName);
printf("isp_contact.MainteNumber=%s\n",isp_contact.MainteNumber);
printf("isp_contact.MainteReason=%s\n",isp_contact.MainteReason);
printf("isp_contact.MainteMailTo=%s\n",isp_contact.MainteMailTo);
printf("isp_contact.MainteTime=%s\n",isp_contact.MainteTime);
printf("isp_contact.ConstName=%s\n",isp_contact.ConstName);
printf("isp_contact.ConstNumber=%s\n",isp_contact.ConstNumber);
printf("isp_contact.ConstReason=%s\n",isp_contact.ConstReason);
printf("isp_contact.ConstMailTo=%s\n",isp_contact.ConstMailTo);
printf("isp_contact.ConstTime=%s\n",isp_contact.ConstTime);
printf("isp_contact.IspMemo=%s\n",isp_contact.IspMemo);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#if(0)  /*  前ばーじょん名残り 今は使わない */
#endif
#endif

		resp = Proc_ISP_CONTACT_Update(&isp_contact);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* isp_contact_update */

/* End of File */

