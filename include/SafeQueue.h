#pragma once

#include <queue>
#include <mutex>
#include <condition_variable>
#include <memory>

template<typename T>
class SafeQueue
{
public:
	SafeQueue() {}
	~SafeQueue() {}

	void push(T value)
	{
		std::lock_guard<std::mutex> lock(m_Mtx);
		m_Queue.emplace(std::make_shared<T>(std::move(value)));
		m_CV.notify_one();
	}

	bool try_pop(T& value)
	{
		std::lock_guard<std::mutex> lock(m_Mtx);
		if (m_Queue.empty())
			return false;
		
		value = std::move(*m_Queue.front());
		m_Queue.pop();
		return true;
	}

	void wait_pop(T& value)
	{
		std::unique_lock<std::mutex> lock(m_Mtx);
		if (m_Queue.empty())
			m_CV.wait(lock);

		value = std::move(*m_Queue.front());
		m_Queue.pop();
	}

	bool empty()
	{
		std::lock_guard<std::mutex> lock(m_Mtx);
		return m_Queue.empty();
	}
private:
	
private:
	std::queue<std::shared_ptr<T>> m_Queue;
	std::mutex m_Mtx;
	std::condition_variable m_CV;
};