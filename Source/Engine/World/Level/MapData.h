#pragma once

#include "Engine/Core/Handle.h"

namespace Hx {
    class FileSystem;
}

namespace Hx {
    struct ArenaAllocator;
}

namespace Hx {

    struct LumpHeader {
        u32 offset;
        u32 length;
    };

    struct MapHeader {
        char identifier[4];
        LumpHeader sectorsLump;
        LumpHeader subSectorsLump;
        LumpHeader lineSegsLump;
        LumpHeader edgesLump;
    };

    struct MapLineSegment {
        s32 v1[2];
        s32 v2[2];
        s32 frontSector;
        s32 backSector;
    };

    struct MapEdge {
        u32 lineSeg;
        u32 reversed;
    };

    struct MapSubsector {
        u32 firstEdge;
        u32 edgeCount;
    };

    struct MapSector {
        u32 firstGroup;
        u32 groupCount;
        s32 floorHeight;
        s32 ceilingHeight;
    };

    struct MapData {
        MapLineSegment* lineSegments;
        usize lineSegmentCount;

        MapEdge* edges;
        usize edgeCount;

        MapSubsector* subsectors;
        usize subsectorCount;

        MapSector* sectors;
        usize sectorCount;
    };

    MapData* LoadMapFromFile(const char* filename, Hx::FileSystem& fileSystem, Hx::ArenaAllocator& transientArena);

}