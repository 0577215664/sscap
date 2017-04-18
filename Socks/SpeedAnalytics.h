#pragma once

/** @brief ����ͳ��SOCKET�����ٶ�
*/
#define MAX_AVE_SPEED 2

class CSpeedAnalytics
{
public:
	CSpeedAnalytics();
	virtual ~CSpeedAnalytics();
	void reset();
	void PushData(int len);
	/** @brief ����bytes/s���ٶ�*/
	unsigned int GetSpeed();
	/** @brief ����kb/s���ٶ�*/
	unsigned int GetKBSpeed();
protected:
	unsigned int m_lastspeed;
	unsigned int m_lastspeedval;
	unsigned int m_firstts;
	unsigned int m_lastts;
	unsigned int m_counting[MAX_AVE_SPEED];
};
