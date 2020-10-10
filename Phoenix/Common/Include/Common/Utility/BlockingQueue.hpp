// Copyright 2019-20 Genten Studios
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
// this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
// this list of conditions and the following disclaimer in the documentation
// and/or other materials provided with the distribution.
//
// 3. Neither the name of the copyright holder nor the names of its contributors
// may be used to endorse or promote products derived from this software without
// specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#pragma once

#include <Common/Utility/Internal/Front.hpp>

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

namespace phx
{
	// namespace
	//{
	template <typename T>
	class IsValid
	{
	public:
		bool operator()(const T&) { return true; }
	};
	//} // namespace

	/**
	 * @brief A thread safe queue
	 *
	 * @tparam T The type of object stored in the queue
	 * @tparam Validator
	 * @tparam Container
	 */
	template <typename T, typename Validator = IsValid<T>,
	          typename Container = std::queue<T>>
	class BlockingQueue : public Container
	{
	public:
		virtual ~BlockingQueue() { stop(); }

		void stop()
		{
			m_done = true;
			m_cond.notify_all();
			m_mutex.lock();
			m_mutex.unlock();
		}
		void clear()
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			Container::c.clear();
		}

		/**
		 * @brief checks if the queue is empty
		 * @return true if empty
		 * @return false if not empty
		 */
		bool empty() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done)
			{
				return true;
			}
			bool res = Container::empty();
			return res;
		}

		/**
		 * @brief Gets the size of the queue
		 *
		 * @return How many elements are in the queue
		 */
		size_t size() const
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done)
			{
				return 0;
			}
			auto res = Container::size();
			return res;
		}

		/**
		 * @brief Removes an element from the end of the queue.
		 *
		 * @return the removed element
		 */
		T pop()
		{
			std::unique_lock<std::mutex> lock_cond(m_mutex);
			if (m_done)
			{
				return {};
			}
			while (true)
			{
				m_cond.wait(lock_cond,
				            [this] { return !Container::empty() || m_done; });
				if (m_done)
				{
					return {};
				}
				T value =
				    phx::front<Container>(*this); // for std::priority_queue
				Container::pop();
				if (validator(value))
				{
					return value;
				}
			}
		}

		bool try_pop(T& _value)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done)
			{
				return false;
			}
			if (Container::empty())
			{
				return false;
			}
			_value = std::move(phx::front<Container>(*this));
			Container::pop();
			return true;
		}

		/**
		 * @brief pushes an element to the front of the queue
		 *
		 * @param value The element to be pushed to the queue
		 */
		void push(const T& value)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done)
			{
				return;
			}
			bool unlock = false;
			if (Container::empty())
			{
				unlock = true;
			}
			Container::push(value);
			if (unlock)
			{
				m_cond.notify_one();
			}
		}
		void push(T&& value)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done)
			{
				return;
			}
			bool unlock = false;
			if (Container::empty())
			{
				unlock = true;
			}
			Container::push(std::move(value));
			if (unlock)
			{
				m_cond.notify_one();
			}
		}

		template <class... Args>
		void emplace(Args&&... args)
		{
			std::lock_guard<std::mutex> lock(m_mutex);
			if (m_done)
			{
				return;
			}
			bool unlock = false;
			if (Container::empty())
			{
				unlock = true;
			}
			Container::emplace(std::forward(args)...);
			if (unlock)
			{
				m_cond.notify_one();
			}
		}

	public: // STL implementation of constructors
		explicit BlockingQueue(const Container& cont) : Container(cont) {}

		explicit BlockingQueue(Container&& cont = Container())
		    : Container(std::move(cont))
		{
		}

		BlockingQueue(const BlockingQueue& other) : Container(other) {}

		BlockingQueue(BlockingQueue&& other) : Container(std::move(other)) {}

		template <class Alloc>
		explicit BlockingQueue(const Alloc& alloc) : Container(alloc)
		{
		}

		template <class Alloc>
		BlockingQueue(const Container& cont, const Alloc& alloc)
		    : Container(cont, alloc)
		{
		}

		template <class Alloc>
		BlockingQueue(Container&& cont, const Alloc& alloc)
		    : Container(std::move(cont), alloc)
		{
		}

		template <class Alloc>
		BlockingQueue(const BlockingQueue& other, const Alloc& alloc)
		    : Container(other, alloc)
		{
		}

		template <class Alloc>
		BlockingQueue(BlockingQueue&& other, const Alloc& alloc)
		    : Container(other, alloc)
		{
		}

		BlockingQueue& operator=(const BlockingQueue& _queue)
		{
			// std::lock_guard<std::mutex> lock(m_mutex);
			return *this;
		}

		BlockingQueue& operator=(BlockingQueue&& _queue)
		{
			// std::lock_guard<std::mutex> lock(m_mutex);
			// std::lock_guard<std::mutex> lock2(_queue.m_mutex);
			return *this;
		}

		void lock() const { m_mutex.lock(); }

		void unlock() const { m_mutex.unlock(); }

	protected:
		mutable std::mutex m_mutex;

	private:
		std::condition_variable m_cond;
		std::atomic<bool>       m_done = false;
		Validator               validator;
	};
	template <typename T, typename Validator, class Compare,
	          typename Container = std::vector<T>>
	using PriorityBlockingQueue =
	    BlockingQueue<T, Validator, std::priority_queue<T, Container, Compare>>;
} // namespace phx