# Benched Software Development Kit

The Benched Software Development Kit (The SDK) is a collection of libraries and applications aimed to ease the development of 3D multimedia applications for Windows XP computers.

*TODO: The SDK does not yet exist.*

## Features

### Asynchronous Programming

Benched SDK implements stackful coroutines for convenient asynchronous programming.

```cpp
	Coroutine coro = CreateCoroutine([](Coroutine coro, void* userdata) {
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