#include <errno.h>
#include <ht/Table.hpp>

namespace ht {

Table::Table()
{

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

	return 0;

error:
	return res;
}

const Table::Key *Table::findFromValue(const std::u32string &value) const
{
	const Key *res;

	auto i = mEntryByValue.find(value);
	if (i != mEntryByValue.end()) {
		res = (const Key *) i->second;
	} else {
		res = NULL;
	}

	return res;
}

const std::u32string *Table::findFromKey(const uint8_t *key, size_t keySize) const
{
	Key inputKey { const_cast<uint8_t *>(key), keySize };
	const std::u32string *res;

	auto i = mEntryByKey.find(&inputKey);
	if (i != mEntryByKey.end()) {
		res = (const std::u32string *) &i->second->mValue;
	} else {
		res = NULL;
	}

	return res;
}

} // namespace ht
