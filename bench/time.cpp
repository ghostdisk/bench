#include <bench/time.hpp>
#include <bench/coroutine.hpp>
#include <vector>

namespace bench {

struct Sleeper {
	double wake_time = 0;
	CoroutineHandle coro = {};
};

static std::vector<Sleeper> g_sleepers;

void Sleep(Coroutine* coro, double seconds) {
	if (seconds > 0) {
		g_sleepers.push_back({ GetTime() + seconds, coro });
		Yield(coro);
	}
}

bool ProcessSleepingCoroutines() {
	double time = GetTime();

	std::vector<Sleeper> still_sleepy;
	bool woke_some = false;

	for (const Sleeper& entry : g_sleepers) {
		if (entry.wake_time < time) {
			woke_some = true;
			ScheduleCoroutine(entry.coro);
		}
		else {
			still_sleepy.push_back(entry);
		}
	}

	g_sleepers = still_sleepy;
	return woke_some;
}

}
