#ifndef BOOTY_CONCURRENCY_UNBOUNDEDLOCKQUEUE_H
#define BOOTY_CONCURRENCY_UNBOUNDEDLOCKQUEUE_H


#include<queue>
#include<mutex>
#include<atomic>
#include<condition_variable>

#include"../base/Base.h"

namespace booty {

	namespace concurrency {

		/// UnboundedLockQueue provides thread-safe operations with STL lock.
		/// enqueue/dequeue never blocks. If neccessary, block measures should
		/// be adopted by caller/invoker.

		template<typename T>
		class UnboundedLockQueue :public NonCopyable {
		public:
			UnboundedLockQueue() {}

			/* enqueue one element(lvalue). */
			void enqueue(const T& ele) {
				std::lock_guard<std::mutex> lock(queue_mtx_);
				queue_.push(ele);
				if (size() == 1) // queue is empty before push().
					cond_.notify_one();
			}

			/* enqueue one element(rvalue). */
			void enqueue(T&& ele) {
				std::lock_guard<std::mutex> lock(queue_mtx_);
				queue_.emplace(std::move(ele));
				if (size() == 1) // queue is empty before push().
					cond_.notify_one();
			}

			/*
			  dequeue one element, pass by reference.
			  return by a local value is not exception-safe, and when queue is empty,
			  it just throw an error by std::queue, but never block to wait for producer,
			  as it mentioned above: block measures should be adopted by caller/invoker.
			*/
			void dequeue(T& recv) {
				std::unique_lock<std::mutex> lock(queue_mtx_);
				if (empty()) {
					cond_.wait(lock, [this] {
						return !queue_.empty();
					});
				}
				recv = std::move(queue_.front());
				queue_.pop();
			}

			/* get size of queue */
			size_t size() const {
				return queue_.size();
			}

			/* judge if queue is empty */
			bool empty() const {
				return queue_.empty();
			}

		private:
			std::queue<T> queue_;
			std::mutex queue_mtx_;
			std::condition_variable cond_;
		};
	}
}

#endif // !BOOTY_CONCURRENCY_UNBOUNDEDLOCKQUEUE_H
