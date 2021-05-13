/*source from Ryan Westwood (by Email) starts here: */
#pragma once
#include <condition_variable>
#include <functional>
#include <thread>
#include <vector>
#include <queue>

class ThreadPool {
public:
	using Task = std::function<void()>;

private:
	std::vector<std::thread> threads;
	std::queue<Task> tasks;

	std::condition_variable condition;
	std::mutex mutex;

	bool isRunning = false;

public:

	ThreadPool(short _threadCount) {
		Start(_threadCount);
	}

	~ThreadPool() {
		Stop();
	}

	void Enqueue(Task _newTask) {
		{
			std::unique_lock<std::mutex> lock(mutex);
			tasks.emplace(std::move(_newTask));
		}

		condition.notify_one();
	}

private:
	void Start(short _threadCount) {

		for (short i = 0; i < _threadCount; i++)
		{
			threads.emplace_back([=] {
				while (true)
				{
					Task task;
					{
						std::unique_lock<std::mutex> lock(mutex);

						condition.wait(lock, [=] { return isRunning || !tasks.empty(); });

						if (isRunning && tasks.empty()) {
							break;
						}

						task = std::move(tasks.front());
						tasks.pop();
					}

					task();
				}
			});
		}

	}

	void Stop() {
		std::unique_lock<std::mutex> lock(mutex);
		isRunning = true;

		condition.notify_all();

		for (auto& thread : threads) {
			thread.join();
		}
	}

};

/* Source from Ryan Westwood Ends Here */