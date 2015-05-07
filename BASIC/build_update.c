/****************************************************************************/
/* file nanme  BUILD_Update.c                                                */
/*                                                                          */
/* Vantive用データベースGateWayプロセス                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "tcp_if.h"
#include "gw_if.h"
#include "dbpag_if.h"
#include "db_func.h"
#include "pagent_trace.h"
#include "proc_str.h"
#include "common.h"

#define  MAX_DATA_LEN   GWP_MXPAGDTLEN + 1

extern int      Proc_BUILD_Update(BUILD*);


/****************************************************************************/
/* Name : build_update                                                       */
/*                                                                          */
/* Parameter : char *src_data                                               */
/*                                                                          */
/* Return : 0  正常終了                                                     */
/*         -1  異常終了                                                     */
/*                                                                          */
/* Create : 2000.12.05                                                      */
/* Modify :                                                                 */
/****************************************************************************/
int build_update(char *src_data) {
	char	tmp_data[12][MAX_DATA_LEN];
	char    *data;
	char    *p_data = src_data;
	int     i = 0;
	int     resp = 0;
    BUILD	build;


	memset(&tmp_data[0][0], NULL, 12*MAX_DATA_LEN);
	memset(&build, NULL, sizeof(build));

	while((data = strchr(p_data, ',')) != NULL)
	{
		data = strchr(p_data, ',');
		if( i < 11 && (data - p_data) < MAX_DATA_LEN)
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

	if( i == 11 )
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

#if(0)  /*  NetExpertから渡されるデータ長は固定ではないので使わない */
	if(i == 11 && strlen(tmp_data[0]) <= BUILD_BUILDNAMEKANA_LEN && 
					strlen(tmp_data[1]) <= BUILD_BUILDCODE_LEN &&
					strlen(tmp_data[2]) <= BUILD_BUILDNAME_LEN && 
					strlen(tmp_data[3]) <= BUILD_BRANCHCODE_LEN &&
					strlen(tmp_data[4]) <= BUILD_ACOMODATEAREA_LEN && 
					strlen(tmp_data[5]) <= BUILD_BUILDTYPE_LEN &&
					strlen(tmp_data[6]) <= BUILD_BUILDADDRESS_LEN &&
					strlen(tmp_data[7]) <= BUILD_FLOOR_LEN &&
					strlen(tmp_data[8]) <= BUILD_MECONTACTID_LEN &&
					strlen(tmp_data[9]) <= BUILD_PHONENUMBER_LEN &&
					strlen(tmp_data[10]) <= BUILD_ADMINAREA_LEN &&
					strlen(tmp_data[11]) <= BUILD_PUBLICBUILD_LEN )
	{
#endif
    if(i == 11) {
        strncpy(build.BuildNameKana, &tmp_data[0][0], BUILD_BUILDNAMEKANA_LEN);
        strncpy(build.BuildCode, &tmp_data[1][0], BUILD_BUILDCODE_LEN);
        strncpy(build.BuildName, &tmp_data[2][0], BUILD_BUILDNAME_LEN);
        strncpy(build.BranchCode, &tmp_data[3][0], BUILD_BRANCHCODE_LEN);
        strncpy(build.AcomodateArea, &tmp_data[4][0], BUILD_ACOMODATEAREA_LEN);
        strncpy(build.BuildType, &tmp_data[5][0], BUILD_BUILDTYPE_LEN);
        strncpy(build.BuildAddress, &tmp_data[6][0], BUILD_BUILDADDRESS_LEN);
        strncpy(build.Floor, &tmp_data[7][0], BUILD_FLOOR_LEN);
        strncpy(build.MeContactid, &tmp_data[8][0], BUILD_MECONTACTID_LEN);
        strncpy(build.PhoneNumber, &tmp_data[9][0], BUILD_PHONENUMBER_LEN);
        strncpy(build.AdminArea, &tmp_data[10][0], BUILD_ADMINAREA_LEN);
        strncpy(build.PublicBuild, &tmp_data[11][0], BUILD_PUBLICBUILD_LEN);

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
fflush( stdout );
printf("############ PROC受け取りデータ分割 #####################\n");
printf("build.BuildNameKana=%s\n",build.BuildNameKana);
printf("build.BuildCode=%s\n",build.BuildCode);
printf("build.BuildName=%s\n",build.BuildName);
printf("build.BranchCode=%s\n",build.BranchCode);
printf("build.AcomodateArea=%s\n",build.AcomodateArea);
printf("build.BuildType=%s\n",build.BuildType);
printf("build.BuildAddress=%s\n",build.BuildAddress);
printf("build.Floor=%s\n",build.Floor);
printf("build.MeContactid=%s\n",build.MeContactid);
printf("build.PhoneNumber=%s\n",build.PhoneNumber);
printf("build.AdminArea=%s\n",build.AdminArea);
printf("build.PublicBuild=%s\n",build.PublicBuild);
fflush( stdout );
printf("############ END OF 分割データ  #########################\n");
fflush( stdout );
#endif

		resp = Proc_BUILD_Update(&build);
	}
	else
	{
		resp = -1;
	} 

    return resp;

} /* data_update */

/* End of File */

