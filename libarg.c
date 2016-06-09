#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "libarg.h"

enum LIBARG_ARG_TYPE {
	LIBARG_ARG_FLAG = 1,
	LIBARG_ARG_INT,
	LIBARG_ARG_STR,
};

struct libarg_t;

struct libarg_ctx_t {
	libarg_usage_func_t usage;
	struct libarg_t* head;
};

struct libarg_t {
	char    type;
	char    missed;
	union {
		char **s_val;
		int  *i_val;
	} v;
	char    short_name;
	char    *long_name;
	struct libarg_t* next;
};

libarg_ctx* libarg_init(libarg_usage_func_t func)
{
	struct libarg_ctx_t *ctx = (struct libarg_ctx_t *)malloc(sizeof(struct libarg_ctx_t *));
	ctx->head = NULL;
	ctx->usage = func;

	return (libarg_ctx *)ctx;
}

void libarg_destroy(libarg_ctx* p)
{
	struct libarg_t *t;
	struct libarg_ctx_t *ctx = (struct libarg_ctx_t *)p;

	if (!ctx) {
		return;
	}

	while (ctx->head) {
		t = ctx->head;
		if (t->long_name)
			free(t->long_name);
		if (t->type == LIBARG_ARG_STR && t->v.s_val) {
			if (*t->v.s_val)
				free(*t->v.s_val);
		}
		ctx->head = t->next;
		free(t);
	}
	if (ctx->head)
		free(ctx->head);
	free(ctx);
}

int libarg_add_flag(libarg_ctx *p, int *ref, char short_arg, const char* long_arg)
{
	struct libarg_t *arg;
	struct libarg_ctx_t *ctx = (struct libarg_ctx_t *)p;

	if (!ref)
		return -2;

	arg = (struct libarg_t*)malloc(sizeof(struct libarg_t));
	if (!arg)
		return -1;

	arg->type = LIBARG_ARG_FLAG;
	arg->v.i_val = ref;
	arg->short_name = short_arg;
	if (long_arg) {
		arg->long_name = (char *)malloc(strlen(long_arg) + 1);
		strcpy(arg->long_name, long_arg);
	}
	arg->missed = 0;
	*arg->v.i_val = 0;

	arg->next = ctx->head;
	ctx->head = arg;

	return 0;
}

int libarg_add_int(libarg_ctx *p, int required, int *ref, const char short_arg, const char* long_arg, int default_val)
{
	struct libarg_t *arg;
	struct libarg_ctx_t *ctx = (struct libarg_ctx_t *)p;

	if (!ref)
		return -2;

	arg = (struct libarg_t*)malloc(sizeof(struct libarg_t));
	if (!arg)
		return -1;

	arg->type = LIBARG_ARG_INT;
	arg->v.i_val = ref;
	arg->short_name = short_arg;
	if (long_arg) {
		arg->long_name = (char *)malloc(strlen(long_arg) + 1);
		strcpy(arg->long_name, long_arg);
	}
	arg->missed = required;
	*arg->v.i_val = default_val;
	arg->next = ctx->head;
	ctx->head = arg;

	return 0;
}

int libarg_add_str(libarg_ctx *p, int required, char **ref, char short_arg, const char* long_arg, const char* default_val)
{
	struct libarg_t *arg;
	struct libarg_ctx_t *ctx = (struct libarg_ctx_t *)p;

	if (!ref)
		return -2;

	arg = (struct libarg_t*)malloc(sizeof(struct libarg_t));
	if (!arg)
		return -1;

	arg->type = LIBARG_ARG_STR;
	arg->v.s_val = ref;
	arg->short_name = short_arg;
	if (long_arg) {
		arg->long_name = (char *)malloc(strlen(long_arg) + 1);
		strcpy(arg->long_name, long_arg);
	}
	arg->missed = required;
	*arg->v.s_val = (char *)malloc(strlen(default_val) + 1);
	strcpy(*arg->v.s_val, default_val);
	arg->next = ctx->head;
	ctx->head = arg;

	return 0;
}

