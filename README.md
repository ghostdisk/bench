# Bench Software Development Kit

The Bench Software Development Kit (The SDK) is a collection of libraries and applications aimed to ease the development of 3D multimedia applications for Windows XP computers.

*TODO: The SDK does not yet exist.*

## Features

### Asynchronous Programming

The SDK provides a Powerful Userspace Coroutine Scheduling System to enable convenient asynchronous programming. If you want something to happen once per second, fire a coroutine with a loop, and call Sleep in it:

If you want to do something once per frame, fire a coroutine with a loop and Sleep(1).
```cpp
	StartCoroutine([](CoroutineHandle coro, void* userdata) {
		for (;;) {
			printf("tic\n");
			Sleep(coro, 1.0);
			printf("tac\n");
			Sleep(coro, 1.0);
		}
	});
```

Coroutines are also leveraged for async I/O:

```cpp
	StartCoroutine([](CoroutineHandle coro, void* userdata) {
		File file = FileOpen("file.txt");

		char buf[32] = {};
		
		// FileReadAsync will yield, and the coroutine will be resumed
		// after the read asynchronously completes.
		I32 nread = FileReadAsync(coro, file, 32, buf);

		buf[31] = '\0';
		printf("%s\n", buf);
	}, nullptr);
```