#include "threadpool.h"

namespace Multix
{
	unsigned int ThreadPool::s_NrThreads = 0;
	std::queue<TaskFn> ThreadPool::s_Q;
	std::vector<std::thread> ThreadPool::s_Threads;
	std::mutex ThreadPool::s_Mutex;
	std::condition_variable ThreadPool::s_Cond;

	void ThreadPool::Init(unsigned int nr_threads)
	{
		s_NrThreads = nr_threads;
		s_Threads.reserve(s_NrThreads);
		for (unsigned int i = 0; i < s_NrThreads; i++)
			s_Threads.emplace_back(&ThreadPool::Worker, i);
	}

	void ThreadPool::JoinAll()
	{
		for (unsigned int i = 0; i < s_NrThreads; i++)
			s_Threads[i].join();
	}

	void ThreadPool::Worker(int id)
	{
		while (true)
		{
			TaskFn task;
			std::unique_lock<std::mutex> lk(s_Mutex);
			if (s_Q.empty())
				s_Cond.wait(lk);
			task = s_Q.front();
			s_Q.pop();
			lk.unlock();
			task();
		}
	}
}