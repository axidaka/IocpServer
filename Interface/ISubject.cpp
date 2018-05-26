#include "stdafx.h"
#include "ISubject.h"
#include "IObserver.h"

ISubject::ISubject()
{

}

ISubject::~ISubject()
{

}

void ISubject::AddObserver(IObserver* pObserver)
{
	scoped_reader_writer_lock::scoped_lock_writer writer(m_rwSetObserver);
	m_setObserver.insert(pObserver);
}

void ISubject::RemoteOberver(IObserver* pObserver)
{
	scoped_reader_writer_lock::scoped_lock_writer writer(m_rwSetObserver);
	auto iter_find = m_setObserver.find(pObserver);
	if (m_setObserver.end() != iter_find)
	{
		m_setObserver.erase(iter_find);
	}
}

bool ISubject::Broadcast(const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2)
{
	scoped_reader_writer_lock::scoped_lock_reader reader(m_rwSetObserver);
	auto iter_set_end = m_setObserver.cend();
	for (auto iter_set = m_setObserver.begin(); iter_set != iter_set_end; ++ iter_set)
	{
		(*iter_set)->Notify(iMessage, pParam1, pParam2);
	}
	
	return true;
}

bool ISubject::Notisfy(IObserver* const pObserver, const NOTISY_MSG_TYPE iMessage, void* pParam1, void* pParam2)
{
	scoped_reader_writer_lock::scoped_lock_reader reader(m_rwSetObserver);
	auto iter_find = m_setObserver.find(pObserver);
	if (m_setObserver.end() != iter_find)
	{
		pObserver->Notify(iMessage, pParam1, pParam2);
	}

	return true;
}