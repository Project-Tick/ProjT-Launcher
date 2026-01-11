#include <cstdint>
#include <exception>
#include <sstream>
#include <string>

#include "io/stream_reader.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	// Avoid excessive allocations on pathological inputs
	constexpr size_t kMaxInputSize = 1 << 20; // 1 MiB
	if (!data || size == 0 || size > kMaxInputSize)
	{
		return 0;
	}

	std::string input(reinterpret_cast<const char*>(data), size);
	std::istringstream stream(input, std::ios::binary);

	try
	{
		nbt::io::read_compound(stream);
	}
	catch (const std::exception&)
	{
		// Expected for malformed inputs.
	}

	return 0;
}
