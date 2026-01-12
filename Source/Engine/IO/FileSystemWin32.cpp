#include "Engine/IO/FileSystem.h"
#include <Windows.h>

namespace Engine::IO {

    class FileHandleWin32 final : public FileHandle {
    public:
        FileHandleWin32(HANDLE handle);

        bool Read(void* Dst, usize BytesToRead);
        bool ReadAt(void* Dst, usize BytesToRead, usize Offset);
        bool Write(const void* Src, usize BytesToWrite);
        void Seek(usize Position, FileSeek SeekMode);
        usize Tell() const;
        usize GetSize() const;

        HANDLE Handle;
    };

    FileHandleWin32::FileHandleWin32(HANDLE InHandle) : Handle(InHandle) {

    }

    bool FileHandleWin32::Read(void* Dst, usize BytesToRead) {
        DWORD BytesRead;
        bool Result = ReadFile(Handle, Dst, static_cast<DWORD>(BytesToRead), &BytesRead, nullptr);
        return Result && BytesRead == BytesToRead;
    }

    bool FileHandleWin32::ReadAt(void* Dst, usize BytesToRead, usize Offset) {
        DWORD OldPos = SetFilePointer(Handle, static_cast<LONG>(Offset), nullptr, FILE_BEGIN);
        if (OldPos == INVALID_SET_FILE_POINTER) {
            return false;
        }

        DWORD BytesRead;
        bool Result = ReadFile(Handle, Dst, static_cast<DWORD>(BytesToRead), &BytesRead, nullptr);
        return Result && BytesRead == BytesToRead;
    }

    bool FileHandleWin32::Write(const void* Src, usize BytesToWrite) {
        DWORD BytesWritten;
        bool Result = WriteFile(Handle, Src, static_cast<DWORD>(BytesToWrite), &BytesWritten, nullptr);
        return Result && BytesWritten == BytesToWrite;
    }

    void FileHandleWin32::Seek(usize Position, FileSeek SeekMode) {
        DWORD MoveMethod = FILE_BEGIN;
        switch (SeekMode) {
        case FileSeek::Begin:
            MoveMethod = FILE_BEGIN;
            break;
        case FileSeek::Current:
            MoveMethod = FILE_CURRENT;
            break;
        case FileSeek::End:
            MoveMethod = FILE_END;
            break;
        }

        SetFilePointer(Handle, static_cast<LONG>(Position), nullptr, MoveMethod);
    }

    usize FileHandleWin32::Tell() const {
        DWORD Position = SetFilePointer(Handle, 0, nullptr, FILE_CURRENT);
        return static_cast<usize>(Position);
    }

    usize FileHandleWin32::GetSize() const {
        LARGE_INTEGER Size;
        GetFileSizeEx(Handle, &Size);
        return static_cast<usize>(Size.QuadPart);
    }

    FileHandle* FileSystem::OpenFileRead(const char* Filename) {
        HANDLE Handle = CreateFileA(Filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (Handle == INVALID_HANDLE_VALUE) {
            return nullptr;
        }
        return new FileHandleWin32(Handle);
    }

    FileHandle* FileSystem::OpenFileWrite(const char* Filename) {
        HANDLE Handle = CreateFileA(Filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (Handle == INVALID_HANDLE_VALUE) {
            return nullptr;
        }
        return new FileHandleWin32(Handle);
    }

    void FileSystem::CloseFile(FileHandle* File) {
        if (File) {
            FileHandleWin32* WinFile = static_cast<FileHandleWin32*>(File);
            CloseHandle(WinFile->Handle);
            delete File;
        }
    }

}