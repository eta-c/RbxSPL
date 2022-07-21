/*
*	@Desc: Error struct with formatting
*	@Note: This requires C++ 20
* 
*	@Tykind
*/
#pragma once
#include <string>
#include <format>

namespace err
{
	class err
	{
		std::string _message;
	public:
		
		auto what()
		{
			return _message;
		}

		template<typename ...Args>
		err(std::string_view msg, Args ...args)
		{
			_message = std::vformat(msg, std::make_format_args(args...));
		}
	};
}