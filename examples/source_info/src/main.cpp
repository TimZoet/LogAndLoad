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

using log_t = lal::Log<lal::CategoryFilterNone, lal::Ordering::Disabled>;

void logFunc(const std::filesystem::path& path)
{
    auto                  log    = log_t(path, 4096);
    auto&                 stream = log.createStream(4096);
    static constexpr auto loc0   = std::source_location::current();
    stream.sourceInfo<lal::hash(loc0)>(loc0);
    stream.message<SingleParam>(10);
    static constexpr auto loc1 = std::source_location::current();
    stream.sourceInfo<lal::hash(loc1)>(loc1);
}

void fmtFunc(const std::filesystem::path& path)
{
    auto formatter = lal::Formatter();
    formatter.format(path);
}

int main(int, char**)
{
    const auto path = std::filesystem::current_path() / "log.bin";

    logFunc(path);

    fmtFunc(path);

    return 0;
}