#ifndef managed_allocator_H
#define managed_allocator_H
#ifdef __cplusplus
extern "C" {
#endif


void* managed_allocate(int size);

void managed_free_all();


#ifdef __cplusplus
}
#endif
#endif // managed_allocator_H
