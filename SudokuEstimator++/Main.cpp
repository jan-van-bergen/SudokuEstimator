#include <thread>

#include "SudokuEstimator.h"

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

int thread_count;			// Number of Logical Processors
int threads_per_processor;

int     processor_count = 0;	// Number of Physical Processors
ULONG * processor_masks;		// Stores the mask of each Physical Processor

void create_and_run_estimator(int thread_index) {
	// Set the Thread Affinity to two logical cores that belong to the same physical core
	HANDLE thread = GetCurrentThread(); 

	DWORD_PTR thread_affinity_mask     = processor_masks[thread_index / threads_per_processor];
	DWORD_PTR thread_affinity_mask_old = SetThreadAffinityMask(thread, thread_affinity_mask);

	// Check validity of Thread Affinity
    if ((thread_affinity_mask & thread_affinity_mask_old) == 0) {
		printf("Unable to set Process Affinity Mask!\n");

		abort();
	}

	// Run the simulator
	SudokuEstimator estimator;
	estimator.run(thread_index);
}

int main() {
	// Ensure there is a Results folder, otherwise the program will crash
	DWORD attrib = GetFileAttributes(L"./Results");
	if (attrib == INVALID_FILE_ATTRIBUTES || !(attrib & FILE_ATTRIBUTE_DIRECTORY)) {
		CreateDirectory(L"./Results", nullptr);
	}
	
	// Acquire logical core count of this machine
	thread_count = std::thread::hardware_concurrency();
	if (thread_count == 0) {
		printf("Something went wrong when attempting to determine the number of cores on this machine!\n");

		abort();
	}

	processor_masks = new ULONG[thread_count];

	SYSTEM_LOGICAL_PROCESSOR_INFORMATION info[64];
	DWORD buffer_length = 64 * sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION);
	GetLogicalProcessorInformation(info, &buffer_length);
	
	// Count the number of physical cores and store their thread masks
	for (int i = 0; i < buffer_length / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION); i++) {
		if (info[i].Relationship == LOGICAL_PROCESSOR_RELATIONSHIP::RelationProcessorCore) {
			processor_masks[processor_count++] = info[i].ProcessorMask;
		}
	}

	threads_per_processor = thread_count / processor_count;

	for (int i = 0; i < thread_count; i++) {
		std::thread(create_and_run_estimator, i).detach();
	}

	// Run function on the main thread that prints the results of all the other threads to the console
	report_results();
}