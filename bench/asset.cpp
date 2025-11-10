#include <bench/core/arena.hpp>
#include <bench/core/format.hpp>
#include <bench/core/string.hpp>
#include <bench/core/file.hpp>
#include <bench/asset.hpp>
#include <bench/coroutine.hpp>
#include <bench/arraylist.hpp>
#include <bench/core/time.hpp>
#include <stdio.h>

namespace bench {

static ArrayList<ModelAsset*> g_model_assets;

Asset::~Asset() {
	if (name)
		name.FreeFromHeap();
}

Asset::Asset() {
	this->fence_on_load = CreateFence();
}

Fence Asset::LoadFence() {
	return this->fence_on_load;
}

ModelAsset* ModelAsset::Get(String _name) {
	for (ModelAsset* asset : g_model_assets) {
		if (asset->name == _name) {
			return asset;
		}
	}

	ModelAsset* asset = new ModelAsset();
	asset->name = _name.CopyToHeap();
	g_model_assets.Push(asset);

	StartCoroutine([=](CoroutineHandle coro) mutable {
		ScratchArenaView scratch = Arena::Scratch();
		String file_path = Format(scratch.arena, "./res/", asset->name, ".obj");
		void* data;
		size_t size;

		Sleep(coro, 3);
		Yield(coro);

		asset->state = AssetState::STREAMING;
		File::ReadEntireFileAsync(coro, file_path, &data, &size);
		asset->state = AssetState::PROCESSING;
		asset->state = AssetState::LOADED;
		printf("Signalling fence...\n");
		SignalFence(asset->fence_on_load);
		printf("Signalled fence fence...\n");
	});

	return asset;
}

}