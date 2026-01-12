#pragma once

#include "Engine/Core/Handle.h"
#include "Engine/Memory/ArenaAllocator.h"

namespace Engine::World {

    struct LumpHeader {
        u32 Offset;
        u32 Length;
    };

    struct MapHeader {
        char Identifier[4];
        LumpHeader SectorsLump;
        LumpHeader SubSectorsLump;
        LumpHeader LineSegsLump;
        LumpHeader EdgesLump;
    };

    struct MapLineSegment {
        s32 V1[2];
        s32 V2[2];
        s32 FrontSector;
        s32 BackSector;
    };

    struct MapEdge {
        u32 LineSeg;
        u32 Reversed;
    };

    struct MapSubsector {
        u32 FirstEdge;
        u32 EdgeCount;
    };

    struct MapSector {
        u32 FirstGroup;
        u32 GroupCount;
        s32 FloorHeight;
        s32 CeilingHeight;
    };

}