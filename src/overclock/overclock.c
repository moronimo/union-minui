//
//	cpu over/underclock (based on code from eggs)
//
#include "direct_overclock.h"
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
	if (argc<2) {
		printf("Usage: %s <freq>\n", argv[0]);
		for (int i=0; cpu_opps[i].clk; i++) {
			printf("  %8i\n", cpu_opps[i].clk);
		}
		return 0;
	}
	
	int clk = 1008000; // default
	char *p;
	errno = 0;
	long arg = strtol(argv[1], &p, 10);
	
	if (errno != 0 || *p != '\0' || arg > INT_MAX || arg < INT_MIN); // buh
	else clk = arg;
	
	clk = setCPUFrequency(clk);
	/*char cmd[128];
	sprintf(cmd, "echo %i > /tmp/cpu_freq\n", clk);
	system(cmd);*/

	return 0;
}
