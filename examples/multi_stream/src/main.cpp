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

using log_t    = lal::Log<lal::CategoryFilterNone, lal::Ordering::Disabled>;
using stream_t = log_t::stream_t;

void write(stream_t& stream, const size_t count)
{
    for (size_t i = 0; i < count; i++) stream.message<BasicMessage>(i);
}

void logFunc(const std::filesystem::path& path)
{
    auto log = log_t(path, 4096);

    // Create multiple streams with different buffer sizes.
    auto& stream0 = log.createStream(4096);
    auto& stream1 = log.createStream(2048);
    auto& stream2 = log.createStream(1024);
    auto& stream3 = log.createStream(1024);
    auto& stream4 = log.createStream(1024);

    // Write to streams concurrently from different threads.
    std::thread t0 = std::thread([&stream0]() { write(stream0, 1 << 16); });
    std::thread t1 = std::thread([&stream1]() { write(stream1, 1 << 15); });
    std::thread t2 = std::thread([&stream2]() { write(stream2, 1 << 14); });
    std::thread t3 = std::thread([&stream3]() { write(stream3, 1 << 13); });
    std::thread t4 = std::thread([&stream4]() { write(stream4, 1 << 12); });
    t0.join();
    t1.join();
    t2.join();
    t3.join();
    t4.join();
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