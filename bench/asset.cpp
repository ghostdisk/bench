#include <bench/core/arena.hpp>
#include <bench/core/format.hpp>
#include <bench/core/string.hpp>
#include <bench/core/file.hpp>
#include <bench/asset.hpp>
#include <bench/coroutine.hpp>
#include <bench/arraylist.hpp>
#include <bench/core/time.hpp>
#include <vendor/fast_obj.h>
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
		String file_path = Format(scratch.arena, "./res/meshes/", asset->name, ".obj");
		void* data;
		size_t size;

		asset->state = AssetState::STREAMING;
		fastObjMesh* mesh = fast_obj_read(scratch.arena.InternCString(file_path));
		if (mesh) {
			int t = 3;
		}
		else {

			Format(File::StdErr, "[Error] Failed to read model asset: ", file_path, "\n");
			asset->state = AssetState::ERROR;
		}
		SignalFence(asset->fence_on_load);

	});

	return asset;
}

}