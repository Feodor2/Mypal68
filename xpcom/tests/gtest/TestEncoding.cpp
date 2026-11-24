/**
 * @file TestEncoding.cpp
 * @brief Test string encoding and comparison behavior.
 * Responsibilities: verify conversion and comparison semantics.
 * Invariants: expected Unicode ordering and error behavior are preserved.
 * Thread-safety: test data belongs to each test invocation.
 * Performance notes: correctness checks do not benchmark the implementation.
 */
/**
 * @file TestEncoding.cpp
 * @brief Tests for UTF conversion and comparison behavior.
 *
 * Responsibilities:
 *  - Verify encoding conversions and Unicode comparison edge cases.
 * Invariants:
 *  - Invalid sequences preserve replacement and accumulated error semantics.
 * Thread-safety:
 *  - Tests use independent local strings.
 * Performance notes:
 *  - These correctness tests are outside performance-sensitive paths.
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <stdlib.h>
#include "nsString.h"
#include "gtest/gtest.h"

TEST(Encoding, CompareAsciiPrefixAndErrors)
{
  bool error = true;
  EXPECT_EQ(0, CompareUTF8toUTF16(nsDependentCString("prefix"),
                                   nsDependentString(u"prefix"), &error));
  EXPECT_TRUE(error);

  error = false;
  EXPECT_LT(CompareUTF8toUTF16(nsDependentCString("prefix-a"),
                               nsDependentString(u"prefix-b"), &error), 0);
  EXPECT_FALSE(error);

  const char invalidUtf8[] = {'a', static_cast<char>(0xff), 0};
  const char16_t replacement[] = {u'a', 0xfffd, 0};
  error = false;
  EXPECT_EQ(0, CompareUTF8toUTF16(nsDependentCString(invalidUtf8),
                                   nsDependentString(replacement), &error));
  EXPECT_TRUE(error);

  const char16_t invalidUtf16[] = {u'a', 0xd800, 0};
  error = false;
  EXPECT_EQ(0, CompareUTF8toUTF16(nsDependentCString("a\xef\xbf\xbd"),
                                   nsDependentString(invalidUtf16), &error));
  EXPECT_TRUE(error);
}

TEST(Encoding, CompareEveryAsciiCodeUnit)
{
  for (unsigned int byte = 0; byte < 128; ++byte) {
    for (unsigned int word = 0; word < 128; ++word) {
      const char utf8 = static_cast<char>(byte);
      const char16_t utf16 = static_cast<char16_t>(word);
      bool error = false;
      const int32_t actual = CompareUTF8toUTF16(
          nsDependentCString(&utf8, 1), nsDependentString(&utf16, 1), &error);
      const int32_t expected = byte < word ? -1 : byte > word ? 1 : 0;
      ASSERT_EQ(expected, actual);
      ASSERT_FALSE(error);
    }
  }
}

TEST(Encoding, GoodSurrogatePair)
{
  // When this string is decoded, the surrogate pair is U+10302 and the rest of
  // the string is specified by indexes 2 onward.
  const char16_t goodPairData[] = {0xD800, 0xDF02, 0x65, 0x78, 0x0};
  nsDependentString goodPair16(goodPairData);

  uint32_t byteCount = 0;
  char* goodPair8 = ToNewUTF8String(goodPair16, &byteCount);
  EXPECT_TRUE(!!goodPair8);

  EXPECT_EQ(byteCount, 6u);

  const unsigned char expected8[] = {0xF0, 0x90, 0x8C, 0x82, 0x65, 0x78, 0x0};
  EXPECT_EQ(0, memcmp(expected8, goodPair8, sizeof(expected8)));

  // This takes a different code path from the above, so test it to make sure
  // the UTF-16 enumeration remains in sync with the UTF-8 enumeration.
  nsDependentCString expected((const char*)expected8);
  EXPECT_EQ(0, CompareUTF8toUTF16(expected, goodPair16));

  free(goodPair8);
}

TEST(Encoding, BackwardsSurrogatePair)
{
  // When this string is decoded, the two surrogates are wrongly ordered and
  // must each be interpreted as U+FFFD.
  const char16_t backwardsPairData[] = {0xDDDD, 0xD863, 0x65, 0x78, 0x0};
  nsDependentString backwardsPair16(backwardsPairData);

  uint32_t byteCount = 0;
  char* backwardsPair8 = ToNewUTF8String(backwardsPair16, &byteCount);
  EXPECT_TRUE(!!backwardsPair8);

  EXPECT_EQ(byteCount, 8u);

  const unsigned char expected8[] = {0xEF, 0xBF, 0xBD, 0xEF, 0xBF,
                                     0xBD, 0x65, 0x78, 0x0};
  EXPECT_EQ(0, memcmp(expected8, backwardsPair8, sizeof(expected8)));

  // This takes a different code path from the above, so test it to make sure
  // the UTF-16 enumeration remains in sync with the UTF-8 enumeration.
  nsDependentCString expected((const char*)expected8);
  EXPECT_EQ(0, CompareUTF8toUTF16(expected, backwardsPair16));

  free(backwardsPair8);
}

TEST(Encoding, MalformedUTF16OrphanHighSurrogate)
{
  // When this string is decoded, the high surrogate should be replaced and the
  // rest of the string is specified by indexes 1 onward.
  const char16_t highSurrogateData[] = {0xD863, 0x74, 0x65, 0x78, 0x74, 0x0};
  nsDependentString highSurrogate16(highSurrogateData);

  uint32_t byteCount = 0;
  char* highSurrogate8 = ToNewUTF8String(highSurrogate16, &byteCount);
  EXPECT_TRUE(!!highSurrogate8);

  EXPECT_EQ(byteCount, 7u);

  const unsigned char expected8[] = {0xEF, 0xBF, 0xBD, 0x74,
                                     0x65, 0x78, 0x74, 0x0};
  EXPECT_EQ(0, memcmp(expected8, highSurrogate8, sizeof(expected8)));

  // This takes a different code path from the above, so test it to make sure
  // the UTF-16 enumeration remains in sync with the UTF-8 enumeration.
  nsDependentCString expected((const char*)expected8);
  EXPECT_EQ(0, CompareUTF8toUTF16(expected, highSurrogate16));

  free(highSurrogate8);
}

TEST(Encoding, MalformedUTF16OrphanLowSurrogate)
{
  // When this string is decoded, the low surrogate should be replaced and the
  // rest of the string is specified by indexes 1 onward.
  const char16_t lowSurrogateData[] = {0xDDDD, 0x74, 0x65, 0x78, 0x74, 0x0};
  nsDependentString lowSurrogate16(lowSurrogateData);

  uint32_t byteCount = 0;
  char* lowSurrogate8 = ToNewUTF8String(lowSurrogate16, &byteCount);
  EXPECT_TRUE(!!lowSurrogate8);

  EXPECT_EQ(byteCount, 7u);

  const unsigned char expected8[] = {0xEF, 0xBF, 0xBD, 0x74,
                                     0x65, 0x78, 0x74, 0x0};
  EXPECT_EQ(0, memcmp(expected8, lowSurrogate8, sizeof(expected8)));

  // This takes a different code path from the above, so test it to make sure
  // the UTF-16 enumeration remains in sync with the UTF-8 enumeration.
  nsDependentCString expected((const char*)expected8);
  EXPECT_EQ(0, CompareUTF8toUTF16(expected, lowSurrogate16));

  free(lowSurrogate8);
}
