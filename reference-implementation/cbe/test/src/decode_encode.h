#ifndef decode_encode_H
#define decode_encode_H
#ifdef __cplusplus
extern "C" {
#endif

#include <cbe/cbe.h>

cbe_decode_process* new_decode_encode_process(cbe_decode_callbacks* callbacks, cbe_encode_process* encode_process);

cbe_encode_status decode_encode(const uint8_t* src, int src_length, cbe_encode_process* encode_process);


#ifdef __cplusplus
}
#endif
#endif // decode_encode_H
