#if !defined(LIBARG_H)
#define LIBARG_H

typedef void* libarg_ctx;
typedef void (*libarg_usage_func_t)(char*);

/** \brief initialize liabarg pool
 * \param pointer to usege function either NULL if no usage function needed
 * */
libarg_ctx* libarg_init(libarg_usage_func_t);

/** \brief destroy libarg pool
 * \param pointer to value returned by libarg_init
 * */
void libarg_destroy(libarg_ctx *ctx);

/** \brief add flag argument to pool
 * \param ctx is a pointer to libarg_ctx object returned by libarg_init()
 * \param required fall in case of missed agrument
 * \param ref is a pointer to user variable
 * \param short_arg is a character argument name prepended with '-'
 * \param long_arg is a string argument name prepended or not with '--'
 * */
int libarg_add_flag(libarg_ctx *ctx, int *ref, char short_arg, const char* long_arg);

/** \brief add integer argument to pool
 * \param ctx is a pointer to libarg_ctx object returned by libarg_init()
 * \param required fall in case of missed agrument
 * \param ref is a pointer to user variable
 * \param short_arg is a character argument name prepended with '-'
 * \param long_arg is a string argument name prepended or not with '--'
 * \param default_val is a defalut value if no shuch argument was in command line
 * */
int libarg_add_int(libarg_ctx *ctx, int required, int *ref, char short_arg, const char* long_arg, int default_val);

/** \brief add string argument to pool
 * \param ctx is a pointer to libarg_ctx object returned by libarg_init()
 * \param required fall in case of missed agrument
 * \param ref is a pointer to user variable
 * \param short_arg is a character argument name prepended with '-'
 * \param long_arg is a string argument name prepended or not with '--'
 * \param default_val is a defalut value if no shuch argument was in command line
 * */
int libarg_add_str(libarg_ctx *ctx, int required, char **ref, char short_arg, const char* long_arg, const char* default_val);

/** \brief setle user variables with argument values
 * \param ctx is a pointer to libarg_ctx object returned by libarg_init()
 * \param required fall in case of missed agrument
 * \param pedantic NON IMPLEMENTED YET
 * \param argc is a arguments count received by main function
 * \param argv is a arguments array received by main function
 * */
int libarg_settle(libarg_ctx *ctx, unsigned char pedantic, int argc, char **argv);

#endif
