////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <thread>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "logandload/format/formatter.h"
#include "logandload/log/log.h"

struct BasicMessage
{
    static constexpr char     message[] = "Value = {}";
    static constexpr uint32_t category  = 0;
};

// Enable message ordering.
using log_t    = lal::Log<lal::CategoryFilterNone, lal::Ordering::Enabled>;
using stream_t = log_t::stream_t;

void write(stream_t& stream, const size_t count)
{
    for (size_t i = 0; i < count; i++) stream.message<BasicMessage>(i);
}

void logFunc(const std::filesystem::path& path)
{
    auto log = log_t(path, 4096);

    auto& stream0 = log.createStream(2048);
    auto& stream1 = log.createStream(2048);
    auto  t0      = std::thread([&stream0]() { write(stream0, 1 << 16); });
    auto  t1      = std::thread([&stream1]() { write(stream1, 1 << 16); });
    t0.join();
    t1.join();
}

void fmtFunc(const std::filesystem::path& path)
{
    auto formatter                  = lal::Formatter();
    formatter.indexPaddingWidth     = 6;
    formatter.indexPaddingCharacter = '#';
    formatter.format(path);
}

int main(int, char**)
{
    const auto path = std::filesystem::current_path() / "log.bin";

    logFunc(path);

    fmtFunc(path);

    return 0;
}