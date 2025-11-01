# Benched Software Development Kit

The Benched Software Development Kit (The SDK) is a collection of libraries and applications aimed to ease the development of 3D multimedia applications for Windows XP computers.

*TODO: The SDK does not yet exist.*

## Features

### Asynchronous Programming

Benched SDK implements stackful coroutines for convenient asynchronous programming.

```cpp
	CoroutineHandle coro = CreateCoroutine([](CoroutineHandle coro, void* userdata) {
		for (int i = 0; i < 5; i++) {
			printf("Iter %d\n", i);
			Yield(coro);
		}
	}, nullptr);

	for (;;) {
		printf("Resuming!\n");
		bool complete = ResumeCoroutine(coro);
		if (complete) break;
	}
```

Async File IO:
```cpp

	CoroutineHandle coro = CreateCoroutine([](CoroutineHandle coro, void* userdata) {
		File file = FileOpen("file.txt");

		char buf[32] = {};
		
		// FileReadAsync will yield, and the coroutine will be resumed
		// after the read asynchronously completes.
		I32 nread = FileReadAsync(coro, file, 32, buf);

		buf[31] = '\0';
		printf("%s\n", buf);
	}, nullptr);
	ResumeCoroutine(coro);

	for (;;) {
		PollFileEvents();
		ExecScheduledCoroutines(); // <- This is where our coroutine will be resumed
	}



```