#pragma once

#include <string>
#include <fstream>
#include <list>
#include "PacketReader.h"

using namespace std;

class HSNPacketReader : public PacketReader
{
public:
	HSNPacketReader(ifstream* ifFile);
	HSNPacketReader(const string src);
	HSNPacketReader();

	~HSNPacketReader();

	/**��ȡһ�����ݰ�
	* @param ��ȡ�������ݰ�
	* @return ���ݰ���ȡ״̬����ȡ�ɹ�Ϊtrue
	*/
	bool readPacket(Packet& pkt);

private:
	bool readPacket(ifstream* ifFile, Packet& pkt);
};

