/*
 * #%L
 * OME-BIOFORMATS C++ library for image IO.
 * %%
 * Copyright © 2006 - 2014 Open Microscopy Environment:
 *   - Massachusetts Institute of Technology
 *   - National Institutes of Health
 *   - University of Dundee
 *   - Board of Regents of the University of Wisconsin-Madison
 *   - Glencoe Software, Inc.
 * %%
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of any organization.
 * #L%
 */

#include <sstream>
#include <stdexcept>
#include <iostream>

#include <ome/bioformats/PixelBuffer.h>

#include <gtest/gtest.h>
#include <gtest/gtest-death-test.h>

#include "pixel.h"

using ome::bioformats::Dimensions;
using ome::bioformats::PixelEndianProperties;
using ome::bioformats::PixelBufferBase;
using ome::bioformats::PixelBuffer;
typedef ome::xml::model::enums::DimensionOrder DO;
typedef ome::xml::model::enums::PixelType PT;

/*
 * NOTE: Update equivalent tests in variantpixelbuffer.cpp when making
 * changes.
 */

class DimensionOrderTestParameters
{
public:
  DO                                  order;
  bool                                interleaved;
  bool                                is_default;
  PixelBufferBase::storage_order_type expected_order;

  DimensionOrderTestParameters(DO                                  order,
                               bool                                interleaved,
                               bool                                is_default,
                               PixelBufferBase::storage_order_type expected_order):
    order(order),
    interleaved(interleaved),
    is_default(is_default),
    expected_order(expected_order)
  {}
};

template<class charT, class traits>
inline std::basic_ostream<charT,traits>&
operator<< (std::basic_ostream<charT,traits>& os,
            const DimensionOrderTestParameters& params)
{
  return os << DO(params.order) << (params.interleaved ? "/chunky" : "/planar");
}

class DimensionOrderTest : public ::testing::TestWithParam<DimensionOrderTestParameters>
{
};

TEST_P(DimensionOrderTest, OrderCorrect)
{
  const DimensionOrderTestParameters& params = GetParam();

  ASSERT_EQ(params.expected_order, PixelBufferBase::make_storage_order(params.order, params.interleaved));
}

TEST_P(DimensionOrderTest, Default)
{
  const DimensionOrderTestParameters& params = GetParam();

  if (params.is_default)
    ASSERT_EQ(PixelBufferBase::default_storage_order(), PixelBufferBase::make_storage_order(params.order, params.interleaved));
  else
    ASSERT_FALSE(PixelBufferBase::default_storage_order() == PixelBufferBase::make_storage_order(params.order, params.interleaved));
}

namespace
{
  PixelBufferBase::storage_order_type
  make_order(ome::bioformats::Dimensions d0,
             ome::bioformats::Dimensions d1,
             ome::bioformats::Dimensions d2,
             ome::bioformats::Dimensions d3,
             ome::bioformats::Dimensions d4,
             ome::bioformats::Dimensions d5,
             ome::bioformats::Dimensions d6,
             ome::bioformats::Dimensions d7,
             ome::bioformats::Dimensions d8)
  {
    PixelBufferBase::size_type ordering[PixelBufferBase::dimensions] = {d0, d1, d2, d3, d4, d5, d6, d7, d8};
    bool ascending[PixelBufferBase::dimensions] = {true, true, true, true, true, true, true, true, true};
    return PixelBufferBase::storage_order_type(ordering, ascending);
  }
}