int libarg_settle(libarg_ctx *ctx, unsigned char pedantic, int argc, char **argv)
{
	struct libarg_t *p;
	int i = 0, k = 0, j = 0, len, i_val, err = 0;
	char *endptr, *ptr;
	char short_arg[] = "-\0";

	if (!ctx) {
		return -1;
	} else
		p = ((struct libarg_ctx_t *)ctx)->head;

	for (i = 1; i < argc; i++) {
		p = ((struct libarg_ctx_t *)ctx)->head;
		err = 1;
		k = 0;

		while (p) {
			short_arg[1] = p->short_name;
			if (p->short_name && !strncmp(argv[i], short_arg, short_arg[1] ? 2 : 1)
					|| p->long_name && !strcmp(argv[i], p->long_name)) {
				err = 0;
				if (strlen(argv[i]) == 2 && (p->type == LIBARG_ARG_INT || p->type == LIBARG_ARG_STR)) {
					i++;
					k = 1;
				}
				break;
			}
			p = p->next;
		}
		i -= k;

		if (!err) {
			switch (p->type) {
			case LIBARG_ARG_FLAG:
				if (!strncmp(argv[i], short_arg, short_arg[1] ? 2 : 1)
						|| p->long_name && !strcmp(argv[i], p->long_name)) {
					(*p->v.i_val)++;
					p->missed = 0;
					continue;
				}
				break;

			case LIBARG_ARG_INT:
				ptr = argv[i];
				len = (unsigned int)strlen(argv[i]);
				if (len > 2 && !strncmp(argv[i], short_arg, short_arg[1] ? 2 : 1)) {
					if ((i_val = strtol(argv[i] + 2, &endptr, 10)) >= 0 && endptr - ptr == len
							&& errno != ERANGE && errno != EINVAL) {
						*p->v.i_val = i_val;
						p->missed = 0;
						continue;
					} else {
						err = 1;
						break;
					}
				} else if ((len == 2 && !strncmp(argv[i], short_arg, short_arg[1] ? 2 : 1)
						|| p->long_name && !strcmp(argv[i], p->long_name)) && argc > i + 1) {
					k = i + 1;
					if ((i_val = strtol(argv[k], &endptr, 10)) >= 0 && (ptr = argv[k], len = strlen(argv[k]))
							&& endptr - ptr == len && errno != ERANGE && errno != EINVAL) {
						*p->v.i_val = i_val;
						i++;
						p->missed = 0;
						continue;
					} else {
						err = 1;
						break;
					}
				} else
					err = 1;
				break;

			case LIBARG_ARG_STR:
				j = k = 0;
				if (!strncmp(argv[i], short_arg, short_arg[1] ? 2 : 1) || p->long_name && !strcmp(argv[i], p->long_name)) {
					if (argc > i + 1 && strlen(argv[i]) == strlen(short_arg) && (k = 1)
							|| strlen(argv[i]) > strlen(short_arg) && (j = strlen(short_arg))) {
						if (*p->v.s_val)
							free (*p->v.s_val);
						*p->v.s_val = (char *)malloc(strlen(argv[i + k]) + 1 - j);
						strcpy(*p->v.s_val, argv[i + k] + j);
						i += k;
						p->missed = 0;
						continue;
					} else {
						err = 1;
						break;
					}
				}
				break;

			default:
				fprintf(stderr, "libarg internal error!\n");
				break;
			}
		}

		if (pedantic && err) {
			printf("am I missed something? %s\n", argv[i]);
			if (((struct libarg_ctx_t *)ctx)->usage)
				((struct libarg_ctx_t *)ctx)->usage(argv[0]);
			return i;
		}
	}
	p = ((struct libarg_ctx_t *)ctx)->head;
	while (p) {
		if (p->missed) {
			printf("am I missed something?\n");
			if (((struct libarg_ctx_t *)ctx)->usage)
				((struct libarg_ctx_t *)ctx)->usage(argv[0]);
			return -1;
		}
		p = p->next;
	}

	return 0;
}

