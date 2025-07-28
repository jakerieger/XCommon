// Author: Jake Rieger
// Created: 12/12/24.
//

#pragma once

#include "Typedefs.hpp"
#include "Macros.hpp"
#include <fstream>
#include <vector>
#include <span>
#include <future>

#ifdef _WIN32
    #ifndef NOMINMAX
        #define NOMINMAX
    #endif

    #ifndef WIN32_LEAN_AND_MEAN
        #define WIN32_LEAN_AND_MEAN
    #endif

    #include <Windows.h>
    #include <direct.h>
    #define getcwd _getcwd
    #define PATH_SEPARATOR '\\'
#else
    #include <unistd.h>
    #define PATH_SEPARATOR '/'
#endif

namespace x {
    class Path;

    class FileReader {
    public:
        static std::vector<u8> ReadBytes(const Path& path);
        static str ReadText(const Path& path);
        static std::vector<str> ReadLines(const Path& path);
        static std::vector<u8> ReadBlock(const Path& path, size_t size, u64 offset = 0);
        static size_t QueryFileSize(const Path& path);
    };

    class FileWriter {
    public:
        static bool WriteBytes(const Path& path, const std::vector<u8>& data);
        static bool WriteText(const Path& path, const str& text);
        static bool WriteLines(const Path& path, const std::vector<str>& lines);
        static bool WriteBlock(const Path& path, const std::span<const u8>& data, u64 offset = 0);
    };

    class AsyncFileReader {
    public:
        static std::future<std::vector<u8>> ReadBytes(const Path& path);
        static std::future<str> ReadText(const Path& path);
        static std::future<std::vector<str>> ReadLines(const Path& path);
        static std::future<std::vector<u8>> ReadBlock(const Path& path, size_t size, u64 offset = 0);

    private:
        template<typename Func>
        static auto RunAsync(Func&& func) -> std::future<decltype(func())> {
            using ReturnType = decltype(func());
            auto task        = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<Func>(func));
            std::future<ReturnType> future = task->get_future();
            std::thread([task]() { (*task)(); }).detach();
            return future;
        }
    };

    class AsyncFileWriter {
    public:
        static std::future<bool> WriteBytes(const Path& path, const std::vector<u8>& data);
        static std::future<bool> WriteText(const Path& path, const str& text);
        static std::future<bool> WriteLines(const Path& path, const std::vector<str>& lines);
        static std::future<bool> WriteBlock(const Path& path, const std::span<const u8>& data, u64 offset = 0);

    private:
        template<typename Func>
        static auto RunAsync(Func&& func) -> std::future<decltype(func())> {
            using ReturnType = decltype(func());
            auto task        = std::make_shared<std::packaged_task<ReturnType()>>(std::forward<Func>(func));
            std::future<ReturnType> future = task->get_future();
            std::thread([task]() { (*task)(); }).detach();
            return future;
        }
    };

    class StreamReader {
    public:
        explicit StreamReader(const Path& path);
        ~StreamReader();

        StreamReader(const StreamReader&)            = delete;
        StreamReader& operator=(const StreamReader&) = delete;

        StreamReader(StreamReader&&) noexcept;
        StreamReader& operator=(StreamReader&&) noexcept;

        bool Read(std::vector<u8>& data, size_t size);
        bool ReadAll(std::vector<u8>& data);
        bool ReadLine(str& line);

        X_NODISCARD bool IsOpen() const;
        X_NODISCARD size_t Size() const;

        bool Seek(u64 offset);
        u64 Position();
        void Close();

    private:
        std::ifstream mStream;
        size_t mSize = 0;
    };

    class StreamWriter {
    public:
        explicit StreamWriter(const Path& path, bool append = false);
        ~StreamWriter();

        StreamWriter(const StreamWriter&)            = delete;
        StreamWriter& operator=(const StreamWriter&) = delete;

        StreamWriter(StreamWriter&&) noexcept;
        StreamWriter& operator=(StreamWriter&&) noexcept;

        bool Write(const std::vector<u8>& buffer);
        bool Write(const std::vector<u8>& buffer, size_t size);
        bool WriteLine(const str& line);
        bool Flush();

        bool IsOpen() const;
        bool Seek(u64 offset);
        u64 Position();
        void Close();

    private:
        std::ofstream mStream;
    };

    class DirectoryIterator;
    class DirectoryEntries;

    class Path {
    public:
        Path() = default;
        explicit Path(const str& path) : mPath(Normalize(path)) {}
        static Path Current();

        X_NODISCARD Path Parent() const;
        X_NODISCARD bool Exists() const;
        X_NODISCARD bool IsFile() const;
        X_NODISCARD bool IsDirectory() const;
        X_NODISCARD bool HasExtension() const;

        /// @brief Returns the file extension without the period '.'
        ///
        /// i.e. 'txt' or 'jpeg'
        X_NODISCARD str Extension() const;
        X_NODISCARD Path ReplaceExtension(const str& ext) const;
        X_NODISCARD Path Join(const str& subPath) const;
        Path& Join(const str& subPath);
        X_NODISCARD str Str() const;
        X_NODISCARD const char* CStr() const;
        X_NODISCARD str Filename() const;
        X_NODISCARD Path RelativeTo(const Path& basePath) const;
        X_NODISCARD str BaseName() const;

        X_NODISCARD Path operator/(const str& subPath) const;
        X_NODISCARD bool operator==(const Path& other) const;
        friend std::ostream& operator<<(std::ostream& os, const Path& path);

        X_NODISCARD bool Create() const;
        X_NODISCARD bool CreateAll() const;
        X_NODISCARD bool Copy(const Path& dest) const;
        X_NODISCARD bool CopyDirectory(const Path& dest) const;

        X_NODISCARD DirectoryEntries Entries() const;

    private:
        str mPath;
        static str Join(const str& lhs, const str& rhs);
        static str Normalize(const str& rawPath);
    };

    class FindHandleWrapper {
    public:
        FindHandleWrapper();

        explicit FindHandleWrapper(HANDLE handle);

        ~FindHandleWrapper();

        FindHandleWrapper(const FindHandleWrapper&)            = delete;
        FindHandleWrapper& operator=(const FindHandleWrapper&) = delete;

        FindHandleWrapper(FindHandleWrapper&& other) noexcept;
        FindHandleWrapper& operator=(FindHandleWrapper&& other) noexcept;

        HANDLE Get() const;
        bool IsValid() const;

    private:
        HANDLE mHandle;
    };

    class DirectoryEntries {
    public:
        explicit DirectoryEntries(const Path& path);

        DirectoryIterator begin();
        DirectoryIterator end();

    private:
        Path mPath;
    };

    class DirectoryIterator {
    public:
        using iterator_category = std::input_iterator_tag;
        using value_type        = Path;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const Path*;
        using reference         = const Path&;

        DirectoryIterator();
        explicit DirectoryIterator(const Path& path);

        reference operator*() const;
        pointer operator->() const;
        DirectoryIterator& operator++();

        // Disallow post-increment operator
        DirectoryIterator& operator++(int) = delete;

        bool operator==(const DirectoryIterator& other) const;
        bool operator!=(const DirectoryIterator& other) const;

    private:
        bool mIsEnd;
        Path mRoot;
        Path mCurrent;
        FindHandleWrapper mFindHandle;

        void ProcessCurrentEntry(const WIN32_FIND_DATAA& findData);
    };
}  // namespace x