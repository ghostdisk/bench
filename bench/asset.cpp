#include <bench/asset.hpp>
#include <bench/string.hpp>
#include <bench/coroutine.hpp>
#include <bench/file.hpp>
#include <bench/arena.hpp>
#include <bench/format.hpp>
#include <bench/arraylist.hpp>

namespace bench {

static ArrayList<ModelAsset*> g_model_assets;

Asset::~Asset() {
	if (name)
		name.FreeFromHeap();
}

ModelAsset* ModelAsset::Get(String name) {
	for (ModelAsset* asset : g_model_assets) {
		if (asset->name == name) {
			return asset;
		}
	}

	ModelAsset* asset = new ModelAsset();
	asset->name = name.CopyToHeap();
	g_model_assets.Push(asset);

	StartCoroutine([=](CoroutineHandle coro) mutable {
		ScratchArenaView scratch = Arena::Scratch();
		String file_path = Format(scratch.arena, "./res/", name, ".obj");
		void* data;
		size_t size;

		asset->state = AssetState::STREAMING;
		File::ReadEntireFileAsync(coro, file_path, &data, &size);
		asset->state = AssetState::PROCESSING;
		asset->state = AssetState::LOADED;
	});
}

}