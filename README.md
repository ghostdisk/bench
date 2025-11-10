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

#### Setting up a vs141_xp Visual Studio project

*TODO: In the future we plan to have a template project, but right now you have to go through the Visual Studio Library Linking Ritual, explained below:* 

1. From the Visual Studio Installer, select "C++ Windows XP Support for VS 2017 (v141) tools"
2. Create a new solution with either "C++ Console Application" or "Win32 Application"
3. Add the Bench SDK as a subdirectory to your project
4. File > Add > Existing Project, and select bench/proj/vs2017/bench.vcxproj
5. Configure your game's project:
	- Switch from x64 to x86 - x64 is not supported at this point in time as it won't run on my 2004 laptop
	- Configuration Properties/
		- General/
			- Windows SDK Version: 10 (or something else, but make sure it matches the bench SDK library's)
			- Platform Toolset: Visual Studio 2017 - Windows XP (v141_xp)	
		- C++/
			- General/
				- Additional Include Directories - Add $(SolutionDir)bench\
			- Preprocessor/
				- Add BENCH_WIN32 define
			- Code Generation
				- Runtime Library - for Debug configuration, "Multi-threaded Debug (/MTd)", and for Release configuration: "Multi-threaded (/MT)"
		- Linker/
			- General/
				- Additional Library Directories: Add $(OutDir)
			- Input/
				- Additional Dependencies: Add bench.lib
			- Advanced/
				- Image Has Safe Exception Handlers: No (/SAFESEH:NO)
				
### Custom Setups

If you're feeling adventurous and want to build with another compiler/build system, it should be fairly simple (unless you're targeting XP, which is not covered here and will involve hair pulling).

1. Create a executable in your build system
2. Add all the .cpp files into your build - either directly as part of your project, or as a separate library
3. Add BENCH_WIN32 define (for forward compatibility)
4. Add the Bench SDK's root folder as an include directory (so <bench/\*> includes resolve)