DimensionOrderTestParameters dimension_params[] =
  { //                           DimensionOrder interleaved default
    //                           expected-order
    DimensionOrderTestParameters(DO::XYZTC, true, true,
                                 make_order(ome::bioformats::DIM_SUBCHANNEL, ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL)),
    DimensionOrderTestParameters(DO::XYZTC, false, false,
                                 make_order(ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y, ome::bioformats::DIM_SUBCHANNEL,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL)),

    DimensionOrderTestParameters(DO::XYZCT, true, false,
                                 make_order(ome::bioformats::DIM_SUBCHANNEL, ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T)),
    DimensionOrderTestParameters(DO::XYZCT, false, false,
                                 make_order(ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y, ome::bioformats::DIM_SUBCHANNEL,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T)),

    DimensionOrderTestParameters(DO::XYTZC, true, false,
                                 make_order(ome::bioformats::DIM_SUBCHANNEL, ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL)),
    DimensionOrderTestParameters(DO::XYTZC, false, false,
                                 make_order(ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y, ome::bioformats::DIM_SUBCHANNEL,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL)),

    DimensionOrderTestParameters(DO::XYTCZ, true, false,
                                 make_order(ome::bioformats::DIM_SUBCHANNEL, ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z)),
    DimensionOrderTestParameters(DO::XYTCZ, false, false,
                                 make_order(ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y, ome::bioformats::DIM_SUBCHANNEL,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z)),

    DimensionOrderTestParameters(DO::XYCZT, true, false,
                                 make_order(ome::bioformats::DIM_SUBCHANNEL, ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T)),
    DimensionOrderTestParameters(DO::XYCZT, false, false,
                                 make_order(ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y, ome::bioformats::DIM_SUBCHANNEL,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T)),

    DimensionOrderTestParameters(DO::XYCTZ, true, false,
                                 make_order(ome::bioformats::DIM_SUBCHANNEL, ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z)),
    DimensionOrderTestParameters(DO::XYCTZ, false, false,
                                 make_order(ome::bioformats::DIM_SPATIAL_X, ome::bioformats::DIM_SPATIAL_Y, ome::bioformats::DIM_SUBCHANNEL,
                                            ome::bioformats::DIM_MODULO_C, ome::bioformats::DIM_CHANNEL,
                                            ome::bioformats::DIM_MODULO_T, ome::bioformats::DIM_TEMPORAL_T,
                                            ome::bioformats::DIM_MODULO_Z, ome::bioformats::DIM_SPATIAL_Z))
  };


template <typename T>
class PixelBufferType : public ::testing::Test
{};

TYPED_TEST_CASE_P(PixelBufferType);

TYPED_TEST_P(PixelBufferType, DefaultConstruct)
{
  ASSERT_NO_THROW(PixelBuffer<TypeParam> buf);
}

TYPED_TEST_P(PixelBufferType, ConstructSize)
{
  ASSERT_NO_THROW(PixelBuffer<TypeParam> buf(boost::extents[5][2][1][1][1][1][1][1][1]));
}

TYPED_TEST_P(PixelBufferType, ConstructRange)
{
  std::vector<TypeParam> source;
  for (int i = 0; i < 10; ++i)
    source.push_back(pixel_value<TypeParam>(i));

  PixelBuffer<TypeParam> buf(boost::extents[5][2][1][1][1][1][1][1][1]);
  buf.assign(source.begin(), source.end());

  ASSERT_EQ(buf.num_elements(), 10U);
  ASSERT_TRUE(buf.data());
  for (int i = 0; i < 10; ++i)
    {
      ASSERT_EQ(pixel_value<TypeParam>(i), *(buf.data()+i));
    }
}

TYPED_TEST_P(PixelBufferType, ConstructCopy)
{
  std::vector<TypeParam> source1;
  for (int i = 0; i < 10; ++i)
    source1.push_back(pixel_value<TypeParam>(i));

  std::vector<TypeParam> source2;
  for (int i = 10U; i < 20U; ++i)
    source2.push_back(pixel_value<TypeParam>(i));

  PixelBuffer<TypeParam> buf1(boost::extents[5][2][1][1][1][1][1][1][1]);
  buf1.assign(source1.begin(), source1.end());
  PixelBuffer<TypeParam> buf2(boost::extents[5][2][1][1][1][1][1][1][1]);
  buf2.assign(source2.begin(), source2.end());

  ASSERT_EQ(buf1, buf1);
  ASSERT_EQ(buf2, buf2);
  ASSERT_NE(buf1, buf2);

  PixelBuffer<TypeParam> buf3(buf2);
  ASSERT_EQ(buf2, buf3);
  ASSERT_NE(buf1, buf2);
}

template<typename T>
void test_operators(const PixelBuffer<T>& buf1,
                    const PixelBuffer<T>& buf2)
{
  EXPECT_GT(buf2, buf1);
  EXPECT_GE(buf2, buf1);
  EXPECT_GE(buf1, buf1);
  EXPECT_LT(buf1, buf2);
  EXPECT_LE(buf1, buf2);
  EXPECT_LE(buf2, buf2);
}

template<typename T>
void test_operators(const PixelBuffer<std::complex<T> >& /* buf1 */,
                    const PixelBuffer<std::complex<T> >& /* buf2 */)
{
}

