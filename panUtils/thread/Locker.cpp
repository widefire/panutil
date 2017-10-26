#include "Locker.h"
namespace panutils {


	RWLock::RWLock() :_numReader(0)
	{
	}


	RWLock::~RWLock()
	{
	}

	void RWLock::RLock()
	{
		std::lock(_muxWriter, _muxReader);
		_numReader++;
		_muxReader.unlock();
		_muxWriter.unlock();
	}

	void RWLock::RUnlock()
	{
		std::unique_lock<std::mutex>	_(_muxReader);
		if (--_numReader == 0)
		{
			_cvReader.notify_one();
		}
	}

	void RWLock::Lock()
	{
		std::lock(_muxWriter, _muxReader);
		_cvReader.wait(_muxReader, [&] {return _numReader == 0; });
	}

	void RWLock::Unlock()
	{
		_cvReader.notify_one();
		_muxReader.unlock();
		_muxWriter.unlock();
	}


	SpinLock::SpinLock()
	{
	}


	SpinLock::~SpinLock()
	{
	}

	void SpinLock::Lock()
	{
		while (_flag.test_and_set(std::memory_order_acquire));
	}

	void SpinLock::Unlock()
	{
		_flag.clear(std::memory_order_release);
	}

}