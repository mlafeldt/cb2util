/*
 * Main project file
 *
 * Copyright (C) 2006-2012 Mathias Lafeldt <mathias.lafeldt@gmail.com>
 *
 * This file is part of cb2util, the CodeBreaker PS2 File Utility.
 *
 * cb2util is licensed under the terms of the MIT License. See LICENSE file.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

extern int cmd_cbc(int argc, char **argv);
extern int cmd_cheats(int argc, char **argv);
extern int cmd_pcb(int argc, char **argv);
static int cmd_help(int argc, char **argv);

struct cmd_struct {
	const char *cmd;
	int (*fn)(int argc, char **argv);
};

static const struct cmd_struct _commands[] = {
	{ "help", cmd_help },
	{ "cbc", cmd_cbc },
	{ "cheats", cmd_cheats },
	{ "pcb", cmd_pcb },
	{ NULL, NULL }
};

static const char *cb2util_usage =
	"usage: cb2util [--version] [--help] <command> [<args>]\n\n"
	"The available commands are:\n";

static void show_usage(void)
{
	const struct cmd_struct *p = _commands;

	printf("%s", cb2util_usage);

	p++; /* skip help */
	while (p->cmd) {
		printf("    %s\n", p->cmd);
		p++;
	}

	printf("\nTry 'cb2util help <command>' for more information.\n");
}

static void show_version(void)
{
	printf("cb2util version %s\n", CB2UTIL_VERSION);
}

static int cmd_help(int argc, char **argv)
{
	show_usage();

	return 0;
}

static void handle_command(int argc, char **argv)
{
	char *cmd = argv[0];
	const struct cmd_struct *p = _commands;

	/* turn "help cmd" into "cmd --help" */
	if (argc > 1 && !strcmp(argv[0], "help")) {
		argv[0] = cmd = argv[1];
		argv[1] = "--help";
	}

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
		show_usage();
		exit(1);
	}

	if (!strcmp(argv[0], "--help")) {
		argv[0] = "help";
	} else if (!strcmp(argv[0], "--version")) {
		show_version();
		return 0;
	}

	cmd = argv[0];
	handle_command(argc, argv);
	fprintf(stderr, "Failed to run command %s\n", cmd);
	return 1;
}
