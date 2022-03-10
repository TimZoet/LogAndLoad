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

struct Message
{
    static constexpr char     message[] = "{} * {} = {}";
    static constexpr uint32_t category  = 0;
};

struct RegionName
{
    static constexpr char     message[] = "Inner Loop";
    static constexpr uint32_t category  = 0;
};

using log_t = lal::Log;

void logFunc(const std::filesystem::path& path)
{
    auto  log    = log_t(path, 4096);
    auto& stream = log.createStream(4096);

    // Create an anonymous region wrapping all messages.
    auto outer = stream.region();

    for (const auto i : std::views::iota(0, 5))
    {
        // Create a named region wrapping the inner loop.
        auto inner = stream.region<RegionName>();

        for (const auto j : std::views::iota(0, 5))
        {
            stream.message<Message>(i, j, i * j);
            std::cout << i << " * " << j << " = " << i * j << std::endl;
        }
    }
}

void fmtFunc(const std::filesystem::path& path)
{
    auto formatter                     = lal::Formatter();
    formatter.anonymousRegionFormatter = [](std::ostream& out, const bool start) {
        if (start)
            out << "<region>";
        else
            out << "</region>";
    };
    formatter.namedRegionFormatter = [](std::ostream& out, const bool start, const std::string& name) {
        if (start)
            out << "<" << name << ">";
        else
            out << "</" << name << ">";
    };
    formatter.regionIndent          = 4;
    formatter.regionIndentCharacter = '.';
    formatter.format(path);
}

int main(int, char**)
{
    const auto path = std::filesystem::current_path() / "log.bin";

    logFunc(path);

    fmtFunc(path);

    return 0;
}