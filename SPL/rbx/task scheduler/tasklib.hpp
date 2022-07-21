/*
*	@Desc: Roblox taskscheduler library
*	@Note: This is old, might remake later; or not
* 
*	@Tykind
*/
#pragma once
#include <optional>
#include <functional>

#include "../../utils/logger/logger.hpp"
#include "../offsets.hpp"

namespace tasklib
{
	inline const auto singleton = offsets::get_jobs_singleton();

	class tasklib
	{
		logger::logger_t _logger;
	public:

		template<typename Ty>
		void iterate_tasks(Ty func)
		{
			while (*reinterpret_cast<int*>(singleton) < 160)
				Sleep(100);

			// -> Iterate tasks
			auto thread_pool_start = *reinterpret_cast<std::uintptr_t**>(singleton + offsets::threadpool_start);
			auto thread_pool_end = *reinterpret_cast<std::uintptr_t**>(singleton + offsets::threadpool_end);

			while (thread_pool_start != thread_pool_end)
			{
				// -> Loop thread pool
				const auto jobitem = *thread_pool_start;

				if (func(*reinterpret_cast<std::string*>(jobitem + 16), jobitem).has_value())
					break;

				thread_pool_start += 2;
			}
		}

		void log_singleton();
		void log_jobs();

		std::optional<std::uintptr_t> get_job(std::string_view); // -> thread safe
		std::uintptr_t wait_for_job(std::string_view); // -> not thread safe

		explicit tasklib(const char* debug_name = "TASKSCHEDULER") : _logger(debug_name) {}
	};
	static tasklib taskscheduler;
}