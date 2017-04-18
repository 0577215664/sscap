#pragma once

/*!
 * @file Encypter.h
 * @date 2015/10/19 18:18
 *
 * @brief ���ܻ���
 *
 * 
 * @author Taro
 * Contact: sockscap64@gmail.com
 *
 *
 * @version 1.0
 *
 * @History
 * <author>      <time>      <version>         <desc>
 *
 * @TODO: long description
 *
 * @note
*/
#include <string>
#include <map>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1

#include "cryptopp562/cryptlib.h"
#include "cryptopp562/hex.h"
#include "cryptopp562/aes.h"
#include "cryptopp562/ccm.h"
#include "cryptopp562/modes.h"
#include "cryptopp562/arc4.h"
#include "cryptopp562/salsa.h"

#include <time.h>
#include "cryptopp562/md5.h"

using CryptoPP::Exception;
using CryptoPP::AES;
using CryptoPP::CBC_Mode;
using CryptoPP::CFB_Mode;
using CryptoPP::HexEncoder;
using CryptoPP::HexDecoder;
//using CryptoPP::Weak1;
using CryptoPP::Salsa20;

#include "include/sodium.h"
using namespace std;

#define AES_256_CFB "aes-256-cfb"
#define IDX_AES_256_CFB 0

#define AES_192_CFB "aes-192-cfb"
#define IDX_AES_192_CFB 1

#define AES_128_CFB "aes-128-cfb"
#define IDX_AES_128_CFB 2

#define RC4_MD5 "rc4-md5"
#define IDX_RC4_MD5 3

#define RC4 "rc4"
#define IDX_RC4 4

#define SALSA20 "salsa20"
#define IDX_SALSA20 5

#define CHACHA20 "chacha20"
#define IDX_CHACHA20 6

#define CHACHA20_IETF_ENC "chacha20-ietf"
#define IDX_CHACHA20_IETF_ENC 7

#define TABLE_ENC "table"
#define IDX_TABLE_ENC 8

using namespace std;

class CCryptor
{
public:
	CCryptor( string method_name, string key ,int key_size /* bytes */, int iv_size /* bytes */ );
	~CCryptor();

public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len) = 0;
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len) = 0;
	/** @brief ���ü�����Ϣ, ������������KEY, IV 
	*/
	virtual void ResetCryption();
protected:
	/** @brief ��ʼ�����ڼ���shadowsocks����key
	* ����CacheKey����,�ҵ��ͷ���, ��δ�ҵ��͸���key���������յ�key
	* ������: 
	*  1: ��key��md5�õ�16 bytes��Ϊǰ16 bytes
	*  2: ����1���õ���16bytes + ԭʼkey��ϵõ� result , �ٶ�results ��md5 �ֵõ�16 bytes
	*  3: ����1���õ���16 bytes + ��2���õ���16 byte�鱣�����յ�32 bytes�� key
	*/
	virtual void InitializeSSEncryptKey();
	virtual void GenerateRandomEncryptIV( char *iv, int length );
	/** @brief ����strInputPass��md5
	*/
	virtual void GetPasswordHash( char *hash /* out */ );
protected:
	string strMethodName; ///< ���ܷ�ʽ������, ��: rc4, aes-256-cfb
	string strInputPass;   ///< �û�ָ����pass ��������InitializeSSKey֮���������յ�key: ( strEncryptKey )
	char strEncryptKey[32];

	bool bEncryptIVSent; ///< ���ط��ͳ�ȥ�����ݵļ���IV�ѷ�
	char ivEncrypt[32];

	bool bDecryptIVGot;  ///< SS�����������������ݼ��ܵ�IV����
	char ivDecrypt[16];

	int nKeySize; ///< ���ܵ�KEY��SIZE(�ֽ�)
	int nIVSize; ///< IVSIZE(�ֽ�)
};

/** @brief ʵ��: aes-128-cfb,aes-192-cfb,aes-256-cfb �ļӽ���
*/
class CAESCfbCryptor : public CCryptor
{
public:
	CAESCfbCryptor ( string key ,int bits /** key�Ƕ���λ��: 128, 192, 256 */ );
	virtual ~CAESCfbCryptor();

public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len);
protected:
	CFB_Mode<AES>::Encryption AesCfbEncryption;
	CFB_Mode<AES>::Decryption AesCfbDecryption;
};

