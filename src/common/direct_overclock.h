#ifndef __FAST_OVERCLOCK_H__
#define __FAST_OVERCLOCK_H__

int setCPUFrequency(int frequency);

struct cpu_opp {
	int clk;
	int volt;
	char* desc;
};

extern struct cpu_opp cpu_opps[];

#endif
