#pragma once

#include <functional>
#include <vector>
#include <cbe/cbe.h>
#include "encoding.h"

typedef enum
{
	CBE_DECODING_OTHER,
	CBE_DECODING_STRING,
	CBE_DECODING_BINARY,
} cbe_decoding_type;

class cbe_decoder
{
private:
	cbe_decode_process* _process;
	std::vector<uint8_t> _received_data;
	int64_t _read_offset = 0;
	bool _process_is_valid = true;
	std::shared_ptr<enc::encoding> _decoded;
	cbe_decoding_type _currently_decoding_type = CBE_DECODING_OTHER;
	int64_t _currently_decoding_length;
	int64_t _currently_decoding_offset;
	std::vector<uint8_t> _currently_decoding_data;

public:
	// Internal functions
	bool set_next(std::shared_ptr<enc::encoding> encoding);
	bool begin_string(int64_t byte_count);
	bool begin_binary(int64_t byte_count);
	bool add_data(const std::vector<uint8_t>& data);

public:
	cbe_decoder();

	~cbe_decoder();

	// Feed data to be decoded.
	cbe_decode_status feed(std::vector<uint8_t>& data);

	// End the decoding process.
	cbe_decode_status end();

	// Get the complete raw data received.
	std::vector<uint8_t>& received_data();

	// Get the decoded encoding objects.
	std::shared_ptr<enc::encoding> decoded();
};
