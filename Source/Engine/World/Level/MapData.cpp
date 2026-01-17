#include "MapData.h"
#include "Engine/Memory/ArenaAllocator.h"
#include "Engine/IO/FileSystem.h"

namespace Engine::World {

    template <typename T>
    inline void ReadLumpData(IO::FileHandle* File, const LumpHeader& Lump, T*& OutData, usize& OutCount, Memory::ArenaAllocator& Arena) {
        OutCount = Lump.Length / sizeof(T);
        OutData = Memory::AllocArray<T>(&Arena.Base, OutCount);
        File->ReadAt(OutData, Lump.Length, Lump.Offset);
    }

    MapData* LoadMapFromFile(const char* Filename, IO::FileSystem& FileSystem, Memory::ArenaAllocator& TransientArena) {
        IO::FileHandle* File = FileSystem.OpenFileRead(Filename);
        if (!File) {
            return nullptr;
        }

        MapHeader Header;
        if (!File->Read(&Header, sizeof(MapHeader))) {
            FileSystem.CloseFile(File);
            return nullptr;
        }

        MapData* Map = Memory::AllocOne<MapData>(&TransientArena.Base, Memory::AllocFlags::ZeroInit);

        ReadLumpData(File, Header.LineSegsLump, Map->LineSegments, Map->LineSegmentCount, TransientArena);
        ReadLumpData(File, Header.EdgesLump, Map->Edges, Map->EdgeCount, TransientArena);
        ReadLumpData(File, Header.SubSectorsLump, Map->Subsectors, Map->SubsectorCount, TransientArena);
        ReadLumpData(File, Header.SectorsLump, Map->Sectors, Map->SectorCount, TransientArena);

        FileSystem.CloseFile(File);

        return Map;
    }

}