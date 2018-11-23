#ifndef decode_encode_H
#define decode_encode_H
#ifdef __cplusplus
extern "C" {
#endif

#include <cbe/cbe.h>

bool decode_encode(const uint8_t* src, int src_length, cbe_encode_context* dst_buffer);


#ifdef __cplusplus
}
#endif
#endif // decode_encode_H
