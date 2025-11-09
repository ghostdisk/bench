#include <bench/defer.hpp>
#include <bench/asset.hpp>
#include <bench/arena.hpp>
#include <bench/string.hpp>
#include <bench/coroutine.hpp>
#include <bench/file.hpp>

namespace bench {

void LoadAsset(String path) {
	path = path.CopyToHeap();

	StartCoroutine([=](CoroutineHandle coro) {
		ScratchArenaView scratch = Arena::Scratch();


		void* data;
		size_t size;
		File::ReadEntireFileAsync(coro, path, &data, &size);
		int t = 3;
	});
}

}