#pragma once
#include <chrono>

// Timer that uses its constructor and destructor to record time
struct Scoped_Timer {
private:
	const char* name;
	std::chrono::high_resolution_clock::time_point start_time;

public:
	inline Scoped_Timer(const char* name = "Scoped Timer") : name(name) {
		start_time = std::chrono::high_resolution_clock::now();
	}

	inline ~Scoped_Timer() {
		auto      stop_time = std::chrono::high_resolution_clock::now();
		long long duration  = std::chrono::duration_cast<std::chrono::microseconds>(stop_time - start_time).count();

		if (duration >= 1000000) {
			printf("%s took: %llu us (%llu s)\n", name, duration, duration / 1000000);
		} else if (duration >= 1000) {
			printf("%s took: %llu us (%llu ms)\n", name, duration, duration / 1000);
		} else {
			printf("%s took: %llu us\n", name, duration);
		}
	}
};