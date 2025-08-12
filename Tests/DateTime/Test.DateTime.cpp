// Author: Jake Rieger
// Created: 8/12/2025.
//

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include "DateTime.hpp"
#include <chrono>
#include <thread>
#include <regex>

using namespace x;
using namespace std::chrono;

TEST_CASE("DateTime construction and basic functionality", "[DateTime][construction]") {
    SECTION("Can create DateTime from timepoint") {
        auto now = system_clock::now();
        DateTime dt(now);

        REQUIRE(dt.TimePoint() == now);
    }

    SECTION("DateTime::Now() creates current time") {
        auto before = system_clock::now();
        auto dt = DateTime::Now();
        auto after = system_clock::now();

        // The DateTime should be created between before and after
        REQUIRE(dt.TimePoint() >= before);
        REQUIRE(dt.TimePoint() <= after);
    }

    SECTION("Multiple calls to Now() show time progression") {
        auto dt1 = DateTime::Now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto dt2 = DateTime::Now();

        REQUIRE(dt2.TimePoint() > dt1.TimePoint());
    }
}

TEST_CASE("DateTime string formatting", "[DateTime][formatting]") {
    // Create a known timestamp: January 15, 2024, 14:30:45 UTC
    std::tm tm = {};
    tm.tm_sec = 45;
    tm.tm_min = 30;
    tm.tm_hour = 14;
    tm.tm_mday = 15;
    tm.tm_mon = 0;  // January (0-based)
    tm.tm_year = 124; // 2024 (years since 1900)
    tm.tm_isdst = -1;

    auto time_t = std::mktime(&tm);

    // Convert back to system_clock time_point
    auto timepoint = system_clock::from_time_t(time_t);
    DateTime dt(timepoint);

    SECTION("DateString format") {
        std::string dateStr = dt.DateString();

        // Should match YYYY-MM-DD format
        REQUIRE_THAT(dateStr, Catch::Matchers::Matches(R"(\d{4}-\d{2}-\d{2})"));

        // For the specific date we created
        REQUIRE_THAT(dateStr, Catch::Matchers::ContainsSubstring("2024"));
        REQUIRE_THAT(dateStr, Catch::Matchers::ContainsSubstring("-01-")); // January
        REQUIRE_THAT(dateStr, Catch::Matchers::ContainsSubstring("-15"));  // 15th day
    }

    SECTION("TimeString format") {
        std::string timeStr = dt.TimeString();

        // Should match HH:MM:SS AM/PM format
        REQUIRE_THAT(timeStr, Catch::Matchers::Matches(R"(\d{2}:\d{2}:\d{2} (AM|PM))"));

        // Should contain the time components
        REQUIRE_THAT(timeStr, Catch::Matchers::ContainsSubstring(":30:45"));
        REQUIRE_THAT(timeStr, Catch::Matchers::ContainsSubstring("PM"));
    }

    SECTION("LocalString format") {
        std::string localStr = dt.LocalString();

        // Should match YYYY-MM-DD HH:MM:SS AM/PM format
        REQUIRE_THAT(localStr, Catch::Matchers::Matches(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} (AM|PM))"));

        // Should contain date and time components
        REQUIRE_THAT(localStr, Catch::Matchers::ContainsSubstring("2024"));
        REQUIRE_THAT(localStr, Catch::Matchers::ContainsSubstring(" PM"));
    }

    SECTION("UTCString format") {
        std::string utcStr = dt.UTCString();

        // Should match YYYY-MM-DD HH:MM:SS AM/PM format
        REQUIRE_THAT(utcStr, Catch::Matchers::Matches(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} (AM|PM))"));

        // Should contain basic format
        REQUIRE_THAT(utcStr, Catch::Matchers::ContainsSubstring("2024"));
    }
}

