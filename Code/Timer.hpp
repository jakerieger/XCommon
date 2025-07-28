#pragma once

#include <chrono>
#include <iostream>

namespace x {
    class Timer {
        std::chrono::time_point<std::chrono::high_resolution_clock> mStart;

    public:
        Timer() { Reset(); }

        void Reset() { mStart = std::chrono::high_resolution_clock::now(); }

        f32 Elapsed() const {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * 0.001f * 0.001f;
        }

        f32 ElapsedMillis() const {
            return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - mStart).count() * 0.001f;
        }
    };

    class ScopedTimer {
        Timer mTimer;
        std::string mName;

    public:
        ScopedTimer(std::string_view name) : mName(name) {}

        ~ScopedTimer() {
            f32 time = mTimer.ElapsedMillis();
            std::cout << mName << ": " << time << " ms\n";
        }
    };
}  // namespace x