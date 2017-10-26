#pragma once
#include <atomic>
#include <mutex>
#include <condition_variable>
namespace panutils {
	class RWLock
	{
	public:
		RWLock();
		~RWLock();

		void RLock();
		void RUnlock();
		void Lock();
		void Unlock();
	private:
		int _numReader;
		std::mutex	_muxReader;
		std::mutex _muxWriter;
		std::condition_variable_any _cvReader;
	};

	class SpinLock
	{
	public:
		SpinLock();
		~SpinLock();
		void Lock();
		void Unlock();
	private:
		std::atomic_flag _flag = ATOMIC_FLAG_INIT;
	};

}

