//
//	cpu over/underclock (based on code from eggs)
//
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "direct_overclock.h"

//	-- 0xe64e v 1000000 : 700000 + (25000x12) / 0x0c 01100 1110[01100]1001110
//	-  0xe6ce v 1025000 : 700000 + (25000x13) / 0x0d 01101 1110[01101]1001110
//	   0xe84e v 1100000 : 700000 + (25000x16) / 0x10 10000 1110[10000]1001110
//	+  0xebce v 1275000 : 700000 + (25000x23) / 0x17 10111 1110[10111]1001110
//	++ 0xedce v 1375000 : 700000 + (25000x27) / 0x1b 11011 1110[11011]1001110
#define	VOLTMIN	(700000)
#define	VOLTMAX	(1400000)
#define	VOLTMUL	(25000)
static void setcpuvolt(int volt) {
	if (volt < VOLTMIN) volt = VOLTMIN;
	else if (volt > VOLTMAX) volt = VOLTMAX;
	volt = ((volt-VOLTMIN) / VOLTMUL)<<7;

	char str[16];
	sprintf(str, "11=%04x", 0xe04e | volt);
	int fd_reg = open("/sys/class/i2c-adapter/i2c-1/1-0065/reg_dbg", O_WRONLY);
	write(fd_reg, str, strlen(str));
	close(fd_reg);
}

#define CMU_BASE	(0xB0160000)
#define	CLKMIN		(192000)
#define	CLKMAX		(1524000)
#define	CLKMUL		(12000)
static void setcpuclock(int clock) {
	if (clock < CLKMIN) clock = CLKMIN;
	else if (clock > CLKMAX) clock = CLKMAX;
	clock = clock / CLKMUL;

	int fd_mem = open("/dev/mem", O_RDWR);
	uint32_t* cmu_mem = mmap(0, 4, PROT_READ|PROT_WRITE, MAP_SHARED, fd_mem, CMU_BASE);
	cmu_mem[0] = (cmu_mem[0] & 0xFFFFFF80) | clock;
	munmap(cmu_mem, 4);
	close(fd_mem);
}

static void setcpu(int clock, int volt) {
	setcpuvolt(VOLTMAX);	// Temporarily maximize
	setcpuclock(clock);
	setcpuvolt(volt);
}

// clk  240000 volt  975000
// clk  504000 volt 1000000
// clk  720000 volt 1025000
// clk 1008000 volt 1100000
// clk 1296000 volt 1275000
// clk 1488000 volt 1375000

struct cpu_opp cpu_opps[] = {
	{1488000, 1375000}, // 1.5GHz, MinUI Performance + launch
	{1392000, 1325000}, // 1.4GHz
	{1296000, 1275000}, // 1.3GHz, MinUI Normal
	{1200000, 1200000}, // 1.2GHz
	{1104000, 1175000}, // 1.1GHz, MinUI Powersave
	{1008000, 1100000}, // 1.0GHz, Anbernic default max, overvolted to stabilize
	{ 840000, 1075000}, // 840MHz, overvolted to stabilize
	{ 720000, 1025000}, // 720MHz, overvolted to stabilize
	{ 504000, 1000000}, // 500MHz, overvolted to stabilize, MinUI menus
	{ 240000,  975000}, // 240MHz, overvolted to stabilize
	{      0,       0},
};

int setCPUFrequency(int frequency) {
	for (int i=0; cpu_opps[i].clk; i++) {
		struct cpu_opp* cpu = &cpu_opps[i];
		if (frequency>=cpu->clk) {
			setcpu( cpu->clk, cpu->volt );

			char str[16];
			sprintf(str, "%i", cpu->clk);
			FILE *freq_file = fopen("/tmp/cpu_freq","w");
			fputs(str, freq_file);
			fclose(freq_file);
			return cpu->clk;
		}
	}
}
