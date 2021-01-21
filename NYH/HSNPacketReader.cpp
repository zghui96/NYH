#include "pch.h"
#include "HSNPacketReader.h"
#include "Log.h"

HSNPacketReader::HSNPacketReader(ifstream* ifFile) :PacketReader(ifFile)
{
}

HSNPacketReader::HSNPacketReader(const string src) : PacketReader(src)
{
}

HSNPacketReader::HSNPacketReader()
{
}

HSNPacketReader::~HSNPacketReader()
{
}

/**��ȡһ�����ݰ�
* @param ��ȡ�������ݰ�
* @return ��ȡ�����ݰ���״̬
*/
bool HSNPacketReader::readPacket(Packet& pkt)
{
	while (!m_fileStream.empty()) {
		ifstream* f = m_fileStream.front();
		while (!f->eof())
		{
			bool bRet = readPacket(f, pkt);
			if (!bRet)
			{
				//Log::error("���ݰ���ȡ����!");
				continue;
			}
			return true;
		}
		f->close();
		f->clear();
		m_fileStream.pop_front();
		delete f;
		f = NULL;
	}
	return false;
}

bool HSNPacketReader::readPacket(ifstream* ifFile, Packet& pkt)
{
	//��ȡ���ݰ���ʱ���
	ifFile->read((char*)&(pkt.time), sizeof(pkt.time));
	if (!ifFile)
	{
		TRACE("read timestamp failure!\n");
		return false;
	}

	//��ȡIP�ײ�����
	IpHeader* pIpHeader = (IpHeader*) new char[sizeof(IpHeader)];
	if (NULL == pIpHeader)
	{
		TRACE("new array error!");
		return false;
	}

	ifFile->read((char*)pIpHeader, sizeof(IpHeader));
	if (!ifFile)
	{
		TRACE("read IP header failure!\n");
		return false;
	}

	//��ȡIP�ײ��ؼ��ֶ���Ϣ
	USHORT usIpHdrLen = (pIpHeader->ver_ihl & 0x0f) * 4;//��ȡIPͷ������
	USHORT usIpTotalLen = ntohs(pIpHeader->total_len); 	//��ȡIP���ܳ���
	pkt.src.ip = ntohl(pIpHeader->srcIP);
	pkt.dst.ip = ntohl(pIpHeader->dstIP);
	pkt.proto = pIpHeader->protocol;

	delete[] pIpHeader;
	pIpHeader = NULL;

	//����IPѡ�����
	if (usIpHdrLen > 20)
	{
		ifFile->seekg(4, ios_base::cur);
		if (!ifFile)
		{
			TRACE("seek file failure!\n");
			return false;
		}
	}

	//����Э���ֶ�������ȡ���������
	if (pkt.proto == PROTO_TCP)
	{
		//��ȡTCP�ײ�����
		TcpHeader* pTcpHeader = (TcpHeader*) new char[sizeof(TcpHeader)];
		if (NULL == pTcpHeader)
		{
			TRACE("new array error!");
			return false;
		}

		ifFile->read((char*)pTcpHeader, sizeof(TcpHeader));
		if (!ifFile)
		{
			TRACE("read TCP header failure!\n");
			return false;
		}

		//��ȡ�˿ں�
		pkt.src.port = ntohs(pTcpHeader->src_port);
		pkt.dst.port = ntohs(pTcpHeader->dst_port);

		delete[] pTcpHeader;
		pTcpHeader = NULL;
	}
	else if (pkt.proto == PROTO_UDP)
	{
		//��ȡUDP�ײ�����
		UdpHeader* pUdpHeader = (UdpHeader*)new char[sizeof(UdpHeader)];
		if (NULL == pUdpHeader)
		{
			TRACE("new array error!");
			return false;
		}

		ifFile->read((char*)pUdpHeader, sizeof(UdpHeader));
		if (!ifFile)
		{
			TRACE("read UDP Header Failure!\n");
			return false;
		}

		//��ȡ�˿ں�
		pkt.src.port = ntohs(pUdpHeader->src_port);
		pkt.dst.port = ntohs(pUdpHeader->dst_port);

		delete[] pUdpHeader;
		pUdpHeader = NULL;
	}
	else	//��TCP��UDP����GRE(47)
	{
		ifFile->seekg(8, ios_base::cur);
		if (!ifFile)
			TRACE("seek file failure!\n");

		return false;
	}

	return true;
}
