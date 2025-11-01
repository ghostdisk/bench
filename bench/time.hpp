#pragma once
#include <bench/common.hpp>

namespace bench {

struct Coroutine;

double GetTime();
void Sleep(Coroutine* coro, double seconds);
bool ProcessSleepingCoroutines();

}