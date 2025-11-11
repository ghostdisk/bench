# Bench Software Development Kit

![image](docs/bench.png)

The Bench Software Development Kit (The SDK) is a collection of libraries and applications aimed to ease the development of 3D multimedia applications for Windows XP computers.

*TODO: The SDK does not yet exist.*

## Features

### Asynchronous Programming

Bench SDK provides a *still-very-early-unstable-work-in-progress* stackful coroutine/fiber-based asynchronous programming environment. 

If you want to do something once per second, fire a coroutine with an loop and Sleep(1).
```cpp
	StartCoroutine([](CoroutineHandle coro, void* userdata) {
		for (;;) {
			printf("tick\n");
			
			// Sleep increases the coroutine's block count, and decreases it when the interval passes
			Sleep(coro, 1.0);
			
			// after blocking the coroutine, yield. The reason Sleep and similar functions don't yield
			// is to allow us to call multiple coroutine-blocking functions, and then Yield once
			Yield();          
		}
	});
```

Fences are supported for synchronization:

```cpp
Fence fence = CreateFence();
int result = 0;

StartCoroutine([&](CoroutineHandle coro) {
	// Pretend we're doing some hard work...
	Sleep(coro, 5.0);
	Yield(coro);
	result = 1234;
	
	SignalFence(fence); 
});

StartCoroutine([&](CoroutineHandle coro) {
	// Pretend we're doing some work...
	Sleep(coro, 2.0);
	Yield(coro);
	
	// Wait for the other coroutine to complete:
	WaitForFence(coro, fence);
	Yield();
});


```

Coroutines are also leveraged for async I/O:

```cpp
	StartCoroutine([](CoroutineHandle coro, void* userdata) {
		File file = File::Open("file.txt");

		char buf[32] = {};

		I32 nread = file.ReadAsync(coro, 32, buf);
		Yield(coro);

		buf[31] = '\0';
		printf("%s\n", buf);
	}, nullptr);
```

## Compiling for Windows XP

### vs141_xp

Currently this is the only officially supported setup. On Visual Studio 2022 you can still install a Windows XP-compatible compiler (). This has the following trade-offs:
- ✔️Works out of the box with Visual Studio
- ✔️Reasonably modern compiler
- ❌No C++20 support (basically no named initializers)
- ❌Modern Windows SDK and CRT are quite bloated - an empty statically linked executable with vs141_xp is ~200KB, while with an older Windows Vista SDK it's ~60KB

### Building with cmake

Use the following CMakePresets.json:
```json
{
  "version": 3,
  "configurePresets": [
    {
      "name": "base-preset",
      "hidden": true,
      "generator": "Visual Studio 17 2022",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "toolset": "v141_xp",
      "architecture": "Win32"
    },
    {
      "inherits": "base-preset",
      "name": "debug",
      "displayName": "Debug (x86)",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Debug"
      }
    },
    {
      "inherits": "base-preset",
      "name": "release",
      "displayName": "Release (x86)",
      "cacheVariables": {
        "CMAKE_BUILD_TYPE": "Release"
      },
    }
  ],
  "buildPresets": [
    {
      "name": "debug",
      "displayName": "Build Debug",
      "configuration": "debug",
      "configurePreset": "debug",
    },
    {
      "name": "release",
      "displayName": "Build Release",
      "configuration": "release",
      "configurePreset": "release"
    }
  ]
}
```

and this CMakeLists.txt:
```
cmake_policy(SET CMP0091 "NEW")
cmake_minimum_required(VERSION 3.24)

project(game)
add_executable(game
	game/main.cpp)

add_subdirectory(bench)
target_link_libraries(game bench_core) 

SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
set_property(TARGET game PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
```


мога да ви го диктувам кода ако искате
ФУНКЦИОН 