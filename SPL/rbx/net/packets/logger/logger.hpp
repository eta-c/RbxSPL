/*
*	@Desc: Roblox raknet packet logger
*	@Note: Stand alone decryption will be added soon!
* 
*	@Tykind
*/
#pragma once
#include <vector>
#include <cstdint>
#include <functional>

#include "../pkg.hpp"
#include "../../../task scheduler/tasklib.hpp"
#include "../../../../utils/vt/vt.hpp"
#include "../../../../utils/err/err.hpp"

// -> Logger settings

#define HOOK_REPLICATOR 0

// -> Function definitions

using logpkg_funct = std::function<std::optional<bool>(std::string_view, pkg::pkg_status, pkg::packet_t<std::uint8_t>*, std::optional<pkg::extra_data_t*>)>;

using rakpeer_sendnetstreamref_t = std::uintptr_t(__fastcall*)(std::uintptr_t, rbx_raknet::netstream_ref*, pkg::pkg_priority, pkg::pkg_reliability,
	char, std::uintptr_t*, bool, unsigned int);

using rakpeer_sendnetstream_t = std::uintptr_t(__fastcall*)(std::uintptr_t, rbx_raknet::netstream*, pkg::pkg_priority, pkg::pkg_reliability,
	char, std::uintptr_t*, bool, unsigned int);

using rakpeer_sendrawstream_t = std::uintptr_t(__fastcall*)(std::uintptr_t, const char*, std::uint32_t, pkg::pkg_priority, pkg::pkg_reliability,
	char, std::uintptr_t*, bool, std::uint32_t);

using rakpeer_deallocate_packet_t = void(__fastcall*)(std::uintptr_t, rbx_raknet::packet*);

#if HOOK_REPLICATOR
using replicator_onreceive_t = std::uintptr_t(__fastcall*)(std::uintptr_t, rbx_raknet::packet*);
#endif

namespace pkg
{
	static bool initialized{ false };
	static std::vector<logpkg_funct> middle_man_funcs;

	// -> Vt backups
	static rakpeer_sendnetstreamref_t rakpeer_sendnetstreamref_orig;
	static rakpeer_sendnetstream_t rakpeer_sendnetstream_orig;
	static rakpeer_sendrawstream_t rakpeer_sendrawstream_orig;
	static rakpeer_deallocate_packet_t rakpeer_deallocate_packet_orig;

#if HOOK_REPLICATOR
	static replicator_onreceive_t replicator_onreceive_orig;
#endif

	namespace hook_func_types
	{
		static std::uintptr_t __fastcall rakpeer_sendnetstreamref(std::uintptr_t rawpeer, rbx_raknet::netstream_ref* netstream_ref,
			pkg_priority priority, pkg_reliability reliability, std::uint8_t orderingChannel, std::uintptr_t* systemIdentifier,
				bool broadcast, std::uint32_t forceReceiptNumber)
		{

			// -> Make packet interface
			auto packet(new packet_t);

			packet->data = netstream_ref->ref->data;
			packet->length = netstream_ref->ref->length;
			packet->ordiring_channel = orderingChannel;
			packet->priority = priority;
			packet->reliability = reliability;
			packet->system_address = systemIdentifier;

			for (auto i = 0u; i < middle_man_funcs.size(); i++)
			{
				const auto& func = middle_man_funcs[i];
				if (func("Rakpeer::Send", pkg::pkg_status::TO_SEND, packet, {}).has_value())
					middle_man_funcs.erase(middle_man_funcs.begin() + i);
			}

			// -> Clean up 

			delete packet; 
			return rakpeer_sendnetstreamref_orig(rawpeer, netstream_ref, priority, reliability, orderingChannel, systemIdentifier, broadcast, forceReceiptNumber);
		}

		static std::uintptr_t __fastcall rakpeer_sendnetstream(std::uintptr_t rawpeer, rbx_raknet::netstream* netstream,
			pkg_priority priority, pkg_reliability reliability, std::uint8_t orderingChannel, std::uintptr_t* systemIdentifier,
			bool broadcast, std::uint32_t forceReceiptNumber)
		{

			// -> Make packet interface
			auto packet(new packet_t);

			packet->data = netstream->data;
			packet->length = netstream->length;
			packet->ordiring_channel = orderingChannel;
			packet->priority = priority;
			packet->reliability = reliability;
			packet->system_address = systemIdentifier;

			for (auto i = 0u; i < middle_man_funcs.size(); i++)
			{
				const auto& func = middle_man_funcs[i];
				if (func("Rakpeer::Send", pkg::pkg_status::TO_SEND, packet, {}).has_value())
					middle_man_funcs.erase(middle_man_funcs.begin() + i);
			}

			delete packet; // -> Delete this ptr struct, roblox later releases all other things

			return rakpeer_sendnetstream_orig(rawpeer, netstream, priority, reliability, orderingChannel, systemIdentifier, broadcast, forceReceiptNumber);
		}

