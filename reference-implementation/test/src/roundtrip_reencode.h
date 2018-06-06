#ifndef roundtrip_reencode_H
#define roundtrip_reencode_H
#ifdef __cplusplus
extern "C" {
#endif

#include <cbe/cbe.h>

void roundtrip_reencode(const uint8_t* src, int src_length, cbe_buffer* dst_buffer);


#ifdef __cplusplus
}
#endif
#endif // roundtrip_reencode_H
