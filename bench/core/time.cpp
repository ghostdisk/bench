#include <bench/core/time.hpp>
#include <bench/core/coroutine.hpp>
#include <bench/core/arraylist.hpp>

namespace bench {

struct Sleeper {
	double wake_time = 0;
	CoroutineHandle coro = {};
};

static ArrayList<Sleeper> g_sleepers;

void Sleep(Coroutine* coro, double seconds) {
	if (seconds > 0) {
		g_sleepers.Push({ GetTime() + seconds, coro });
		BlockCoroutine(coro, 1);
	}
}

bool ProcessSleepingCoroutines() {
	double time = GetTime();

	ArrayList<Sleeper> still_sleepy;
	bool woke_some = false;

	for (const Sleeper& entry : g_sleepers) {
		if (entry.wake_time < time) {
			woke_some |= UnblockCoroutine(entry.coro, 1);
		}
		else {
			still_sleepy.Push(entry);
		}
	}

	g_sleepers = still_sleepy;
	return woke_some;
}

}
