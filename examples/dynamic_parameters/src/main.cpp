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

struct SingleParam
{
    static constexpr char     message[] = "{}";
    static constexpr uint32_t category  = 0;
};

struct MultiParam
{
    static constexpr char     message[] = "Halfway [{}, {}]";
    static constexpr uint32_t category  = 0;
};

// Custom type we are going to log.
struct Range
{
    uint32_t start = 0;
    uint32_t end   = 0;
};

using log_t = lal::Log<lal::CategoryFilterNone, lal::Ordering::Disabled>;

void logFunc(const std::filesystem::path& path)
{
    auto  log    = log_t(path, 4096);
    auto& stream = log.createStream(4096);

    // Iterate over the range [0, 10] and just log the values.
    constexpr Range range{0, 10};
    stream.message<SingleParam>(range);
    for (const auto v : std::views::iota(range.start, range.end))
    {
        if (v == (range.start + range.end) / 2) stream.message<MultiParam>(range.start, range.end);
        stream.message<SingleParam>(v);
        std::cout << v << std::endl;
    }
}

void fmtFunc(const std::filesystem::path& path)
{
    auto formatter = lal::Formatter();

    // All integer types are registered automatically. Our Range type isn't, so we do that here.
    formatter.registerParameter<Range>(
      [](std::ostream& out, const Range& range) { out << "[" << range.start << ", " << range.end << "]"; });

    formatter.format(path);
}

int main(int, char**)
{
    const auto path = std::filesystem::current_path() / "log.bin";

    logFunc(path);

    fmtFunc(path);

    return 0;
}