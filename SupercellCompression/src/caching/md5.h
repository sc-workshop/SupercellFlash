#ifndef MD5_H
#define MD5_H

#include <stdint.h>

class md5
{
public:

	/* The size of the MD5 digest in bytes */
	const static int size = 16;

	md5()
	{
		init();
	}

	/* MD5 block update operation. Continues an MD5 message-digest
	   operation, processing another message block, and updating the
	   context. */
	void update(const unsigned char *input, unsigned int inputLen);

	/* MD5 finalization. Ends an MD5 message-digest operation, writing
	   the message digest and zeroizing the context. */
	void final(unsigned char digest[16]);

private:

	/* MD5 context. */
	uint64_t _count;		/* number of bits, modulo 2^64 */
	uint32_t _state[4];		/* state (ABCD) */
	unsigned char _buffer[64];	/* input buffer */
    
	/* MD5 initialization. Begins an MD5 operation, writing a new context. */
	void init();

	/* Encodes input (UINT4) into output (unsigned char). Assumes len is
	   a multiple of 4. */
	static void encode(
		unsigned char *output,
		const uint32_t *input,
		unsigned int len);

	/* Decodes input (unsigned char) into output (UINT4). Assumes len is
	   a multiple of 4. */
	static void decode(
		uint32_t *output,
		const unsigned char *input,
		unsigned int len);

	/* MD5 basic transformation. Transforms state based on block. */
	static void transform(uint32_t state[4], const unsigned char block[64]);
};

#endif
