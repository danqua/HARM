#pragma once

#include "Engine/Core/Types.h"
#include "Engine/Core/Handle.h"

namespace Hx {

    enum class FileSeek {
        Begin,
        Current,
        End
    };

    class FileHandle {
    public:
        virtual bool Read(void* Dst, usize BytesToRead) = 0;
        virtual bool ReadAt(void* Dst, usize BytesToRead, usize Offset) = 0;

        virtual bool Write(const void* Src, usize BytesToWrite) = 0;

        virtual void Seek(usize Position, FileSeek SeekMode) = 0;
        virtual usize Tell() const = 0;

        virtual usize GetSize() const = 0;
    };

    class FileSystem {
    public:
        FileHandle* OpenFileRead(const char* Filename);
        FileHandle* OpenFileWrite(const char* Filename);
        void CloseFile(FileHandle* File);

        bool IsOpen(FileHandle* File) const;
        bool FileExists(const char* Filename);
    };

}