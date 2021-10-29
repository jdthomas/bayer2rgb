#include <fmt/ranges.h>
#include <gtest/gtest.h>

#include <vector>

#include "bayer.h"

TEST(Bayer2RgbTest, TestNN8bpp) {
  // clang-format off
  constexpr int width=6, height=4;
  constexpr uint8_t R=1, G1=2, G2=4, B=5;
  [[maybe_unused]]constexpr uint8_t G=(G1+G2)/2;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    R, G1, B,  R, G2, B,  R, G1, B,  R, G2, B,  R, G1, B,  0, 0, 0,
    R, G1, B,  R, G2, B,  R, G1, B,  R, G2, B,  R, G1, B,  0, 0, 0,
    R, G1, B,  R, G2, B,  R, G1, B,  R, G2, B,  R, G1, B,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto rv = dc1394_bayer_decoding_8bit(tiny_bayer.data(), tiny_rgb.data(),
                                       width, height, DC1394_COLOR_FILTER_RGGB,
                                       DC1394_BAYER_METHOD_NEAREST);
  EXPECT_EQ(rv, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestBL8bpp) {
  // clang-format off
  constexpr int width=4, height=4;
  constexpr uint8_t R=1, G=2, B=3;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G, R, G,
    G, B, G, B,
    R, G, R, G,
    G, B, G, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
    0, 0, 0,  R, G, B,  R, G, B,  0, 0, 0,
    0, 0, 0,  R, G, B,  R, G, B,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_BILINEAR);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestS8bpp) {
  // clang-format off
  constexpr int width=4, height=4;
  constexpr uint8_t R=1, G=2, B=3;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G, R, G,
    G, B, G, B,
    R, G, R, G,
    G, B, G, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    R, G, B,  R, G, B,  R, G, B,  0, 0, 0,
    R, G, B,  R, G, B,  R, G, B,  0, 0, 0,
    R, G, B,  R, G, B,  R, G, B,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_SIMPLE);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestHQL8bpp) {
  // clang-format off
  constexpr int width=6, height=4;
  constexpr uint8_t R=1, G1=2, G2=4, B=5;
  [[maybe_unused]]constexpr uint8_t G=(G1+G2)/2;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_HQLINEAR);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestDS8bpp) {
  // clang-format off
  constexpr int width=6, height=4;
  constexpr uint8_t R=1, G1=2, G2=4, B=5, G=(G1+G2)/2;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    1, G, B, 1, G, B, 1, G, B, 1, G, B, 1, G, B, 1, G, B,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_DOWNSAMPLE);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestES8bpp) {
  // clang-format off
  constexpr int width=6, height=4;
  constexpr uint8_t R=1, G1=2, G2=4, B=5;
  [[maybe_unused]]constexpr uint8_t G=(G1+G2)/2;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_EDGESENSE);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestVNG8bpp) {
  // clang-format off
  constexpr int width=6, height=4;
  constexpr uint8_t R=1, G1=2, G2=4, B=5, G=(G1+G2)/2;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, R, G, B, 0, 0, 0, 0, 0, 0, R, G2, B, 0, 0, 0,
    0, 0, 0, R, G1, B, R, G, B, R, G1, B, R, G, B, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_VNG);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestAHD8bpp) {
  // clang-format off
  constexpr int width=6, height=4;
  constexpr uint8_t R=1, G1=2, G2=4, B=5, G=(G1+G2)/2;
  std::array<uint8_t, width*height> tiny_bayer{{
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
    R, G1, R, G1, R, G1,
    G2, B, G2, B, G2, B,
  }};
  std::array<uint8_t, width*height*3> tiny_rgb_expected{{
    R, G, B, R, G1, B, R, G1, B, R, G1, B, R, G1, B, R, G1, B,
    R, G2, B, R, G, B, R, G2, B, R, G, B, R, G2, B, R, G1, B,
    R, G, B, R, G1, B, R, 3, B, R, G1, B, R, 3, B, R, G1, B,
    R, G2, B, R, G, B, R, G2, B, R, 3, B, R, G2, B, R, 3, B
  }};
  std::array<uint8_t, width*height*3> tiny_rgb{};
  // clang-format on
  auto result = dc1394_bayer_decoding_8bit(
      tiny_bayer.data(), tiny_rgb.data(), width, height,
      DC1394_COLOR_FILTER_RGGB, DC1394_BAYER_METHOD_AHD);
  EXPECT_EQ(result, DC1394_SUCCESS);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestNN16bpp) {
  // clang-format off
  constexpr int width=4, height=4;
  constexpr uint16_t R=1, G=0xffff, B=3;
  std::array<uint16_t, width*height> tiny_bayer{{
    R, G, R, G,
    G, B, G, B,
    R, G, R, G,
    G, B, G, B,
  }};
  std::array<uint16_t, width*height*3> tiny_rgb_expected{{
    R, G, B,  R, G, B,  R, G, B,  0, 0, 0,
    R, G, B,  R, G, B,  R, G, B,  0, 0, 0,
    R, G, B,  R, G, B,  R, G, B,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
  }};
  std::array<uint16_t, width*height*3> tiny_rgb{};
  // clang-format on
  dc1394_bayer_decoding_16bit(tiny_bayer.data(), tiny_rgb.data(), width, height,
                              DC1394_COLOR_FILTER_RGGB,
                              DC1394_BAYER_METHOD_NEAREST, 16);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

TEST(Bayer2RgbTest, TestBL16bpp) {
  // clang-format off
  constexpr int width=4, height=4;
  constexpr uint16_t R=1, G=0xffff, B=3;
  std::array<uint16_t, width*height> tiny_bayer{{
    R, G, R, G,
    G, B, G, B,
    R, G, R, G,
    G, B, G, B,
  }};
  std::array<uint16_t, width*height*3> tiny_rgb_expected{{
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
    0, 0, 0,  R, G, B,  R, G, B,  0, 0, 0,
    0, 0, 0,  R, G, B,  R, G, B,  0, 0, 0,
    0, 0, 0,  0, 0, 0,  0, 0, 0,  0, 0, 0,
  }};
  std::array<uint16_t, width*height*3> tiny_rgb{};
  // clang-format on
  dc1394_bayer_decoding_16bit(tiny_bayer.data(), tiny_rgb.data(), width, height,
                              DC1394_COLOR_FILTER_RGGB,
                              DC1394_BAYER_METHOD_BILINEAR, 16);

  fmt::print("E: {}\n", tiny_rgb_expected);
  fmt::print("A: {}\n", tiny_rgb);

  EXPECT_TRUE(std::equal(tiny_rgb.begin(), tiny_rgb.end(),
                         tiny_rgb_expected.begin(), tiny_rgb_expected.end()));
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
