#ifndef cbe_config_H
#define cbe_config_H
#ifdef __cplusplus 
extern "C" {
#endif

#ifdef __cplusplus
// C++ doesn't seem to have a POD implementation of decimal types.
	#define CBE_HAS_DECIMAL_SUPPORT 0
#else
	#define CBE_HAS_DECIMAL_SUPPORT 1
#endif


#ifdef __cplusplus 
}
#endif
#endif // cbe_config_H
