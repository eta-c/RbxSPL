/*
*	@Desc: Roblox offsets
*	@Note: This aren't for the main RBX, they are meant for the studio version
* 
*	@Tykind
*/
#pragma once
#include <Windows.h>
#include <cstdint>

#include "../utils/flags.hpp"

namespace offsets
{
	inline const auto base = reinterpret_cast<std::uintptr_t>(GetModuleHandle(nullptr));

	template <class Ty = std::uintptr_t>
	Ty rebase(auto x)
	{
		return Ty((std::uintptr_t(x) - flags::dissa_base) + base);
	}

	template <class Ty = std::uintptr_t>
	Ty unbase(auto x)
	{
		return Ty((std::uintptr_t(x) - base) + flags::dissa_base);
	}


	// -> Raw offsets
	inline constexpr auto threadpool_start{ 376 };
	inline constexpr auto threadpool_end{ 384 };

	inline constexpr auto concurrent_rakpeer{ 376 };

	enum rakpeer_vtoffsets
	{
		SEND_NETSTREAM = 20,
		SEND_BITSTREAM = 21,
		SEND_RAW = 22
	};

	// -> Functions

	inline const auto get_jobs_singleton = reinterpret_cast<std::int64_t(*)()>(rebase(0x14219BB90)); // isRunning
}

namespace rbx_raknet
{
	class Rakpeer
	{
	public:

		virtual void Function0();
		virtual void Function1();
		virtual void Function2();
		virtual void Function3();
		virtual void Function4();
		virtual void Function5();
		virtual void Function6();
		virtual void Function7();
		virtual void Function8();
		virtual void Function9();
		virtual void setincommingpassword(char*, std::uint32_t*);
		virtual void getincommingpassword(char*, std::uint32_t*);
		virtual void connect();
		virtual void connect_with_socket();
		virtual void shutdown();
		virtual void Function15();
		virtual void Function16();
		virtual void Function17();
		virtual void increment_next_receipt();
		virtual void increment_next_send_receipt();
		virtual std::int64_t sendnetstream(std::uintptr_t, int, int, char, std::uintptr_t*, bool, unsigned int);
		virtual std::int64_t sendbitstream(std::uintptr_t, int, int, char, std::uintptr_t*, bool, unsigned int);
		virtual std::int64_t sendraw(const char*, std::uint32_t, int, int, char, std::uintptr_t*, bool, unsigned int);
		virtual std::int64_t send_loopback(const void*, int);
		virtual std::int64_t receive(bool);
		virtual void deallocate_packet(std::uint64_t);
		virtual void Function26();
		virtual void close_connection();
	};

	struct packet
	{
		char pad_0000[48]; //0x0000
		uint32_t length; //0x0030
		char pad_0034[4]; //0x0034
		uint8_t* data; //0x0038
		bool deallocate_data; //0x0040
	};

	struct netstream
	{
		char pad_0000[16];
		uint8_t* data;
		char pad_0018[24];
		uint32_t length;
		char pad_0034[88];
	};

	struct netstream_ref
	{
		struct netstream* ref;
	};
}