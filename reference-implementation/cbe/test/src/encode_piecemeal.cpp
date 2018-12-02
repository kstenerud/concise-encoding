#include "test_helpers.h"

// Test working with partial buffers to simulate a streaming environment

TEST(EncodePiecemeal, piecemeal)
{
	expect_piecemeal_operation_decrementing(
		4,
		2,
		[&](cbe_encode_process* encode_process)
		{
			cbe_encode_status status;
			if((status = cbe_encode_begin_list(encode_process)) != CBE_ENCODE_STATUS_OK) return status;
			if((status = cbe_encode_add_int(encode_process, 1)) != CBE_ENCODE_STATUS_OK) return status;
			if((status = cbe_encode_add_int(encode_process, 0x1000)) != CBE_ENCODE_STATUS_OK) return status;
			return cbe_encode_end_container(encode_process);
		},
		[&](cbe_encode_process* encode_process)
		{
			cbe_encode_status status;
			if((status = cbe_encode_add_int(encode_process, 0x1000)) != CBE_ENCODE_STATUS_OK) return status;
			return cbe_encode_end_container(encode_process);
		},
		{0x7b, 0x01, 0x6e, 0x00, 0x10, 0x7d});
}