TYPED_TEST_P(PixelBufferType, Operators)
{
  std::vector<TypeParam> source1;
  for (int i = 0; i < 10; ++i)
    source1.push_back(pixel_value<TypeParam>(i));

  std::vector<TypeParam> source2;
  for (int i = 100U; i < 120U; ++i)
    source2.push_back(pixel_value<TypeParam>(i));

  PixelBuffer<TypeParam> buf1(boost::extents[5][2][1][1][1][1][1][1][1]);
  buf1.assign(source1.begin(), source1.end());
  PixelBuffer<TypeParam> buf2(boost::extents[5][2][1][1][1][1][1][1][1]);
  buf2.assign(source2.begin(), source2.end());

  EXPECT_EQ(buf1, buf1);
  EXPECT_EQ(buf2, buf2);
  EXPECT_NE(buf1, buf2);
  test_operators(buf1, buf2);
}

TYPED_TEST_P(PixelBufferType, Array)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_NO_THROW(buf.array());
  ASSERT_NO_THROW(cbuf.array());
  ASSERT_EQ(100U, buf.array().num_elements());
  ASSERT_EQ(100U, cbuf.array().num_elements());
}

TYPED_TEST_P(PixelBufferType, Data)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_TRUE(buf.data());
  ASSERT_TRUE(cbuf.data());
  ASSERT_EQ(buf.array().data(), buf.data());
  ASSERT_EQ(cbuf.array().data(), cbuf.data());
}

TYPED_TEST_P(PixelBufferType, Valid)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_TRUE(buf.valid());
  ASSERT_TRUE(cbuf.valid());
}

TYPED_TEST_P(PixelBufferType, Managed)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  EXPECT_TRUE(buf.managed());
  EXPECT_TRUE(cbuf.managed());

  TypeParam *backing = 0;
  PixelBuffer<TypeParam> mbuf(backing, boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cmbuf(mbuf);

  EXPECT_FALSE(mbuf.managed());
  EXPECT_FALSE(cmbuf.managed());
}

TYPED_TEST_P(PixelBufferType, NumElements)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][10][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_EQ(1000U, buf.num_elements());
  ASSERT_EQ(1000U, cbuf.num_elements());
}

TYPED_TEST_P(PixelBufferType, NumDimensions)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][10][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_EQ(9U, buf.num_dimensions());
  ASSERT_EQ(9U, cbuf.num_dimensions());
}

TYPED_TEST_P(PixelBufferType, Shape)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][3][1][1][10][1][4][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  const typename PixelBuffer<TypeParam>::size_type *shape = cbuf.shape();
  EXPECT_EQ(10U, *(shape+0));
  EXPECT_EQ( 3U, *(shape+1));
  EXPECT_EQ( 1U, *(shape+2));
  EXPECT_EQ( 1U, *(shape+3));
  EXPECT_EQ(10U, *(shape+4));
  EXPECT_EQ( 1U, *(shape+5));
  EXPECT_EQ( 4U, *(shape+6));
  EXPECT_EQ( 1U, *(shape+7));
  EXPECT_EQ( 1U, *(shape+8));
}

TYPED_TEST_P(PixelBufferType, Strides)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][3][1][1][10][1][4][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  const boost::multi_array_types::index *strides = cbuf.strides();
  EXPECT_EQ(  1U, *(strides+0));
  EXPECT_EQ( 10U, *(strides+1));
  EXPECT_EQ(120U, *(strides+2));
  EXPECT_EQ(120U, *(strides+3));
  EXPECT_EQ(120U, *(strides+4));
  EXPECT_EQ(  1U, *(strides+5));
  EXPECT_EQ( 30U, *(strides+6));
  EXPECT_EQ(120U, *(strides+7));
  EXPECT_EQ(120U, *(strides+8));
}

TYPED_TEST_P(PixelBufferType, IndexBases)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][3][1][1][10][1][4][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  const boost::multi_array_types::index *bases = cbuf.index_bases();
  EXPECT_EQ(0U, *(bases+0));
  EXPECT_EQ(0U, *(bases+1));
  EXPECT_EQ(0U, *(bases+2));
  EXPECT_EQ(0U, *(bases+3));
  EXPECT_EQ(0U, *(bases+4));
  EXPECT_EQ(0U, *(bases+5));
  EXPECT_EQ(0U, *(bases+6));
  EXPECT_EQ(0U, *(bases+7));
  EXPECT_EQ(0U, *(bases+8));
}

