/*
 *	@Desc: Roblox studio packet logger
 *	@Note: Log incomming/sent packets
 *	@Version: version-d71b6db47f774e36
 *
 *	@Tykind & @Bork101
 */
#include <Windows.h>
#include <cstdint>
#include <thread>

#include "utils/logger/logger.hpp"
#include "rbx/net/packets/pkg.hpp"
#include "rbx/net/packets/logger/logger.hpp"

#include "utils/err/err.hpp"

void main_thread()
{
	try
	{
		pkg::pkg_logger packet_logger;

		char password[2];
		std::uint32_t passwordLength{ 2u };

		packet_logger.rakpeer->getincommingpassword(password, &passwordLength);

		logger::logger.log("Net packetreceived -> %s\n", LOG_TO_HEX(packet_logger.netprjob_ptr));
		logger::logger.log("Rawpeer -> %s\n", LOG_TO_HEX(reinterpret_cast<std::uintptr_t> (packet_logger.rakpeer)));
		logger::logger.log("Incomming password -> % s\n", password);

		packet_logger.new_middleman([&](auto fn_name, auto status, auto packet, auto attached_data) -> std::optional<bool>
		{
			switch (status)
			{
				case(pkg::pkg_status::TO_SEND):
				{
					logger::logger.log("%s\n\tPacket piority -> %d\n\tReliability -> %d\n\tOrdering channel -> %d\n\tId -> %d\n\tType -> %d\n\tSubtype -> %d\n",
						fn_name.data(), packet->priority, packet->reliability, packet->ordiring_channel, packet->data[0], packet->data[1], packet->data[2]);

					logger::logger.log_arr(packet->data, packet->length);
					logger::logger.log_stream();
					break;
				}
				case(pkg::pkg_status::RECEIVED):
				{
					logger::logger.log("%s\n\tId -> %d\n\tType -> %d\n\tSubtype -> %d\n%s",
						fn_name.data(), packet->data[0], packet->data[1], packet->data[2], attached_data.has_value() ? attached_data.value()->deallocate_data ? "\tDeallocate data -> true\n" : "\tDeallocate data -> false\n" : "");

					logger::logger.log_arr(packet->data, packet->length);
					logger::logger.log_stream();
					break;
				}
			}
			return {};
		});
	}
	catch (err::err& err)
	{
		logger::logger.log("catched error -> {}", err.what().c_str());
	}
}

bool __stdcall DllMain(void*, std::uint32_t reason, void*)
{
	if (reason == DLL_PROCESS_ATTACH)
		std::thread{ main_thread }.detach();
	return true;
}