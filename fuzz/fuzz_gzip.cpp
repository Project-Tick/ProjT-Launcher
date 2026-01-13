#include <cstdint>

#include <QByteArray>

#include "GZip.h"

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	QByteArray input(reinterpret_cast<const char*>(data), static_cast<int>(size));
	QByteArray output;
	QByteArray roundtrip;

	GZip::unzip(input, output);
	GZip::zip(output, roundtrip);

	return 0;
}
