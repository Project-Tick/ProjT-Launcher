#include <cstdint>

#include <QJsonDocument>
#include <QJsonParseError>

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	if (!data || size == 0)
	{
		return 0;
	}

	QJsonParseError error{};
	QJsonDocument::fromJson(QByteArray(reinterpret_cast<const char*>(data), static_cast<int>(size)), &error);

	return 0;
}
