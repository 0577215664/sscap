#pragma once

#include <string>

// ����pac�ļ�
// base64����
#define ONLINE_PAC_URL "https://raw.githubusercontent.com/tarolabs/sscap/master/bpac.txt"

using namespace std;
/** @brief ����PAC�ļ�
* 
* bForceLoad = TRUE ǿ���d, ��ʹ֮ǰ���ع���.
*/
BOOL LoadPacFile( BOOL bForceLoad = FALSE );
/** @brief ��ȡPAC�ļ�����
*/
string GetPacFileContent();
/** @brief ��������PAC�ļ�
*/
BOOL UpdateOnlinePacFile();
string GetUserPacFileContent();