#include <stdio.h>
#include <stdlib.h>
#include "libarg.h"

void usage(char *fn)
{
	printf("%s: arguments error\n", fn);
//	exit(1);
}

int main(int argc, char **argv)
{
	int ret;
	int bool_arg;
	int int_arg1, int_arg2;
	char *str;
	libarg_ctx *ctx;

	ctx = libarg_init(usage);

	libarg_add_flag(ctx, &bool_arg, 'k', "--keyon");
	libarg_add_int(ctx, 1, &int_arg1, 'd', "device", 1);
	libarg_add_int(ctx, 0, &int_arg2, 'n', "number", 7);
	libarg_add_str(ctx, 0, &str, 's', "string", "default");

	if ((ret = libarg_settle(ctx, 0, argc, argv)) != 0) {
		printf ("error with %s\n", ret > 0 ? argv[ret] : argv[0]);
		goto out;
	} else {
		printf("bool_arg = %d\n", bool_arg);
		printf("int_arg1 = %d\n", int_arg1);
		printf("int_arg2 = %d\n", int_arg2);
		printf("str = %s\n", str);
	}

out:
	libarg_destroy(ctx);

	return 0;
}

