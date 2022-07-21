/*
*	@Desc: Virtual table utilities
*	@Note: WIP
* 
*	@Tykind
*/
#pragma once
#include <Windows.h>
#include <cstdint>

#include <map>

namespace vt
{
	template<typename Ty = std::uintptr_t>
	class vt final
	{
		Ty* vft{nullptr};
		Ty* back_up{ nullptr };

		std::map<std::uint32_t, Ty> funcs_hooked;
	public:

		template<typename RetType = std::uintptr_t>
		RetType replace_func(std::uint32_t idx, Ty func)
		{
			RetType ret{ 0u };
			unsigned long old;
			VirtualProtect(&vft[idx], sizeof(Ty), PAGE_READWRITE, &old);
			ret = reinterpret_cast<RetType>(vft[idx]);
			funcs_hooked[idx] = vft[idx];
			vft[idx] = func;
			VirtualProtect(&vft[idx], sizeof(Ty), old, &old);
			return ret;
		}

		void return_func(std::uintptr_t idx)
		{
			const auto backed_ptr = funcs_hooked.find(idx);
			if (backed_ptr != funcs_hooked.end())
			{
				funcs_hooked.erase(backed_ptr);
				replace_func(idx, backed_ptr->second);
			}
		}

		vt() = delete;
		explicit vt(Ty instance) {
			vft = *reinterpret_cast<Ty**>(instance);
		}
	};
}