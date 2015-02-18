#ifndef __HT_CHARSETCONVERTER_HPP__
#define __HT_CHARSETCONVERTER_HPP__

namespace ht {

class CharsetConverter {
public:
	struct Cb {
		std::function<int(const void *buff, size_t size)> output;
		std::function<int(void)> invalidSeq;

		Cb()
		{
			clear();
		}

		bool isValid() const
		{
			return output != nullptr;
		}

		void clear()
		{
			output = nullptr;
			invalidSeq = nullptr;
		}
	};

public:
	virtual ~CharsetConverter() {}

	virtual int open(const char *tocode, const char *fromcode) = 0;
	virtual int open(const char *tocode, const char *fromcode, const Cb &cb) = 0;
	virtual int close() = 0;

	virtual int input(const void *rawContent, size_t rawContentSize) = 0;
	virtual int input(const void *rawContent, size_t rawContentSize, const Cb &cb) = 0;

public:
	HTAPI static CharsetConverter *create();
	HTAPI static void destroy(CharsetConverter *self);
};

} // ht

#endif // !__HT_CHARSETCONVERTER_HPP__
