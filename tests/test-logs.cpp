#include <ht/Log.hpp>

static const char *TAG = "TEST";

int main(int argc, char *argv[])
{
	ht::Log::i(TAG, "Hello world");

	return 0;
}
