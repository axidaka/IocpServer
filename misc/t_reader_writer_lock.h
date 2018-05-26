#ifndef _T_READER_WRITER_LOCK_H_
#define _T_READER_WRITER_LOCK_H_

#include <concrt.h>

template<typename TBASE>
class t_reader_writer_lock
{
	typedef Concurrency::reader_writer_lock base_reader_writer_lock;

public:

	t_reader_writer_lock()
	{}

	~t_reader_writer_lock()
	{}

	void lock_read()
	{
		m_rw.lock_read();
	}

	void try_lock_read()
	{
		m_rw.try_lock_read();
	}

	void unlock_read()
	{
		m_rw.unlock();
	}

	void lock_write()
	{
		m_rw.lock();
	}

	void try_lock_write()
	{
		m_rw.try_lock();
	}

	void unlock_write()
	{
		m_rw.unlock();
	}

	operator base_reader_writer_lock& ()
	{
		return m_rw;
	}

private:

	mutable base_reader_writer_lock m_rw;

public:

	class scoped_lock_reader
	{
	public:

		scoped_lock_reader(TBASE *pBase)
			: m_ref_rw(*pBase)
		{
			m_ref_rw.lock_read();
		}

		scoped_lock_reader(base_reader_writer_lock &rw)
			: m_ref_rw(rw)
		{
			m_ref_rw.lock_read();
		}

		~scoped_lock_reader()
		{
			m_ref_rw.unlock();
		}

	private:

		base_reader_writer_lock& m_ref_rw;

		scoped_lock_reader(const scoped_lock_reader&);                    // no copy constructor
		scoped_lock_reader const & operator=(const scoped_lock_reader&);  // no assignment operator
	};

	class scoped_lock_writer
	{
	public:

		scoped_lock_writer(TBASE *pBase)
			: m_ref_rw(*pBase)
		{
			m_ref_rw.lock();
		}

		scoped_lock_writer(base_reader_writer_lock &rw)
			: m_ref_rw(rw)
		{
			m_ref_rw.lock();
		}

		~scoped_lock_writer()
		{
			m_ref_rw.unlock();
		}

	private:

		base_reader_writer_lock& m_ref_rw;

		scoped_lock_writer(const scoped_lock_writer&);                    // no copy constructor
		scoped_lock_writer const & operator=(const scoped_lock_writer&);  // no assignment operator
	};
};

class __none__;

typedef t_reader_writer_lock<__none__> scoped_reader_writer_lock;

#endif //_T_READER_WRITER_LOCK_H_