TEST_CASE("DateTime 12-hour format conversion", "[DateTime][time-format]") {
    SECTION("Morning hours (AM)") {
        // Test 8:15:30 AM
        std::tm tm = {};
        tm.tm_sec = 30;
        tm.tm_min = 15;
        tm.tm_hour = 8;
        tm.tm_mday = 1;
        tm.tm_mon = 0;
        tm.tm_year = 124;
        tm.tm_isdst = -1;

        auto time_t = std::mktime(&tm);

        DateTime dt(system_clock::from_time_t(time_t));
        std::string timeStr = dt.TimeString();

        REQUIRE_THAT(timeStr, Catch::Matchers::ContainsSubstring("08:15:30 AM"));
    }

    SECTION("Noon (12 PM)") {
        std::tm tm = {};
        tm.tm_sec = 0;
        tm.tm_min = 0;
        tm.tm_hour = 12;
        tm.tm_mday = 1;
        tm.tm_mon = 0;
        tm.tm_year = 124;
        tm.tm_isdst = -1;

        auto time_t = std::mktime(&tm);

        DateTime dt(system_clock::from_time_t(time_t));
        std::string timeStr = dt.TimeString();

        REQUIRE_THAT(timeStr, Catch::Matchers::ContainsSubstring("12:00:00 PM"));
    }

    SECTION("Midnight (12 AM)") {
        std::tm tm = {};
        tm.tm_sec = 0;
        tm.tm_min = 0;
        tm.tm_hour = 0;
        tm.tm_mday = 1;
        tm.tm_mon = 0;
        tm.tm_year = 124;
        tm.tm_isdst = -1;

        auto time_t = std::mktime(&tm);

        DateTime dt(system_clock::from_time_t(time_t));
        std::string timeStr = dt.TimeString();

        REQUIRE_THAT(timeStr, Catch::Matchers::ContainsSubstring("12:00:00 AM"));
    }

    SECTION("Afternoon hours (PM)") {
        // Test 3:45:15 PM (15:45:15 in 24-hour)
        std::tm tm = {};
        tm.tm_sec = 15;
        tm.tm_min = 45;
        tm.tm_hour = 15;
        tm.tm_mday = 1;
        tm.tm_mon = 0;
        tm.tm_year = 124;
        tm.tm_isdst = -1;

        auto time_t = std::mktime(&tm);

        DateTime dt(system_clock::from_time_t(time_t));
        std::string timeStr = dt.TimeString();

        REQUIRE_THAT(timeStr, Catch::Matchers::ContainsSubstring("03:45:15 PM"));
    }
}

TEST_CASE("DateTime edge cases", "[DateTime][edge-cases]") {
    SECTION("Leap year February 29th") {
        // February 29, 2024 (leap year)
        std::tm tm = {};
        tm.tm_sec = 0;
        tm.tm_min = 0;
        tm.tm_hour = 12;
        tm.tm_mday = 29;
        tm.tm_mon = 1;  // February
        tm.tm_year = 124; // 2024
        tm.tm_isdst = -1;

        auto time_t = std::mktime(&tm);

        DateTime dt(system_clock::from_time_t(time_t));
        std::string dateStr = dt.DateString();

        REQUIRE_THAT(dateStr, Catch::Matchers::ContainsSubstring("2024-02-29"));
    }

    SECTION("Year boundaries") {
        // December 31, 2023 23:59:59
        std::tm tm = {};
        tm.tm_sec = 59;
        tm.tm_min = 59;
        tm.tm_hour = 23;
        tm.tm_mday = 31;
        tm.tm_mon = 11; // December
        tm.tm_year = 123; // 2023
        tm.tm_isdst = -1;

        auto time_t = std::mktime(&tm);

        DateTime dt(system_clock::from_time_t(time_t));
        std::string localStr = dt.LocalString();

        REQUIRE_THAT(localStr, Catch::Matchers::ContainsSubstring("2023-12-31"));
        REQUIRE_THAT(localStr, Catch::Matchers::ContainsSubstring("11:59:59 PM"));
    }
}

TEST_CASE("DateTime consistency", "[DateTime][consistency]") {
    SECTION("Same timepoint produces consistent strings") {
        auto timepoint = system_clock::now();
        DateTime dt1(timepoint);
        DateTime dt2(timepoint);

        REQUIRE(dt1.DateString() == dt2.DateString());
        REQUIRE(dt1.TimeString() == dt2.TimeString());
        REQUIRE(dt1.LocalString() == dt2.LocalString());
        REQUIRE(dt1.UTCString() == dt2.UTCString());
    }

    SECTION("LocalString contains DateString and TimeString components") {
        auto dt = DateTime::Now();

        std::string localStr = dt.LocalString();
        std::string dateStr = dt.DateString();
        std::string timeStr = dt.TimeString();

        REQUIRE_THAT(localStr, Catch::Matchers::ContainsSubstring(dateStr));
        REQUIRE_THAT(localStr, Catch::Matchers::ContainsSubstring(timeStr));
    }
}

TEST_CASE("DateTime format validation", "[DateTime][validation]") {
    auto dt = DateTime::Now();

    SECTION("All string outputs are non-empty") {
        REQUIRE_FALSE(dt.DateString().empty());
        REQUIRE_FALSE(dt.TimeString().empty());
        REQUIRE_FALSE(dt.LocalString().empty());
        REQUIRE_FALSE(dt.UTCString().empty());
    }

    SECTION("String formats match expected patterns") {
        // Date: YYYY-MM-DD
        REQUIRE_THAT(dt.DateString(),
                    Catch::Matchers::Matches(R"(\d{4}-\d{2}-\d{2})"));

        // Time: HH:MM:SS AM/PM
        REQUIRE_THAT(dt.TimeString(),
                    Catch::Matchers::Matches(R"(\d{2}:\d{2}:\d{2} (AM|PM))"));

        // DateTime: YYYY-MM-DD HH:MM:SS AM/PM
        REQUIRE_THAT(dt.LocalString(),
                    Catch::Matchers::Matches(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} (AM|PM))"));

        REQUIRE_THAT(dt.UTCString(),
                    Catch::Matchers::Matches(R"(\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2} (AM|PM))"));
    }
}