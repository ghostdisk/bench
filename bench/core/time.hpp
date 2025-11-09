#pragma once
#include <bench/core/common.hpp>

namespace bench {

struct Coroutine;

double GetTime();
void Sleep(Coroutine* coro, double seconds);
bool ProcessSleepingCoroutines();

}