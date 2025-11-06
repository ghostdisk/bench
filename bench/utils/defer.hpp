#pragma once

// thank you mr ginger bill
// https://www.gingerbill.org/article/2015/08/19/defer-in-cpp/

namespace bench {

template <typename F>
struct privDefer {
	F f;
	privDefer(F f) : f(f) {}
	~privDefer() { f(); }
};

template <typename F>
privDefer<F> defer_func(F f) {
	return privDefer<F>(f);
}

}

#define BENCH_DEFER_1(x, y) x##y
#define BENCH_DEFER_2(x, y) BENCH_DEFER_1(x, y)
#define BENCH_DEFER_3(x)    BENCH_DEFER_2(x, __COUNTER__)
#define BENCH_DEFER(code)   auto BENCH_DEFER_3(_defer_) = bench::defer_func([&](){code;})

#ifndef BENCH_NO_NONPREFIXED_MACROS
#define DEFER BENCH_DEFER
#endif
