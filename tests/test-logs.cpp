#include <ht.hpp>

static const char32_t *TAG = U"TEST";

int main(int argc, char *argv[])
{
	ht::Log::i(TAG, U"Hello world é$€ÀÇ");

	ht::Log::d(TAG, U"Writing %d bytes at %p", 1, (void *) 0x100802000);
	ht::Log::d(TAG, U"Writing %d bytes at", 1);

	ht::Log::setLogLevel(ht::Log::Level::Debug);
	ht::Log::d(TAG, U"Writing bytes at %p", (void *) 0x100802000);

	return 0;
}
