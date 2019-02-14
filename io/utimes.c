// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2016 Deepa Dinamani
 * All Rights Reserved.
 */

#include <fcntl.h>
#include <sys/syscall.h>

#include "command.h"
#include "input.h"
#include "init.h"
#include "io.h"

#ifndef AT_UTIME_BTIME
#define AT_UTIME_BTIME 0x8000
#endif

static cmdinfo_t utimes_cmd;

static void
utimes_help(void)
{
	printf(_(
"\n"
" Update file atime, mtime, and optionally btime of the current file with\n"
" nansecond precision.\n"
"\n"
" Usage: utimes atime_sec atime_nsec mtime_sec mtime_nsec [btime_sec btime_nsec]\n"
" *_sec: Seconds elapsed since 1970-01-01 00:00:00 UTC.\n"
" *_nsec: Nanoseconds since the corresponding *_sec.\n"
"\n"));
}

static int
utimes_f(
	int		argc,
	char		**argv)
{
	struct timespec t[3];
	int flags = 0;
	int result;

	if (argc == 6)
		return command_usage(&utimes_cmd);

	/* Get the timestamps */
	result = timespec_from_string(argv[1], argv[2], &t[0]);
	if (result) {
		fprintf(stderr, "Bad value for atime\n");
		return 0;
	}
	result = timespec_from_string(argv[3], argv[4], &t[1]);
	if (result) {
		fprintf(stderr, "Bad value for mtime\n");
		return 0;
	}
	if (argc == 7) {
		result = timespec_from_string(argv[5], argv[6], &t[2]);
		if (result) {
			fprintf(stderr, "Bad value for btime\n");
			return 0;
		}
		flags |= AT_UTIME_BTIME;
	}

	/*
	 * Use syscall() because the glibc wrapper for utimensat() disallows a
	 * NULL pathname.
	 */
	if (syscall(SYS_utimensat, file->fd, NULL, t, flags))
		perror("utimensat");

	return 0;
}

void
utimes_init(void)
{
	utimes_cmd.name = "utimes";
	utimes_cmd.cfunc = utimes_f;
	utimes_cmd.argmin = 4;
	utimes_cmd.argmax = 6;
	utimes_cmd.flags = CMD_NOMAP_OK | CMD_FOREIGN_OK;
	utimes_cmd.args = _("atime_sec atime_nsec mtime_sec mtime_nsec [btime_sec btime_nsec]");
	utimes_cmd.oneline = _("Update file times of the current file");
	utimes_cmd.help = utimes_help;

	add_command(&utimes_cmd);
}
