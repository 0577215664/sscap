#pragma once

#define SSCAP_PRIVOXY_LOCAL_IP "127.0.0.1"

/** @brief ����privoxy����
*/
BOOL RunPrivoxy( );
/** @biref ��ȡprivoxy���̹����˿�
*/
unsigned short GetPrivoxyListenPort();
BOOL IsPrivoxyStarted();