#include <benchmark/benchmark.h>
#include <fmt/ranges.h>

#include <array>
#include <vector>

#include "bayer.h"

namespace {
// constexpr int width = 8256, height = 5504;
constexpr int width = 4228, height = 2848;
std::array<uint8_t, width * height> bayer_8{};
std::array<uint8_t, width * height * 3> rgb_8{};
std::array<uint16_t, width * height> bayer_16{};
std::array<uint16_t, width * height * 3> rgb_16{};
}; // namespace

static void BM_8bpp(benchmark::State &state) {
  for (auto _ : state) {
    dc1394_bayer_decoding_8bit(
        bayer_8.data(), rgb_8.data(), width, height,
        static_cast<dc1394color_filter_t>(DC1394_COLOR_FILTER_RGGB),
        static_cast<dc1394bayer_method_t>(state.range(0)));
  }
}
BENCHMARK(BM_8bpp)
    ->Args({
        DC1394_BAYER_METHOD_NEAREST,
    })
    ->Args({
        DC1394_BAYER_METHOD_SIMPLE,
    })
    ->Args({
        DC1394_BAYER_METHOD_BILINEAR,
    })
    ->Args({
        DC1394_BAYER_METHOD_HQLINEAR,
    })
    ->Args({
        DC1394_BAYER_METHOD_DOWNSAMPLE,
    })
    ->Args({
        DC1394_BAYER_METHOD_EDGESENSE,
    })
    ->Args({
        DC1394_BAYER_METHOD_VNG,
    })
    ->Args({
        DC1394_BAYER_METHOD_AHD,
    });

static void BM_16bpp(benchmark::State &state) {
  for (auto _ : state) {
    dc1394_bayer_decoding_16bit(bayer_16.data(), rgb_16.data(), width, height,
        static_cast<dc1394color_filter_t>(DC1394_COLOR_FILTER_RGGB),
        static_cast<dc1394bayer_method_t>(state.range(0)), 16);
  }
}

BENCHMARK(BM_16bpp)
    ->Args({
        DC1394_BAYER_METHOD_NEAREST,
    })
    ->Args({
        DC1394_BAYER_METHOD_SIMPLE,
    })
    ->Args({
        DC1394_BAYER_METHOD_BILINEAR,
    })
    ->Args({
        DC1394_BAYER_METHOD_HQLINEAR,
    })
    ->Args({
        DC1394_BAYER_METHOD_DOWNSAMPLE,
    })
    ->Args({
        DC1394_BAYER_METHOD_EDGESENSE,
    })
    ->Args({
        DC1394_BAYER_METHOD_VNG,
    })
    ->Args({
        DC1394_BAYER_METHOD_AHD,
    });

BENCHMARK_MAIN();
