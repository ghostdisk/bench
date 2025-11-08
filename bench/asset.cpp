#include <bench/utils/defer.hpp>
#include <bench/asset.hpp>
#include <bench/arena.hpp>
#include <bench/string.hpp>
#include <bench/coroutine.hpp>
#include <bench/file.hpp>

namespace bench {

void LoadAsset(String path) {
	struct Args {
		String path;
		Arena* arena;
	};

	/*`
	Arena* arena = Arena::CreateAndGetPtr();
	Args* args = arena->New<Args>();
	args->arena = arena;
	args->path = arena->InternString(path);

	StartCoroutine([](CoroutineHandle coro, void* _userdata) {
		Args* args = (Args*)_userdata;
		DEFER(args->arena->Destroy());

		void* data;
		size_t size;
		File::ReadEntireFileAsync(coro, args->path, &data, &size);
		int t = 3;

	}, args);
	*/
}

}