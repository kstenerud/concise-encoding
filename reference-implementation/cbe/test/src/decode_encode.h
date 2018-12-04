#ifndef decode_encode_H
#define decode_encode_H
#ifdef __cplusplus
extern "C" {
#endif

#include <cbe/cbe.h>

struct cbe_decode_process* new_decode_encode_process(cbe_decode_callbacks* callbacks, struct cbe_encode_process* encode_process);

/**
 * Decode a document and use the decode events to encode the document again.
 *
 * @param document The source document.
 * @param byte_count The length in bytes of the document.
 * @param encode_process An encode process to use for creating the new document.
 * @return The decoder's last status.
 */
cbe_decode_status perform_decode_encode(const uint8_t* document, int byte_count, struct cbe_encode_process* encode_process);


#ifdef __cplusplus
}
#endif
#endif // decode_encode_H
