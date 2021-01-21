#pragma once

#include "common.h"
#include <list>
#include <fstream>

using namespace std;

/**
*	���ݰ���ȡ��
*/
class PacketReader
{
protected:
	list<ifstream*>  m_fileStream;		//TRACE�ļ���

public:
	PacketReader(ifstream* ifFile);
	PacketReader(const string src);
	PacketReader();
	virtual ~PacketReader();

	/**��ȡһ�����ݰ�
	* @param ��ȡ�������ݰ�
	* @return ���ݰ���ȡ״̬����ȡ�ɹ�Ϊtrue
	*/
	virtual bool readPacket(Packet& pkt) = 0;

	/**���һ�����ݰ��ļ�
	* @param �ļ�·��
	* @���ݰ���ȡ״̬����ȡ�ɹ�Ϊtrue
	*/
	virtual void addFile(const string src);

	/**�ر��ļ���*/
	virtual void close();
};

