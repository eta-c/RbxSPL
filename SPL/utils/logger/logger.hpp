/*
*	@Desc: Better logger
*	@Note: Colors will be added
* 
*	@Tykind
*/
#pragma once
#include <windows.h>
#include <iostream>
#include <sstream>
#include <format>
#include <chrono>
#include <optional>

#include "../flags.hpp"

#define LOG_TO_HEX(x)  std::format("\x1B[33m\x1B[1m{:X}\x1B[22m\x1B[39m", x).c_str()

static auto allocated_console{ false };
namespace logger
{
	class logger_t
	{
		const char* _debug_name{ "LOG" };

		auto get_time()
		{
			auto now = std::chrono::system_clock::now();
			auto start_of_day = std::chrono::floor<std::chrono::days>(now);
			auto time_from_start_day = std::chrono::round<std::chrono::seconds>(now - start_of_day);

			std::chrono::hh_mm_ss hms{ time_from_start_day };

			return std::format("{}", hms);
		}

	public:
		std::stringstream stream;
		
		void log_arr(std::uint8_t* data, std::uint32_t sz, bool raw = false)
		{
			for (auto i = 0u; i < sz; i++)
			{
				if (!raw)
					std::cout << std::hex << static_cast<int>(data[i]) << " ";
				else
					std::cout << data[i];
			}
			std::cout << std::endl;
		}

		void log_stream(bool raw = false, bool dont_clear = false)
		{
			std::uint8_t c;
			while (stream >> c)
			{
				if (!raw)
					std::cout << std::hex << static_cast<int>(c) << " ";
				else
					std::cout << c;
			}
			if (!dont_clear)
				stream.clear();
			std::cout << std::endl;
		}

		template<typename ...Args>
		void log(const char* str, Args ...args)
		{
			std::printf("[\x1B[90m\x1B[2m%s\x1B[22m\x1B[39m] [\x1B[92m\x1B[1m%s\x1B[22m\x1B[39m] - ", get_time().c_str(), _debug_name);
			std::printf(str, std::forward<Args>(args)...);
		}

		logger_t() = delete;
		explicit logger_t(const char* debug_name, std::optional<std::string_view> console_title = std::nullopt) : _debug_name(debug_name)
		{
			if (!allocated_console)
			{
				AllocConsole();
				FILE* file_stream;
				freopen_s(&file_stream, "CONIN$", "r", stdin);
				freopen_s(&file_stream, "CONOUT$", "w", stdout);
				freopen_s(&file_stream, "CONOUT$", "w", stderr);

				// -> Fucking windows and their updates
#ifdef ENABLE_VIRTUAL_TERMINAL_PROCESSING
				HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				DWORD dwMode = 0;
				GetConsoleMode(hOut, &dwMode);
				dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
				SetConsoleMode(hOut, dwMode);
#endif
				allocated_console = false;
			}

			if (console_title.has_value())
				SetConsoleTitleA(console_title->data());
		}
	};

	static logger::logger_t logger("LOG", flags::application_name);
}