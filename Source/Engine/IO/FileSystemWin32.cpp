#include "Engine/IO/FileSystem.h"
#include <Windows.h>

namespace Hx {

    class FileHandleWin32 final : public FileHandle {
    public:
        FileHandleWin32(HANDLE handle);

        bool Read(void* dst, usize bytesToRead);
        bool ReadAt(void* dst, usize bytesToRead, usize offset);
        bool Write(const void* src, usize bytesToWrite);
        void Seek(usize position, FileSeek seekMode);
        usize Tell() const;
        usize GetSize() const;

        HANDLE handle;
    };

    FileHandleWin32::FileHandleWin32(HANDLE inHandle) : handle(inHandle) {

    }

    bool FileHandleWin32::Read(void* dst, usize bytesToRead) {
        DWORD bytesRead;
        bool result = ReadFile(handle, dst, static_cast<DWORD>(bytesToRead), &bytesRead, nullptr);
        return result && bytesRead == bytesToRead;
    }

    bool FileHandleWin32::ReadAt(void* dst, usize bytesToRead, usize offset) {
        DWORD oldPos = SetFilePointer(handle, static_cast<LONG>(offset), nullptr, FILE_BEGIN);
        if (oldPos == INVALID_SET_FILE_POINTER) {
            return false;
        }

        DWORD bytesRead;
        bool result = ReadFile(handle, dst, static_cast<DWORD>(bytesToRead), &bytesRead, nullptr);
        return result && bytesRead == bytesToRead;
    }

    bool FileHandleWin32::Write(const void* src, usize bytesToWrite) {
        DWORD bytesWritten;
        bool result = WriteFile(handle, src, static_cast<DWORD>(bytesToWrite), &bytesWritten, nullptr);
        return result && bytesWritten == bytesToWrite;
    }

    void FileHandleWin32::Seek(usize position, FileSeek seekMode) {
        DWORD moveMethod = FILE_BEGIN;
        switch (seekMode) {
        case FileSeek::Begin:
            moveMethod = FILE_BEGIN;
            break;
        case FileSeek::Current:
            moveMethod = FILE_CURRENT;
            break;
        case FileSeek::End:
            moveMethod = FILE_END;
            break;
        }

        SetFilePointer(handle, static_cast<LONG>(position), nullptr, moveMethod);
    }

    usize FileHandleWin32::Tell() const {
        DWORD position = SetFilePointer(handle, 0, nullptr, FILE_CURRENT);
        return static_cast<usize>(position);
    }

    usize FileHandleWin32::GetSize() const {
        LARGE_INTEGER size;
        GetFileSizeEx(handle, &size);
        return static_cast<usize>(size.QuadPart);
    }

    FileHandle* FileSystem::OpenFileRead(const char* filename) {
        HANDLE handle = CreateFileA(filename, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            return nullptr;
        }
        return new FileHandleWin32(handle);
    }

    FileHandle* FileSystem::OpenFileWrite(const char* filename) {
        HANDLE handle = CreateFileA(filename, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (handle == INVALID_HANDLE_VALUE) {
            return nullptr;
        }
        return new FileHandleWin32(handle);
    }

    void FileSystem::CloseFile(FileHandle* file) {
        if (file) {
            FileHandleWin32* winFile = static_cast<FileHandleWin32*>(file);
            CloseHandle(winFile->handle);
            delete file;
        }
    }

    bool FileSystem::IsOpen(FileHandle* file) const {
        return file != nullptr;
    }

    bool FileSystem::FileExists(const char* filename) {
        DWORD attributes = GetFileAttributesA(filename);
        return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
    }

}