#include "tasklib.hpp"

void tasklib::tasklib::log_singleton()
{
	_logger.log("Taskscheduler singleton %s\n", LOG_TO_HEX(singleton));
}

void tasklib::tasklib::log_jobs()
{
	iterate_tasks([&](auto name, auto ptr) -> std::optional<bool> {
		_logger.log("Job \"%s\" : %s\n", name.c_str(), LOG_TO_HEX(ptr));
		return {};
	});
}


std::optional<std::uintptr_t> tasklib::tasklib::get_job(std::string_view job_name)
{
	std::optional<std::uintptr_t> job_ptr;

	iterate_tasks([&](auto name, auto ptr) -> std::optional<bool> {
		if (job_name == name)
		{
			job_ptr = std::make_optional<std::uintptr_t>(ptr);
			return true;
		}
		return {};
	});
	
	return job_ptr;
}

std::uintptr_t tasklib::tasklib::wait_for_job(std::string_view job_name)
{
	std::uintptr_t job_ptr{ 0u };

	std::optional<std::uintptr_t> obtained_job;
	while (!job_ptr)
	{
		if ((obtained_job = get_job(job_name)).has_value())
		{
			job_ptr = obtained_job.value();
			break;
		}
		
		Sleep(100);
	}
	return job_ptr;
}