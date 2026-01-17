#include "MapData.h"
#include "Engine/Memory/ArenaAllocator.h"
#include "Engine/IO/FileSystem.h"

namespace Hx {

    template <typename T>
    inline void ReadLumpData(FileHandle* file, const LumpHeader& lump, T*& outData, usize& outCount, ArenaAllocator& arena) {
        outCount = lump.length / sizeof(T);
        outData = AllocArray<T>(&arena.base, outCount);
        file->ReadAt(outData, lump.length, lump.offset);
    }

    MapData* LoadMapFromFile(const char* filename, FileSystem& fileSystem, ArenaAllocator& transientArena) {
        FileHandle* file = fileSystem.OpenFileRead(filename);
        if (!file) {
            return nullptr;
        }

        MapHeader header;
        if (!file->Read(&header, sizeof(MapHeader))) {
            fileSystem.CloseFile(file);
            return nullptr;
        }

        MapData* map = AllocOne<MapData>(&transientArena.base, AllocFlags::ZeroInit);

        ReadLumpData(file, header.lineSegsLump, map->lineSegments, map->lineSegmentCount, transientArena);
        ReadLumpData(file, header.edgesLump, map->edges, map->edgeCount, transientArena);
        ReadLumpData(file, header.subSectorsLump, map->subsectors, map->subsectorCount, transientArena);
        ReadLumpData(file, header.sectorsLump, map->sectors, map->sectorCount, transientArena);

        fileSystem.CloseFile(file);

        return map;
    }

}