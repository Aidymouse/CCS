
#define NOB_IMPLEMENTATION

#include "nob.h"

#define PROGRAM_NAME "Game"

int main(int argc, char **argv) {

	NOB_GO_REBUILD_URSELF(argc, argv);

	Nob_Cmd cmd = { 0 };

	nob_cmd_append(&cmd, "clang", "-g", "-o", PROGRAM_NAME);
	nob_cmd_append(&cmd, "main.c");

	nob_cmd_append(&cmd, "-g");

	nob_cmd_append(&cmd, "-Iinclude");
	nob_cmd_append(&cmd, "-Llib");
	nob_cmd_append(&cmd, "-Wl,-rpath,./lib");
	nob_cmd_append(&cmd, "-lraylib");

	// Other stuff
	nob_cmd_append(&cmd, "-lm");

	nob_cmd_run_sync(cmd);

	return 0;
}

