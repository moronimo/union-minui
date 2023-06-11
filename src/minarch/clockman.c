#include "clockman.h"

//basic clock manager for minarch

#define NUM_CPU_CLOCKS 7
#define TARGET_CPU_UTILIZATION 81

static int cpu_freqs[NUM_CPU_CLOCKS] =          { 1296000, 1200000, 1104000, 1008000, 840000, 720000, 504000 };
//ratio of the current clock / next 
static float next_freq_ratios[NUM_CPU_CLOCKS] = {   1.08f,   1.09f,   1.10f,   1.20f,  1.16f,  1.43f,  10.0f };

static int current_cpu_clock_index = 0;
static int current_cpu_clock_weight = 0;

int getTargetClock( double cpu_rate, double utilization ) {
	int target_cpu_clock_index = current_cpu_clock_index;

	if ( cpu_rate < 59 ) {
		target_cpu_clock_index = 0;
	} else if ( utilization * next_freq_ratios[current_cpu_clock_index] > TARGET_CPU_UTILIZATION ) {
		current_cpu_clock_weight = 0;
	} else {
		++current_cpu_clock_weight;
		if( current_cpu_clock_weight > 60 ) {
			if( target_cpu_clock_index + 1 < NUM_CPU_CLOCKS ) {
				++target_cpu_clock_index;
			}
		}
	}

	if(target_cpu_clock_index != current_cpu_clock_index) {
		current_cpu_clock_index = target_cpu_clock_index;
		current_cpu_clock_weight = 0;
		return cpu_freqs[target_cpu_clock_index];
	}
	return 0;
}

