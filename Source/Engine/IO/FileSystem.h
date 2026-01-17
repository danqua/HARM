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
        virtual bool Read(void* dst, usize bytesToRead) = 0;
        virtual bool ReadAt(void* dst, usize bytesToRead, usize offset) = 0;

        virtual bool Write(const void* src, usize bytesToWrite) = 0;

        virtual void Seek(usize position, FileSeek seekMode) = 0;
        virtual usize Tell() const = 0;

        virtual usize GetSize() const = 0;
    };

    class FileSystem {
    public:
        FileHandle* OpenFileRead(const char* filename);
        FileHandle* OpenFileWrite(const char* filename);
        void CloseFile(FileHandle* file);

        bool IsOpen(FileHandle* file) const;
        bool FileExists(const char* filename);
    };

}