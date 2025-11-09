#pragma once
#include <bench/core/common.hpp>
#include <bench/core/string.hpp>

namespace bench {

struct String;
struct Coroutine;

enum class AssetState {
    UNLOADED,
    STREAMING,
    PROCESSING,
    LOADED,
};

class Asset {
public:
    String name;
    AssetState state = AssetState::UNLOADED;

    virtual ~Asset();
};

class ModelAsset : public Asset {
public:
    static ModelAsset* Get(String name);
};

};