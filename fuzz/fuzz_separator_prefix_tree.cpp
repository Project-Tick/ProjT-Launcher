#include <cstddef>
#include <cstdint>
#include <cstdlib>

#include <QByteArray>
#include <QString>
#include <QStringList>

#include "SeparatorPrefixTree.h"

namespace {
class InputCursor
{
  public:
	InputCursor(const uint8_t* data, size_t size) : m_data(data), m_size(size) {}

	bool has(size_t count) const
	{
		return m_pos + count <= m_size;
	}

	uint8_t nextByte()
	{
		if (!has(1))
			return 0;
		return m_data[m_pos++];
	}

	QByteArray takeBytes(size_t count)
	{
		if (!has(count))
			count = m_size - m_pos;
		QByteArray out(reinterpret_cast<const char*>(m_data + m_pos), static_cast<int>(count));
		m_pos += count;
		return out;
	}

	QString nextPath()
	{
		static const char kAlphabet[] = "abcdefghijklmnopqrstuvwxyz0123456789-_.";
		const int alphabet_len = static_cast<int>(sizeof(kAlphabet) - 1);

		const int segments = 1 + (nextByte() % 4);
		QStringList parts;
		parts.reserve(segments);

		for (int i = 0; i < segments; ++i) {
			const int length = 1 + (nextByte() % 8);
			QByteArray bytes;
			bytes.reserve(length);
			for (int j = 0; j < length; ++j) {
				bytes.append(kAlphabet[nextByte() % alphabet_len]);
			}
			parts.append(QString::fromLatin1(bytes));
		}

		return parts.join('/');
	}

  private:
	const uint8_t* m_data = nullptr;
	size_t m_size = 0;
	size_t m_pos = 0;
};
} // namespace

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
	InputCursor cursor(data, size);
	SeparatorPrefixTree<'/'> tree;

	const size_t max_ops = size < 256 ? size : 256;
	for (size_t i = 0; i < max_ops && cursor.has(1); ++i) {
		const uint8_t op = cursor.nextByte() % 8;
		const QString path = cursor.nextPath();

		switch (op) {
			case 0:
				tree.insert(path);
				break;
			case 1:
				tree.remove(path);
				break;
			case 2: {
				const bool contains = tree.contains(path);
				if (contains && !tree.covers(path)) {
					abort();
				}
				break;
			}
			case 3:
				tree.covers(path);
				break;
			case 4: {
				const bool covers = tree.covers(path);
				const QString cover = tree.cover(path);
				if (covers && cover.isNull()) {
					abort();
				}
				break;
			}
			case 5:
				tree.exists(path);
				break;
			case 6: {
				const auto list = tree.toStringList();
				for (const auto& entry : list) {
					if (!tree.contains(entry)) {
						abort();
					}
				}
				break;
			}
			case 7:
				tree.clear();
				break;
			default:
				break;
		}
	}

	return 0;
}
