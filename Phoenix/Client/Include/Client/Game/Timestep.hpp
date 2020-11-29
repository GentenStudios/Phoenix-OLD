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

namespace phx::client
{
	/**
	 * @brief Simple Timestep handler.
	 *
	 * This class is essentially a simple timer. Everytime "step" is called, the
	 * delta-time will be set to the difference in time since the last call of
	 * "step". You can pass this as a parameter into something that requires a
	 * float and it will automatically cast to it - although it's more ideal to
	 * just pass ``const Timestep&`` as it provides access to the other helper
	 * methods.
	 *
	 * The clear method can be used to reset the timer. This is useful if you
	 * have a stall in the program - such as a system taking too long to
	 * process. If the time is too large or you know there is an issue - just
	 * calling clear can get rid of the really long time that will be
	 * calculated.
	 *
	 * @paragraph Usage
	 * @code
	 * Timestep timer;
	 * while (window->isRunning())
	 * {
	 *     // set things up.
	 *
	 *     const Timestep& dt = timer.step();
	 *     for (system : systems)
	 *     {
	 *         system.onUpdate(dt);
	 *     }
	 *
	 *     world.render(dt);
	 * }
	 * @endparagraph
	 *
	 * @note If the underlying SDL timing system is not good enough, the system
	 * can be migrated to use std::chrono without changing any interface
	 * compatibility.
	 */
	class Timestep
	{
	public:
		/**
		 * @brief Sets up the Timestep ready to step.
		 *
		 * This constructor initializes m_last to the current ticks since
		 * program start using SDL's timing system.
		 */
		Timestep();

		/**
		 * @brief Steps the timer (updating delta-time).
		 * @return The timestep object, castable to float.
		 */
		Timestep& step();

		/**
		 * @brief Clears the timer's current count.
		 *
		 * This method is useful if a system takes too long or something similar
		 * which would mess up timings on future steps/updates.
		 */
		void clear();

		// will return seconds.
		operator float() const { return m_time; }

		/**
		 * @brief Gets the delta-time since last step in seconds.
		 * @return Delta time in seconds.
		 */
		float getSeconds() const { return m_time; }

		/**
		 * @brief Gets the delta-time since last step in milliseconds.
		 * @return Delta time in milliseconds.
		 */
		float getMilliseconds() const { return m_time * 1000; }

		/**
		 * @brief Gets the delta-time since last step in microseconds.
		 * @return Delta time in microseconds.
		 */
		float getMicroseconds() const { return m_time * 1000000; }

	private:
		float m_time = 0.f;
		float m_last = 0.f;
	};

	/**
	 * @brief Fixed timer for System updates.
	 *
	 * This timer is a more complex version of the simple Timestep. It utilizes
	 * an "accumulator" to hold the time since the last update. Once that time
	 * is greater than or equal to the timestep - you should ideally call the
	 * update() function on the object and update all your classes using the
	 * fixed timestep. Every time you call step, the accumulator will be
	 * increased by the amount that the delta-time between stepping is.
	 *
	 * @paragraph Usage
	 * @code
	 * FixedTimestep timer(FixedTimestep::FPS_60);
	 *
	 * while (window->isRunning())
	 * {
	 *     while (timer.shouldUpdate())
	 *     {
	 *         timer.update()
	 *
	 *         for (system : systems)
	 *         {
	 *             // do this or just pass timer for them to call getTimestep.
	 *             system.onUpdate(timer.getTimestep());
	 *         }
	 *     }
	 *
	 *     renderer->render(timer.step());
	 */
	class FixedTimestep
	{
	public:
		static constexpr float FPS_60 = 0.016666667f;
		
	public:
		explicit FixedTimestep(float timestep);

		/**
		 * @brief Steps the timer (calculates new delta time).
		 * @return The timestep object, castable to float.
		 *
		 * This method works just like the simple Timestep. Just increments the
		 * accumulator and returns the current delta-time. This lets this timer
		 * be used for everything else like normal as well.
		 */
		FixedTimestep& step();
		
		/**
		 * @brief Clears the timer's current count.
		 *
		 * This method is useful if a system takes too long or something similar
		 * which would mess up timings on future steps/updates. It also resets
		 * the accumulator. This should probably not be used as much in the
		 * context of this class since your systems would keep updating until
		 * the accumulator has been emptied - but sometimes such errors can
		 * occur where this method will come in handy.
		 */
		void clear();

		/**
		 * @brief Gets if enough time has passed (the timestep) to update.
		 * @return Whether the systems should update or not.
		 */
		bool  shouldUpdate() const;

		/**
		 * @brief Updates the accumulator to factor in that you've updated.
		 */
		void  update();

		/**
		 * @brief Gets the time that should be used in update functions.
		 * @return The timestep provided at construction.
		 */
		float getTimestep() const;

		operator float() const { return m_time; }

		/**
		 * @brief Gets the delta-time since last step in seconds.
		 * @return Delta time in seconds.
		 */
		float getSeconds() const { return m_time; }

		/**
		 * @brief Gets the delta-time since last step in milliseconds.
		 * @return Delta time in milliseconds.
		 */
		float getMilliseconds() const { return m_time * 1000; }

		/**
		 * @brief Gets the delta-time since last step in microseconds.
		 * @return Delta time in microseconds.
		 */
		float getMicroseconds() const { return m_time * 1000000; }

	private:
		float m_timestep;
		float m_last;
		
		float m_accumulator = 0.f;
		float m_time = 0.f;
	};
} // namespace phx::client
