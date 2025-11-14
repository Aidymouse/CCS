
#include <stdio.h> 
#include <string.h>

#define NOB_IMPLEMENTATION

//#define TARGET_WIN32
#define TARGET_UNIX

#include "nob.h"

#define PROGRAM_NAME "ECS"

/* All paths assume target is created in the top level folder. */
/* That is, to distribute, just delete all the C files n shit lmao */

int main(int argc, char **argv) {

	NOB_GO_REBUILD_URSELF(argc, argv);

	Nob_Cmd cmd = { 0 };

	nob_cmd_append(&cmd, "clang", "-o", PROGRAM_NAME);
	nob_cmd_append(&cmd, "test.c");

	nob_cmd_append(&cmd, "-g");

	nob_cmd_append(&cmd, "-Iinclude");

	// Other stuff
	nob_cmd_append(&cmd, "-lm");

	nob_cmd_run_sync(cmd);

	return 0;
}

