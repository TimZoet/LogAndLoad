////////////////////////////////////////////////////////////////
// Standard includes.
////////////////////////////////////////////////////////////////

#include <ranges>

////////////////////////////////////////////////////////////////
// Module includes.
////////////////////////////////////////////////////////////////

#include "logandload/analyze/analyzer.h"
#include "logandload/analyze/tree.h"
#include "logandload/format/formatter.h"
#include "logandload/log/log.h"

static constexpr uint32_t info    = 0;
static constexpr uint32_t warning = 1;
static constexpr uint32_t error   = 2;
static constexpr uint32_t fatal   = 3;

template<uint32_t C>
struct NoParam
{
    static constexpr char     message[] = "msg";
    static constexpr uint32_t category  = C;
};

template<uint32_t C>
struct SingleParam
{
    static constexpr char     message[] = "{}";
    static constexpr uint32_t category  = C;
};

template<uint32_t C>
struct MultiParam
{
    static constexpr char     message[] = "{} {} {}";
    static constexpr uint32_t category  = C;
};

struct Region0
{
    static constexpr char     message[] = "Region 0";
    static constexpr uint32_t category  = 0;
};

struct Region1
{
    static constexpr char     message[] = "Region 1";
    static constexpr uint32_t category  = 0;
};

struct float2
{
    float x = 0;
    float y = 0;
};

using log_t = lal::Log<lal::CategoryFilterNone, lal::Ordering::Enabled>;

void logFunc(const std::filesystem::path& path)
{
    auto  log     = log_t(path, 4096);
    auto& stream0 = log.createStream(4096);
    auto& stream1 = log.createStream(4096);

    stream0.message<NoParam<info>>();
    stream1.message<MultiParam<info>>(4000.0f, 64ull, static_cast<std::byte>(255));

    {
        auto region = stream0.region();
        for (const auto v : std::views::iota(0, 10)) { stream0.message<SingleParam<info>>(v); }
    }

    stream1.message<SingleParam<error>>(0);
    for (const auto v : std::views::iota(0, 3)) stream1.message<SingleParam<info>>(static_cast<float>(v) * 3.33f);

    {
        auto region = stream0.region<Region0>();
        stream0.message<SingleParam<info>>(float2{1.11f, 2.22f});
        stream0.message<SingleParam<info>>(float2{3.33f, 4.44f});
    }

    {
        auto region0 = stream1.region<Region1>();
        auto region1 = stream1.region();
        for (const auto v : std::views::iota(0, 5))
            stream1.message<MultiParam<info>>(
              float2{static_cast<float>(v), 9.0f - v}, float2{-static_cast<float>(v), v - 9.0f}, v);
    }
}

void fmtFunc(const std::filesystem::path& path)
{
    auto formatter = lal::Formatter();

    formatter.registerParameter<float2>(
      [](std::ostream& out, const float2& value) { out << "[" << value.x << ", " << value.y << "]"; });

    formatter.format(path);
}

void analyzeFunc(const std::filesystem::path& path)
{
    lal::Analyzer analyzer;
    analyzer.registerParameter<float2>();
    analyzer.read(path);

    lal::Tree tree(analyzer);
    tree.filterRegion([](const lal::Tree::Flags oldFlags, const lal::Node& node) {
        return node.formatType ? lal::Tree::Flags::Enabled : lal::Tree::Flags::Disabled;
    });
    
    tree.filterMessage<SingleParam<info>, int>(
      [](const lal::Tree::Flags oldFlags, const lal::Node& node) {
          if (node.get<int>(0) < 5) return lal::Tree::Flags::Disabled;
          return oldFlags;
      },
      [](const lal::Tree::Flags flags, const lal::Node& node) {
          if (flags == lal::Tree::Flags::Disabled)
          {
              if (node.type == lal::Node::Type::Region && node.formatType == nullptr) return lal::Tree::Action::Skip;
              return lal::Tree::Action::Terminate;
          }
          return lal::Tree::Action::Apply;
      });
    analyzer.writeGraph("log.dot", &tree);
}

int main(int, char**)
{
    const auto path = std::filesystem::current_path() / "log.bin";

    logFunc(path);

    fmtFunc(path);

    analyzeFunc(path);

    return 0;
}