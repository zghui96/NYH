#include "pch.h"
#include "PacketReader.h"
#include "Log.h"

PacketReader::PacketReader(ifstream* ifFile)
{
	m_fileStream.push_back(ifFile);
}

PacketReader::PacketReader(const string src)
{
	addFile(src);
}

PacketReader::PacketReader()
{
}

PacketReader::~PacketReader()
{
	close();
}

void PacketReader::addFile(const string src)
{
	ifstream* ifFile = new ifstream;
	ifFile->open(src, ios_base::in | ios_base::binary);
	if (ifFile->fail()) {
		Log::error("���ݰ���ȡ�ļ���ʧ��!");
		return;
	}
	m_fileStream.push_back(ifFile);
}

void PacketReader::close()
{
	//����ļ���
	while (!m_fileStream.empty()) {
		ifstream* f = m_fileStream.front();
		f->close();
		f->clear();
		m_fileStream.pop_front();
		delete f;
		f = NULL;
	}
}
