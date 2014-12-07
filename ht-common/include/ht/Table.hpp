#ifndef __HT_TABLE_HPP__
#define __HT_TABLE_HPP__

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <string>
#include <list>
#include <map>

namespace ht {

class Table {
public:
	struct Key {
		uint8_t *mValue;
		size_t mSize;
	};

	struct Entry {
		Key mKey;
		std::u32string mValue;

		Entry(const uint8_t *key, size_t keySize, const std::u32string &value)
		{
			mKey.mValue = (uint8_t *) malloc(keySize * sizeof(uint8_t));
			memcpy(mKey.mValue, key, keySize);
			mKey.mSize = keySize;

			mValue = value;
		}

		~Entry()
		{
			free(mKey.mValue);
		}
	};

private:
	struct KeyPtrComparator {
		bool operator()(Key *x, Key *y) const {
			if (x->mSize < y->mSize) {
				return true;
			} else if (x->mSize > y->mSize) {
				return false;
			} else {
				return (memcmp(x->mValue, y->mValue, x->mSize) < 0);
			}
		}
	};

private:
	std::list<Entry *> mEntryList;

	std::map<Key *, Entry *, KeyPtrComparator> mEntryByKey;
	std::map<std::u32string, Entry *> mEntryByValue;

	size_t mMaxKeySize;
	size_t mMaxValueSize;

public:
	Table();
	virtual ~Table();

	int addEntry(const uint8_t *key, size_t keySize, const std::u32string &value);

	size_t getMaxKeySize() const;
	size_t getMaxValueSize() const;

	const Entry *findFromValue(const std::u32string &value) const;
	const Entry *findFromKey(const uint8_t *key, size_t keySize) const;
};

} // ht

#endif // !__HT_TABLE_HPP__
