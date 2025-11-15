#pragma once
#include <bench/core/sys.hpp>
#include <Windows.h>

// undef insane Window.h macros:
#undef CreateWindow
#undef Yield
#undef GetCurrentDirectory

// 
#undef CREATE_NEW
#undef CREATE_ALWAYS
#undef OPEN_EXISTING
#undef OPEN_ALWAYS
#undef TRUNCATE_EXISTING
