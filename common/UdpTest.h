#pragma once

/** @brief ����UDP ����
*/

/** �ɹ�����TRUE,ʧ�ܷ���FALSE
* ����TRUE������£�dwDataTransferLatency ���� ���ݴ�����ӳ�, �����ֵ��Ч
*/
BOOL TestUdpConnection( 
	const char *server_ip,
	int server_port, 
	CCryptor *pCryptor, 
	LPCTSTR lpszUDPServer,
	LPCTSTR lpszUDPData,
	DWORD &dwDataTransferLatency );