#include <errno.h>
#include "ht-private.hpp"

namespace ht {

Table::Table()
{
	mMaxKeySize = 0;
	mMaxValueSize = 0;
}

Table::~Table()
{
	mEntryByKey.clear();
	mEntryByValue.clear();

	for (auto entry : mEntryList) {
		delete entry;
	}
}

int Table::addEntry(const uint8_t *key, size_t keySize, const std::u32string &value)
{
	Key inputKey { const_cast<uint8_t *>(key), keySize };
	Entry *newEntry;
	int res;

	// Duplicates forbidden
	if (mEntryByKey.count(&inputKey) > 0) {
		res = -EINVAL;
		goto error;
	} else if (mEntryByValue.count(value) > 0) {
		res = -EINVAL;
		goto error;
	}

	newEntry = new Entry(key, keySize, value);
	if (newEntry == NULL) {
		res = -ENOMEM;
		goto error;
	}

	mEntryList.push_back(newEntry);
	mEntryByKey.insert(std::make_pair(&newEntry->mKey, newEntry));
	mEntryByValue.insert(std::make_pair(value, newEntry));

	if (keySize > mMaxKeySize) {
		mMaxKeySize = keySize;
	}

	if (value.size() > mMaxValueSize) {
		mMaxValueSize = value.size();
	}

	return 0;

error:
	return res;
}

size_t Table::getEntryCount() const
{
	return mEntryByKey.size();
}

size_t Table::getMaxKeySize() const
{
	return mMaxKeySize;
}

size_t Table::getMaxValueSize() const
{
	return mMaxValueSize;
}

const Table::Entry *Table::findFromValue(const std::u32string &value) const
{
	const Entry *res;

	auto i = mEntryByValue.find(value);
	if (i != mEntryByValue.end()) {
		res = (const Entry *) i->second;
	} else {
		res = NULL;
	}

	return res;
}

const Table::Entry *Table::findFromKey(const uint8_t *key, size_t keySize) const
{
	Key inputKey { const_cast<uint8_t *>(key), keySize };
	const Entry *res;

	auto i = mEntryByKey.find(&inputKey);
	if (i != mEntryByKey.end()) {
		res = (const Entry *) i->second;
	} else {
		res = NULL;
	}

	return res;
}

} // namespace ht
