////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <iostream>
#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "logandload/format/formatter.h"
#include "logandload/log/log.h"

// Use bits 0-2 to specify severity.
static constexpr uint32_t info    = 0;
static constexpr uint32_t warning = 1;
static constexpr uint32_t error   = 2;
static constexpr uint32_t fatal   = 3;

// Use bits 3-4 to specify some category value.
static constexpr uint32_t categoryA = 8;
static constexpr uint32_t categoryB = 16;

// Allow filtering by severity level and category flag.
template<uint32_t Severity, uint32_t Categories>
struct Filter
{
    template<typename F>
    static consteval bool message()
    {
        // Disable messages with a lower severity.
        if ((F::category & 7) < Severity) return false;

        // Disable messages without a matching category.
        if ((F::category & ~7 & Categories) == 0) return false;

        return true;
    }

    static consteval bool region() { return true; }

    static consteval bool source() { return true; }
};

// Error message.
struct DivisionByZero
{
    static constexpr char     message[] = "Dividing by 0";
    static constexpr uint32_t category  = error | categoryA;
};

// Warning message.
struct NegativeNumber
{
    static constexpr char     message[] = "Dividing by a negative number";
    static constexpr uint32_t category  = warning | categoryB;
};

// By default filter out no messages. Modify parameters to Filter to change this.
using log_t = lal::Log<Filter<info, categoryA | categoryB>, lal::Ordering::Disabled>;

void logFunc(const std::filesystem::path& path)
{
    auto  log    = log_t(path, 4096);
    auto& stream = log.createStream(4096);

    // Iterate over the range [-1, 10] and try to do some divisions.
    for (const auto v : std::views::iota(-1, 10))
    {
        // Can't divide by 0, so log the error and skip.
        if (v == 0)
        {
            stream.message<DivisionByZero>();
            continue;
        }

        // Can divide by a value < 0, but perhaps the sign value is unexpected. Log a warning.
        if (v < 0) stream.message<NegativeNumber>();

        std::cout << "1000 / " << v << " = " << 1000 / v << std::endl;
    }
}

void fmtFunc(const std::filesystem::path& path)
{
    auto formatter = lal::Formatter();

    // Set custom format function to properly print severity and category A/B.
    formatter.categoryFormatter = [](std::ostream& out, const uint32_t category) {
        const auto severity = category & 3;

        switch (severity)
        {
        case info: out << "INFO   "; break;
        case warning: out << "WARNING"; break;
        case error: out << "ERROR  "; break;
        case fatal: out << "FATAL  "; break;
        default: break;
        }

        if (category & categoryA)
            out << " (A)";
        else if (category & categoryB)
            out << " (B)";

        out << " | ";
    };

    formatter.format(path);
}

int main(int, char**)
{
    const auto path = std::filesystem::current_path() / "log.bin";

    logFunc(path);

    fmtFunc(path);

    return 0;
}