TYPED_TEST_P(PixelBufferType, Origin)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][3][1][1][10][1][4][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  const TypeParam *origin = cbuf.origin();
  EXPECT_EQ(cbuf.data(), origin);
}

TYPED_TEST_P(PixelBufferType, StorageOrder)
{
  {
    PixelBuffer<TypeParam> buf(boost::extents[10][3][1][1][10][1][4][1][1]);
    const PixelBuffer<TypeParam>& cbuf(buf);

    const typename PixelBuffer<TypeParam>::storage_order_type& order = cbuf.storage_order();

    EXPECT_EQ(5, order.ordering(0));
    EXPECT_EQ(0, order.ordering(1));
    EXPECT_EQ(1, order.ordering(2));
    EXPECT_EQ(6, order.ordering(3));
    EXPECT_EQ(2, order.ordering(4));
    EXPECT_EQ(7, order.ordering(5));
    EXPECT_EQ(3, order.ordering(6));
    EXPECT_EQ(8, order.ordering(7));
    EXPECT_EQ(4, order.ordering(8));

    EXPECT_TRUE(order.ascending(0));
    EXPECT_TRUE(order.ascending(1));
    EXPECT_TRUE(order.ascending(2));
    EXPECT_TRUE(order.ascending(3));
    EXPECT_TRUE(order.ascending(4));
    EXPECT_TRUE(order.ascending(5));
    EXPECT_TRUE(order.ascending(6));
    EXPECT_TRUE(order.ascending(7));
    EXPECT_TRUE(order.ascending(8));
  }
}

TYPED_TEST_P(PixelBufferType, GetIndex)
{
  std::vector<TypeParam> source;
  for (int i = 0; i < 100; ++i)
    source.push_back(pixel_value<TypeParam>(i));

  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  buf.assign(source.begin(), source.end());
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_EQ(buf.num_elements(), 100U);
  ASSERT_TRUE(buf.data());
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
      {
        typename PixelBuffer<TypeParam>::indices_type idx;
        idx[0] = i;
        idx[1] = j;
        idx[2] = idx[3] = idx[4] = idx[5] = idx[6] = idx[7] = idx[8] = 0;
        EXPECT_EQ(pixel_value<TypeParam>((j * 10) + i), buf.at(idx));
        EXPECT_EQ(pixel_value<TypeParam>((j * 10) + i), cbuf.at(idx));
      }
}

TYPED_TEST_P(PixelBufferType, SetIndex)
{
  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  ASSERT_EQ(buf.num_elements(), 100U);
  ASSERT_TRUE(buf.data());
  for (int i = 0; i < 10; ++i)
    for (int j = 0; j < 10; ++j)
      {
        typename PixelBuffer<TypeParam>::indices_type idx;
        idx[0] = i;
        idx[1] = j;
        idx[2] = idx[3] = idx[4] = idx[5] = idx[6] = idx[7] = idx[8] = 0;

        TypeParam val = pixel_value<TypeParam>(i + j + j);

        buf.at(idx) = val;

        ASSERT_EQ(val, buf.at(idx));
        ASSERT_EQ(val, cbuf.at(idx));
    }
}

TYPED_TEST_P(PixelBufferType, SetIndexDeathTest)
{
  ::testing::FLAGS_gtest_death_test_style = "threadsafe";

  PixelBuffer<TypeParam> buf(boost::extents[10][10][1][1][1][1][1][1][1]);
  const PixelBuffer<TypeParam>& cbuf(buf);

  typename PixelBuffer<TypeParam>::indices_type badidx;
  badidx[0] = 13;
  badidx[1] = 2;
  badidx[2] = badidx[3] = badidx[4] = badidx[5] = badidx[6] = badidx[7] = badidx[8] = 0;

  ASSERT_DEATH(buf.at(badidx) = 4U, "Assertion.*failed");
  ASSERT_DEATH(cbuf.at(badidx), "Assertion.*failed");
}

