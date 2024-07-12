#pragma once

#include <queue>
#include <functional>
#include <thread>
#include <vector>
#include <iostream>
#include <condition_variable>

namespace Multix {
	using TaskFn = std::function<void(void)>;

	class ThreadPool
	{
	public:
		ThreadPool() = delete;
		~ThreadPool() = delete;

		static void Init(unsigned int nr_threads);
		static void JoinAll();

		template<class F, class... Args>
		static void AddTask(F&& f, Args&&... args);

		inline static unsigned int NrThreads() { return s_NrThreads; }
	private:
		static void Worker(int id);
	private:
		static unsigned int s_NrThreads;
		static std::queue<TaskFn> s_Q;
		static std::vector<std::thread> s_Threads;
		static std::mutex s_Mutex;
		static std::condition_variable s_Cond;
	};

	template<class F, class... Args>
	void ThreadPool::AddTask(F&& f, Args&&... args)
	{
		std::unique_lock<std::mutex> lk(s_Mutex);
		s_Q.emplace([&] {
			f(std::forward<Args>(args)...);
		});
		s_Cond.notify_one();
		lk.unlock();
	}
}