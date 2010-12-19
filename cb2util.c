/*
 * Main project file
 *
 * Copyright (C) 2006-2010 Mathias Lafeldt <misfire@debugon.org>
 * All rights reserved.
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * cb2util is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

extern int cmd_cbc(int argc, char **argv);
extern int cmd_pcb(int argc, char **argv);
extern int cmd_cheats(int argc, char **argv);

int cmd_help(int argc, char **argv)
{
	printf("not implemented yet\n");
	return 0;
}

struct cmd_struct {
	const char *cmd;
	int (*fn)(int argc, char **argv);
	int option;
};

static void handle_command(int argc, char **argv)
{
	char *cmd = argv[0];
	static struct cmd_struct commands[] = {
		{ "help", cmd_help, 0 },
		{ "cbc", cmd_cbc, 0 },
		{ "pcb", cmd_pcb, 0 },
		{ "cheats", cmd_cheats, 0 },
		{ NULL, NULL, 0 }
	};
	struct cmd_struct *p = commands;
#if 0
	/* turn "cmd --help" into "help cmd" */
	if (argc > 1 && !strcmp(argv[1], "--help")) {
		argv[1] = argv[0];
		argv[0] = cmd = "help";
	}
#endif
	while (p->cmd) {
		if (!strcmp(p->cmd, cmd))
			exit(p->fn(argc, argv));
		p++;
	}
}

int main(int argc, char **argv)
{
	const char *cmd;

	argc--;
	argv++;
	if (!argc) {
		printf("args missing\n");
		exit(1);
	}
	cmd = argv[0];

	handle_command(argc, argv);
	fprintf(stderr, "Failed to run command %s\n", cmd);
	return 1;
}
