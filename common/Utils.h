#pragma once
#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <tchar.h>
#include <tcpmib.h>

using namespace std;
#define MY_FREE(x) { if(x){free(x); x =NULL;}}
#define MY_DELEETE(x) { if(x){delete x;x=NULL;}}
#define MY_DELEETE_ARR(x) { if(x){delete []x; x=NULL;}}

// http/https
#define HTTP	3
// SOCKS 4/4A
#define SOCKS4  4
// SOCKS 5
#define SOCKS5  5
// shadowsocks����
#define SHADOWSOCKS 6

#define PROXY_IP_ADDRESS_MAX_LENGTH 260
#define PROXY_USERNAME_MAX_LENGTH 64
#define PROXY_PASSWORD_MAX_LENGTH 64
#define PROXY_REAL_IP_ADDRESS_MAX_LANGEHT 33

wchar_t * WINAPI lm_a2u (const char *str);
char * WINAPI lm_u2a_s (wchar_t * str);
char * WINAPI lm_u2a (wchar_t * str);
wchar_t* WINAPI lm_a2u_s (const char *str);
wchar_t* WINAPI lm_u82u16_s (const char *u8);
wchar_t* WINAPI lm_u82u16 (const char *u8);
char * WINAPI lm_u162u8_s (wchar_t* u16);
char * WINAPI lm_u162u8 (wchar_t * str);

// get time format string for specific format.
void GetTimeString( time_t t ,TCHAR *szTimeFormat ,TCHAR *szFormatOut, int len );
int WINAPI SplitStringW(const wstring& input, const wstring& delimiter, vector<wstring>& results, bool includeEmpties = true );
int WINAPI SplitStringA(const string& input, const string& delimiter, vector<string>& results, bool includeEmpties = true );
/** �޼�string��ǰ��ո�*/
std::string& WINAPI trim(string& s, const std::string& drop =" ");
/** �޼�string��ǰ��ո�*/
std::wstring& WINAPI trimW(wstring& s, const std::wstring& drop =L" ");
// �ж��ļ��Ƿ����
bool IsFileExist(TCHAR *file);
typedef struct _LSA_UNICODE_STRING {
	USHORT Length;
	USHORT MaximumLength;
	PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING, UNICODE_STRING, *PUNICODE_STRING;
typedef LONG NTSTATUS;
char* readallfile(TCHAR *fname,TCHAR *flags);
char* readallfile(TCHAR *fname,TCHAR *flags ,int *outlength );
/** @brief �������ʱ�����������
* ����difftime����,����ָ��ʱ����Ⱥ�˳��.
* ��������е�ʱ��˳��������ָ��
*
* @param[in] t1 ʱ��1
* @param[in] t2 ʱ��2
*
* @return t1��t2��ʱ���(��)
*/
double GetTimeInterval(time_t t1 ,time_t t2);
//unsigned char *base64_decode(const char *str, int *ret_len);
/** @brief ��ʾ���ʵ��ֽڵ�λ������
* <1024����ʾ�ֽ�, >1024 ��ʾKb, >1048576��ʾMb...
*
* @param[in] bytes ���ݳ������ֽ�Ϊ��λ
* @param[out] szBuffer �����ȷ�ĵ�λ�ĸ�ʽ
* @param[in] len szBuffer�ĳ���.
*/
void Show_bytes_in_char( long long bytes, TCHAR *szBuffer, int len );
string   Replace(string   &str,  string string_to_replace, string new_string);
BOOL PutTextToClipboardA(LPCSTR pTxtData,HWND hWnd);

//char * base64_encode(const unsigned char *data, int len);
/** @brief ���ĳ��TCP�˿��Ƿ�ռ����
*
* @returns
* -TRUE: ָ���Ķ˿ڿ���
* -FALSE; ָ���Ķ˿ڲ�����
*/
BOOL CheckTcpPortValid( UINT nPort );

/** @brief ����һ��δռ�õĶ˿�
*
* @param nFromPort ��ʼ�����˿�
* @param nSearchAmount ���������� 
* @returns 0 û���ҵ��˿�, ��0�ҵ���δռ�õĶ˿�
*/
UINT SearchAnUnsedPort( UINT nFromPort ,UINT nSearchAmount );
/** @brief ��һ��vector<string> ��ϳ��ַ���, ��delim�ָ�, ������PHP��implode
*/
string implode(const vector<string>& vec, const char* delim);

/** @brief �������ڲ��Ե�Ŀ��URL��ַ, ������HOST, OBJ, PORT
*
* @param szURL Ҫ������URL
* @param szProtocol ����������Э��
* @param szAddress ����������HOST
* @param dwPort �� ��
* @param szURI ����������OBJ
*/
BOOL ParseDestinationUrl( LPCSTR szURL, LPSTR szProtocol,int p_len, LPSTR szAddress,int a_len, u_short &dwPort, LPSTR szURI, int u_len  );
char * strdup_printf(const char* fmt,...);

/** @brief CHotKeyCtrl��ֵת��RegisterHotKey
* CHotKeyCtrl::SetHotKey��GetHotKey�е�wModifiers��RegisterHotKey�е�fsModifiers��ͬ��
* a. ��CHotKeyCtrl�ؼ��õ���Alt��Shift�ֱ���HOTKEYF_ALT��HOTKEYF_SHIFT����RegisterHotKey�е�Alt��Shift��ΪMOD_ALT��MOD_SHIFT������GetHotKey֮����Ҫע������Ҫ����ת��
* b. SetHotKeyʱ��Ҳ��Ҫ������ص�ת������
*/
WORD HKF2SDK(WORD mod);


/** @brief ��stringתΪСд ansi*/
std::string LowerStringA(const std::string &str );