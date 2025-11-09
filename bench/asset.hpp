#pragma once
#include <bench/common.hpp>
#include <bench/string.hpp>

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