#include<iostream>
#include<queue>
#include<chrono>
#include<mutex>
#include<functional>
#include"../booty/concurrency/UnboundedLockQueue.hpp"

using namespace booty;
using namespace std;
using namespace std::chrono;

void single_thread() {
	auto start1 = system_clock::now();
	concurrency::UnboundedLockQueue<int> queue;
	for (int i = 0; i < 1000000; ++i) {
		queue.enqueue(0);
	}

	for (int i = 0; i < 100000; ++i) {
		int j;
		queue.dequeue(j);
		//std::cout << data << ' ';
	}

	std::cout << std::endl;
	auto end1 = system_clock::now();
	auto start2 = system_clock::now();
	std::mutex mtx;
	std::queue<int> queue1;
	for (int i = 0; i < 1000000; ++i) {
		{
			lock_guard<std::mutex> lock(mtx);
			queue1.push(i);
		}
	}

	for (int i = 0; i < 100000; ++i) {
		std::mutex mtx;
		lock_guard<std::mutex> lock(mtx);
		queue1.pop();
		//std::cout << data << ' ';
	}
	std::cout << std::endl;
	auto end2 = system_clock::now();
	auto dur1 = duration_cast<microseconds>(end1 - start1);
	auto dur2 = duration_cast<microseconds>(end2 - start2);
	std::cout << "Test one took "
		<< double(dur1.count()) * microseconds::period::num / microseconds::period::den
		<< "s" << std::endl;
	std::cout << "Test two took "
		<< double(dur2.count()) * microseconds::period::num / microseconds::period::den
		<< "s" << std::endl;
}

void multi_thread() {
	concurrency::UnboundedLockQueue<int> queue;
	vector<thread> threads;
	for (int n = 0; n < 5; ++n)
		threads.emplace_back([&queue] {
		for (int i = 0; i < 1000; ++i) {
			queue.enqueue(i);
		}
	});
	for (auto& thread : threads)
		thread.join();
	cout << "Finish enqueue and start dequeue." << endl;
	std::this_thread::sleep_for(std::chrono::microseconds(500));
	vector<thread> other_threads;
	for (int n = 0; n < 5; ++n)
		other_threads.emplace_back([&queue] {
		for (int i = 0; i < 1000; ++i) {
			int m;
			queue.dequeue(m);
		}
	});
	for (auto& thread : other_threads)
		thread.join();
	cout << "Finish dequeue." << endl;
}


int main() {
	using namespace std::chrono;
	cout << "START!!!!" << endl;
	/*for (int i = 0; i < 10; ++i) {
		multi_thread();
		std::this_thread::sleep_for(500ms);
	}*/
	single_thread();
	cout << "FINISH!!!!" << endl;
	return 0;
}