TYPED_TEST_P(PixelBufferType, StreamInput)
{
  PixelBuffer<TypeParam> buf(boost::extents[2][2][3][4][1][1][1][1][1]);
  typename PixelBuffer<TypeParam>::size_type size = buf.num_elements();
  std::stringstream ss;

  for (typename PixelBuffer<TypeParam>::size_type i = 0; i < size; ++i)
    {
      TypeParam val = pixel_value<TypeParam>(i);
      ss.write(reinterpret_cast<const char *>(&val), sizeof(TypeParam));
    }

  ss.seekg(0, std::ios::beg);
  ss >> buf;
  EXPECT_FALSE(!ss);

  typename PixelBuffer<TypeParam>::indices_type idx;
  idx[0] = idx[1] = idx[2] = idx[3] = idx[4] = idx[5] = idx[6] = idx[7] = idx[8] = 0;
  std::vector<int>::size_type i = 0;
  for (idx[3] = 0; idx[3] < 4; ++idx[3])
    for (idx[2] = 0; idx[2] < 3; ++idx[2])
      for (idx[1] = 0; idx[1] < 2; ++idx[1])
        for (idx[0] = 0; idx[0] < 2; ++idx[0])
          EXPECT_EQ(pixel_value<TypeParam>(i++), buf.at(idx));
}

TYPED_TEST_P(PixelBufferType, StreamOutput)
{
  PixelBuffer<TypeParam> buf(boost::extents[2][2][3][4][1][1][1][1][1]);
  typename PixelBuffer<TypeParam>::size_type size = buf.num_elements();
  std::stringstream ss;

  std::vector<TypeParam> v;
  for (typename PixelBuffer<TypeParam>::size_type i = 0; i < size; ++i)
    {
      TypeParam val = pixel_value<TypeParam>(i);
      v.push_back(val);
    }

  buf.assign(v.begin(), v.end());
  ss << buf;
  EXPECT_FALSE(!ss);
  ss.seekg(0, std::ios::beg);

  typename PixelBuffer<TypeParam>::indices_type idx;
  idx[0] = idx[1] = idx[2] = idx[3] = idx[4] = idx[5] = idx[6] = idx[7] = idx[8] = 0;
  typename std::vector<TypeParam>::size_type i = 0;
  for (idx[3] = 0; idx[3] < 4; ++idx[3])
    for (idx[2] = 0; idx[2] < 3; ++idx[2])
      for (idx[1] = 0; idx[1] < 2; ++idx[1])
        for (idx[0] = 0; idx[0] < 2; ++idx[0])
          {
            EXPECT_EQ(pixel_value<TypeParam>(i), buf.at(idx));
            TypeParam sval;
            ss.read(reinterpret_cast<char *>(&sval), sizeof(TypeParam));
            EXPECT_FALSE(!ss);
            EXPECT_EQ(pixel_value<TypeParam>(i), sval);
            ++i;
          }
}

REGISTER_TYPED_TEST_CASE_P(PixelBufferType,
                           DefaultConstruct,
                           ConstructSize,
                           ConstructRange,
                           ConstructCopy,
                           Operators,
                           Array,
                           Data,
                           Valid,
                           Managed,
                           NumElements,
                           NumDimensions,
                           Shape,
                           Strides,
                           IndexBases,
                           Origin,
                           StorageOrder,
                           GetIndex,
                           SetIndex,
                           SetIndexDeathTest,
                           StreamInput,
                           StreamOutput);

typedef ::testing::Types<
  PixelEndianProperties<PT::INT8,          ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::INT8,          ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::INT8,          ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::INT16,         ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::INT16,         ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::INT16,         ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::INT32,         ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::INT32,         ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::INT32,         ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::UINT8,         ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::UINT8,         ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::UINT8,         ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::UINT16,        ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::UINT16,        ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::UINT16,        ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::UINT32,        ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::UINT32,        ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::UINT32,        ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::FLOAT,         ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::FLOAT,         ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::FLOAT,         ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::DOUBLE,        ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::DOUBLE,        ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::DOUBLE,        ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::BIT,           ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::BIT,           ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::BIT,           ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::COMPLEX,       ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::COMPLEX,       ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::COMPLEX,       ome::bioformats::NATIVE>::type,
  PixelEndianProperties<PT::DOUBLECOMPLEX, ome::bioformats::BIG   >::type,
  PixelEndianProperties<PT::DOUBLECOMPLEX, ome::bioformats::LITTLE>::type,
  PixelEndianProperties<PT::DOUBLECOMPLEX, ome::bioformats::NATIVE>::type> TestTypes;

INSTANTIATE_TYPED_TEST_CASE_P(PixelBufferTypeTest, PixelBufferType, TestTypes);

INSTANTIATE_TEST_CASE_P(DimensionOrderVariants, DimensionOrderTest, ::testing::ValuesIn(dimension_params));
