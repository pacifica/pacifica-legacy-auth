#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pacifica_auth_cb)(const char *buffer, int size, void *data);

int pacifica_auth(pacifica_auth_cb callback, void *data);

#ifdef __cplusplus
} /* END extern C */
#endif

