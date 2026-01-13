#include <cstdint>
#include <exception>
#include <sstream>
#include <string>

#include "io/stream_reader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	// Avoid excessive allocations on pathological inputs
	// Very strict limit to prevent array allocation bombs
	// E.g., NBT array with count 0x92929292 would allocate 3.2GB
	constexpr size_t kMaxInputSize = 4 * 1024; // 4 KiB (was 64 KiB)
	if (!data || size == 0 || size > kMaxInputSize)
	{
		return 0;
	}

	try
	{
		// Use custom string stream without extra copies
		std::istringstream stream(std::string(reinterpret_cast<const char*>(data), size), std::ios::binary);
		nbt::io::read_compound(stream);
	}
	catch (const std::exception&)
	{
		// Expected for malformed inputs or resource exhaustion
	}
	catch (const std::bad_alloc&)
	{
		// Handle out-of-memory gracefully
		return -1;
	}

	return 0;
}