		static void __fastcall rakpeer_deallocate_packet(std::uintptr_t rawpeer, rbx_raknet::packet* received_packet)
		{
			// -> Our packet interface
			auto packet(new packet_t);
			auto extra_data(new extra_data_t);
			packet->data = received_packet->data;
			packet->length = received_packet->length;
			extra_data->deallocate_data = received_packet->deallocate_data;

			for (auto i = 0u; i < middle_man_funcs.size(); i++)
			{
				const auto& func = middle_man_funcs[i];
				if (func("Rakpeer::DeallocatePacket", pkg::pkg_status::RECEIVED, packet, extra_data).has_value())
					middle_man_funcs.erase(middle_man_funcs.begin() + i);
			}

			// -> Clean up
			delete packet;
			delete extra_data;
			rakpeer_deallocate_packet_orig(rawpeer, received_packet);
		}

#if HOOK_REPLICATOR
		static std::uintptr_t __fastcall replicator_onreceive(std::uintptr_t replicator, rbx_raknet::packet* received_packet)
		{
			// -> Our packet interface
			auto packet(new packet_t);
			auto extra_data(new extra_data_t);
			packet->data = received_packet->data;
			packet->length = received_packet->length;
			extra_data->deallocate_data = received_packet->deallocate_data;

			for (auto i = 0u; i < middle_man_funcs.size(); i++)
			{
				const auto& func = middle_man_funcs[i];
				if (func("Replicator::OnReceive", pkg::pkg_status::RECEIVED, packet, extra_data).has_value())
					middle_man_funcs.erase(middle_man_funcs.begin() + i);
			}

			// -> Clean up
			delete packet;
			delete extra_data;
			return replicator_onreceive_orig(replicator, received_packet);
		}
#endif
	}

	struct pkg_logger
	{
		rbx_raknet::Rakpeer* rakpeer{ nullptr };
		std::uintptr_t netprjob_ptr{ 0u };

#if HOOK_REPLICATOR
		std::uintptr_t rprocjob_ptr{ 0u };
#endif

		template<typename funcTy = logpkg_funct>
		void new_middleman(funcTy func)
		{
			middle_man_funcs.push_back(func);
		}

		pkg_logger()
		{
			if (initialized)
				return;

			netprjob_ptr = tasklib::taskscheduler.wait_for_job("Net PacketReceive");
			const auto concurrent_rakpeer = *reinterpret_cast<std::uintptr_t**> (netprjob_ptr + offsets::concurrent_rakpeer);

			if (!concurrent_rakpeer)
				throw err::err("Concurrent rakpeer at offset {} not found", offsets::concurrent_rakpeer);
			rakpeer = reinterpret_cast<rbx_raknet::Rakpeer*>(*concurrent_rakpeer);

			// -> Rakpeer hooks

			vt::vt rakpeer_vt(*concurrent_rakpeer);

			rakpeer_sendnetstreamref_orig = rakpeer_vt.replace_func<rakpeer_sendnetstreamref_t>(20,
				reinterpret_cast<std::uintptr_t> (hook_func_types::rakpeer_sendnetstreamref));

			rakpeer_sendnetstream_orig = rakpeer_vt.replace_func<rakpeer_sendnetstream_t>(21,
				reinterpret_cast<std::uintptr_t> (hook_func_types::rakpeer_sendnetstream));

			rakpeer_deallocate_packet_orig = rakpeer_vt.replace_func<rakpeer_deallocate_packet_t>(25,
				reinterpret_cast<std::uintptr_t> (hook_func_types::rakpeer_deallocate_packet));

#if HOOK_REPLICATOR
			// -> Replicator hooks
			rprocjob_ptr = tasklib::taskscheduler.wait_for_job("Replicator ProcessPackets");
			const auto replicator = *reinterpret_cast<std::uintptr_t*>(rprocjob_ptr + 0x168);
			vt::vt replicator_vt(replicator);
			
			replicator_onreceive_orig = replicator_vt.replace_func<replicator_onreceive_t>(55,
				reinterpret_cast<std::uintptr_t> (hook_func_types::replicator_onreceive));
#endif
		}
	};
}
#undef HOOK_REPLICATOR