/** @brief ʵ��rc4, �Լ�shadowsocks�е�rc4-md5�ļӽ���
*
* rc4-md5��ss�����Լ������
* �ɵ� RC4 ����֮���Բ���ȫ����Ϊ Shadowsocks ��ÿ���������ظ�ʹ�� key��û��ʹ�� IV��
* �� RC4 �� AES �ٶȿ�ü������������·�����ϻ������������������
* ���԰�����ȷ�ķ�ʽ����ʵ���� RC4 ���ܣ�Ϊ������ɵ� RC4 ����Ϊ RC4-MD5��
* https://github.com/shadowsocks/shadowsocks/issues/178
* rc4-md5��ʵ�־��ǽ�key��hashһ��
* true_key = hash(key + IV)
* 
* 
* RC4-MD5 spec:
* Generate key from password via EVP_bytes_to_key as usual.
* key is 16 bytes and IV is 16 bytes.
* Generate true RC4 key from key and IV: rc4_key = md5(key + IV). Thus we have a different RC4 key per * * connection. rc4_key is 16 bytes.
* Use RC4 key to do encryption & decryption.
*/
class CRC4Cryptor: public CCryptor
{
public:
	CRC4Cryptor( string key ,bool bRC4Md5 /** �Ƿ�Ϊrc4-md5 */ );
	virtual ~CRC4Cryptor();

protected:
	bool bIsRC4Md5Cipher;
	char szDecryptKey[32]; // ����rc4-md5��key, ��̬���յ�iv�����ɵ�key
	char szOriginalKey[32]; // ԭʼkey, ��Ϊrc4-md5����iv��ԭʼkey������ʵkey, �������ﱣ��ԭʼkey.
public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len);
protected:
	virtual void InitializeRC4Key( BOOL bIsEncKey  /* TRUE: Enc Key, FALSE: Des Key */ );
	void GenerateRC4Md5Key( char *oukey ,char *inkey, char *iv);

protected:
	CryptoPP::Weak1::ARC4::Encryption rc4Encryption;
	CryptoPP::Weak1::ARC4::Decryption rc4Decryption;
};

/** @brief salsa20
*/
class CSalsa20Criptor :public CCryptor
{
public:
	CSalsa20Criptor( string key );
	virtual ~CSalsa20Criptor(){}

public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len);

protected:
	Salsa20::Encryption salsaEncryption;
	Salsa20::Decryption salsaDecryption;
};

class CTableCryptor : public CCryptor
{
public:
	CTableCryptor( string key );
	virtual ~CTableCryptor(){}

public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len);
};


#define SODIUM_BLOCK_SIZE   64
// ʹ��Libsodium���мӽ���
// http://doc.libsodium.org/installation/index.html
// http://doc.libsodium.org/advanced/chacha20.html
class CChaCha20Criptor : public CCryptor
{
public:
	CChaCha20Criptor( string key);
	virtual ~CChaCha20Criptor(){}

public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ���ü�����Ϣ, ������������KEY, IV 
	*/
	virtual void ResetCryption();
protected:
	u_long uEnCounter;
	u_long uDeCounter;
};

#define CHACHA20_IETF_ENC_KEY_SIZE 32
#define CHACHA20_IETF_ENC_IV_SIZE 12

// ʹ��Libsodium���мӽ���
// http://doc.libsodium.org/installation/index.html
// http://doc.libsodium.org/advanced/chacha20.html
class CChaCha20IetfCriptor : public CCryptor
{
public:
	CChaCha20IetfCriptor( string key);
	virtual ~CChaCha20IetfCriptor(){}

public:
	/** @brief ����
	*/
	virtual BOOL Encrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ����
	*/
	virtual BOOL Decrypt( char *in, int in_len, char *out , int &out_len);
	/** @brief ���ü�����Ϣ, ������������KEY, IV 
	*/
	virtual void ResetCryption();
protected:
	u_long uEnCounter;
	u_long uDeCounter;
};