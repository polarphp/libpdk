// @copyright 2017-2018 zzu_softboy <zzu_softboy@163.com>
//
// THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
// IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
// INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
// NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
// THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Created by softboy on 2017/11/30.

#include <limits>
#include <limits.h>
#include <cstdio>
#include <list>
#include <utility>
#include <tuple>
#include <type_traits>

#include "gtest/gtest.h"
#include "pdk/base/os/thread/Atomic.h"

using pdk::os::thread::AtomicInteger;

#if !defined(PDK_ATOMIC_INT32_IS_SUPPORTED)
#  error "pdk::os::thread::AtomicInteger for 32-bit types must be supported!"
#endif

#if PDK_POINTER_SIZE == 8 && !defined(PDK_ATOMIC_INT64_IS_SUPPORTED)
#  error "pdk::os::thread::AtomicInteger for 64-bit types must be supported on 64-bit builds!"
#endif

//PDK_WARNING_DISABLE_GCC("-Wtype-limits")
//PDK_WARNING_DISABLE_GCC("-Wsign-compare")
//PDK_WARNING_DISABLE_GCC("-Wtautological-constant-out-of-range-compare")

template <class T>
class AtomicIntegerTest : public testing::Test
{};

using AlwaysSupportTypes = testing::Types
<int, unsigned int, long, unsigned long, pdk::ptrdiff, pdk::uintptr
#if (defined(__SIZEOF_WCHAR_T__) && (__SIZEOF_WCHAR_T__-0) > 2) \
   || (defined(WCHAR_MAX) && (WCHAR_MAX-0 > 0x10000))
#define PDK_TYPE_SUPPORTED_wchar_t
, wchar_t
#endif
#ifdef PDK_COMPILER_UNICODE_STRINGS
, char32_t
#endif
#ifdef PDK_ATOMIC_INT8_IS_SUPPORTED
, char, unsigned char, signed char
#endif
#ifdef PDK_ATOMIC_INT16_IS_SUPPORTED
, short, unsigned short, signed short
#  ifdef PDK_COMPILER_UNICODE_STRINGS
, char16_t
#  endif
#  ifndef PDK_TYPE_SUPPORTED_wchar_t
, wchar_t
#  endif
#endif
#ifdef PDK_ATOMIC_INT64_IS_SUPPORTED
, pdk::plonglong, pdk::pulonglong
#endif
>;

namespace 
{

template <bool>
inline void boolean_helper()
{}

template <typename TestType, typename DataType>
void init_test_data(std::list<DataType> &data)
{
   using Limits = std::numeric_limits<TestType>;
   data.push_back(static_cast<DataType>(0));
   data.push_back(static_cast<DataType>(1));
   data.push_back(static_cast<DataType>(42));
   bool isSigned = std::is_signed<TestType>::value;
   if (isSigned) {
      data.push_back(static_cast<pdk::pint64>(-1));
      data.push_back(static_cast<pdk::pint64>(-47));
   }
   if (isSigned && Limits::min() < static_cast<pdk::pint64>(SCHAR_MIN)) {
      data.push_back(static_cast<pdk::pint64>(SCHAR_MIN));
   }
   if (Limits::max() > static_cast<DataType>(SCHAR_MAX)) {
      data.push_back(static_cast<DataType>(SCHAR_MAX));
   }
   if (Limits::max() > static_cast<DataType>(UCHAR_MAX)) {
      data.push_back(static_cast<DataType>(UCHAR_MAX));
   }
   if (isSigned && Limits::min() < -static_cast<pdk::pint64>(UCHAR_MAX)) {
      data.push_back(-static_cast<pdk::pint64>(UCHAR_MAX));
   }
   if (Limits::max() > static_cast<DataType>(SHRT_MAX)) {
      data.push_back(static_cast<DataType>(SHRT_MAX));
   }
   if (isSigned && Limits::min() < static_cast<pdk::pint64>(SHRT_MIN)) {
      data.push_back(static_cast<pdk::pint64>(SHRT_MIN));
   }
   if (Limits::max() > static_cast<DataType>(USHRT_MAX)) {
      data.push_back(static_cast<DataType>(USHRT_MAX));
   }
   if (isSigned && Limits::min() < -static_cast<pdk::pint64>(USHRT_MAX)) {
      data.push_back(-static_cast<pdk::pint64>(USHRT_MAX));
   }
   if (Limits::max() > static_cast<DataType>(INT_MAX)) {
      data.push_back(static_cast<DataType>(INT_MAX));
   }
   if (isSigned && Limits::min() < static_cast<pdk::pint64>(INT_MIN)) {
      data.push_back(static_cast<pdk::pint64>(INT_MIN));
   }
   if (Limits::max() > static_cast<DataType>(UINT_MAX)) {
      data.push_back(static_cast<DataType>(UINT_MAX));
   }
   if (Limits::max() > static_cast<DataType>(std::numeric_limits<pdk::pint64>::max())) {
      data.push_back(static_cast<DataType>(std::numeric_limits<pdk::pint64>::max()));
   }
   if (isSigned && Limits::min() < -static_cast<pdk::pint64>(UINT_MAX)) {
      data.push_back(-static_cast<pdk::pint64>(UINT_MAX));
   }
   if (isSigned) {
      data.push_back(static_cast<pdk::pint64>(Limits::min()));
   }
   data.push_back(static_cast<pdk::pint64>(Limits::max()));
}

}

TYPED_TEST_CASE(AtomicIntegerTest, AlwaysSupportTypes);

TYPED_TEST(AtomicIntegerTest, testStaticChecks)
{
   PDK_STATIC_ASSERT(sizeof(AtomicInteger<TypeParam>) == sizeof(TypeParam));
   PDK_STATIC_ASSERT(alignof(AtomicInteger<TypeParam>) == alignof(TypeParam));
   // statements with no effect
   (void) AtomicInteger<TypeParam>::isRefCountingNative();
   (void) AtomicInteger<TypeParam>::isRefCountingWaitFree();
   (void) AtomicInteger<TypeParam>::isTestAndSetNative();
   (void) AtomicInteger<TypeParam>::isTestAndSetWaitFree();
   (void) AtomicInteger<TypeParam>::isFetchAndStoreNative();
   (void) AtomicInteger<TypeParam>::isFetchAndStoreWaitFree();
   (void) AtomicInteger<TypeParam>::isFetchAndAddNative();
   (void) AtomicInteger<TypeParam>::isFetchAndAddWaitFree();
#ifdef PDK_COMPILER_CONSTEXPR
   boolean_helper<AtomicInteger<TypeParam>::isRefCountingNative()>();
   boolean_helper<AtomicInteger<TypeParam>::isRefCountingWaitFree()>();
   boolean_helper<AtomicInteger<TypeParam>::isTestAndSetNative()>();
   boolean_helper<AtomicInteger<TypeParam>::isTestAndSetWaitFree()>();
   boolean_helper<AtomicInteger<TypeParam>::isFetchAndStoreNative()>();
   boolean_helper<AtomicInteger<TypeParam>::isFetchAndStoreWaitFree()>();
   boolean_helper<AtomicInteger<TypeParam>::isFetchAndAddNative()>();
   boolean_helper<AtomicInteger<TypeParam>::isFetchAndAddWaitFree()>();
#endif
}

TYPED_TEST(AtomicIntegerTest, testConstructor)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic1(value);
      ASSERT_EQ(atomic1.load(), static_cast<TypeParam>(value));
      AtomicInteger<TypeParam> atomic2 = value;
      ASSERT_EQ(atomic2.load(), static_cast<TypeParam>(value));
      ASSERT_TRUE(atomic1.load() >= std::numeric_limits<TypeParam>::min());
      ASSERT_TRUE(atomic1.load() <= std::numeric_limits<TypeParam>::max());
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testCopy)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      AtomicInteger<TypeParam> copy(atomic);
      ASSERT_EQ(atomic.load(), copy.load());
      AtomicInteger<TypeParam> copy2 = atomic;
      ASSERT_EQ(atomic.load(), copy2.load());
      
      AtomicInteger<TypeParam> copy3(std::move(atomic));
      ASSERT_EQ(atomic.load(), copy3.load());
      AtomicInteger<TypeParam> copy4 = std::move(atomic);
      ASSERT_EQ(atomic.load(), copy4.load());
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testAssign)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      AtomicInteger<TypeParam> copy;
      copy = atomic;
      ASSERT_EQ(copy.load(), atomic.load());
      
      AtomicInteger<TypeParam> copy2;
      copy2 = atomic;
      ASSERT_EQ(copy2.load(), atomic.load());
      
      AtomicInteger<TypeParam> copy2bis;
      copy2bis = atomic.load(); // operator=(TypeParam)
      ASSERT_EQ(copy2bis.load(), atomic.load());
      
      AtomicInteger<TypeParam> copy3;
      copy3 = std::move(copy);
      ASSERT_EQ(copy3.load(), atomic.load());
      
      AtomicInteger<TypeParam> copy4;
      copy4 = std::move(copy2);
      ASSERT_EQ(copy4.load(), atomic.load());
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testOperatorInteger)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      TypeParam value2 = atomic;
      ASSERT_EQ(value2, static_cast<TypeParam>(value));
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testLoadAcquireStoreRelease)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      ASSERT_EQ(atomic.loadAcquire(), static_cast<TypeParam>(value));
      atomic.storeRelease(~value);
      ASSERT_EQ(atomic.loadAcquire(), static_cast<TypeParam>(~value));
      atomic.storeRelease(value);
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testRefDeref)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      const bool needToPreventOverflow  = std::is_signed<TypeParam>::value && value == std::numeric_limits<TypeParam>::max();
      const bool needToPreventUnderflow = std::is_signed<TypeParam>::value && value == std::numeric_limits<TypeParam>::min();
      TypeParam nextValue = static_cast<TypeParam>(value);
      if (!needToPreventOverflow) {
         ++nextValue;
      }
      TypeParam prevValue = static_cast<TypeParam>(value);
      if (!needToPreventUnderflow) {
         --prevValue;
      }
      AtomicInteger<TypeParam> atomic(value);
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic.ref(), nextValue != 0);
         ASSERT_EQ(atomic.load(), nextValue);
         ASSERT_EQ(atomic.deref(), value != 0);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.deref(), prevValue != 0);
         ASSERT_EQ(atomic.load(), prevValue);
         ASSERT_EQ(atomic.ref(), value != 0);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      if (!needToPreventOverflow) {
         ASSERT_EQ(++atomic, nextValue);
         ASSERT_EQ(--atomic, static_cast<TypeParam>(value));
      }
      if (!needToPreventUnderflow) {
         ASSERT_EQ(--atomic, prevValue);
         ASSERT_EQ(++atomic, static_cast<TypeParam>(value));
      }
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic++, static_cast<TypeParam>(value));
         ASSERT_EQ(atomic--, nextValue);
      }
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic--, static_cast<TypeParam>(value));
         ASSERT_EQ(atomic++, prevValue);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testTestAndSet)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      TypeParam newValue = ~static_cast<TypeParam>(value);
      AtomicInteger<TypeParam> atomic(value);
      
      ASSERT_TRUE(atomic.testAndSetRelaxed(value, newValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetRelaxed(value, newValue));
      ASSERT_TRUE(atomic.testAndSetRelaxed(newValue, value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_TRUE(atomic.testAndSetAcquire(value, newValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetAcquire(value, newValue));
      ASSERT_TRUE(atomic.testAndSetAcquire(newValue, value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_TRUE(atomic.testAndSetRelease(value, newValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetRelease(value, newValue));
      ASSERT_TRUE(atomic.testAndSetRelease(newValue, value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_TRUE(atomic.testAndSetOrdered(value, newValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetOrdered(value, newValue));
      ASSERT_TRUE(atomic.testAndSetOrdered(newValue, value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testTestAndSetWithOldValue)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      TypeParam oldValue;
      TypeParam newValue = ~static_cast<TypeParam>(value);
      AtomicInteger<TypeParam> atomic(value);
      
      ASSERT_TRUE(atomic.testAndSetRelaxed(value, newValue, oldValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetRelaxed(value, newValue, oldValue));
      ASSERT_EQ(oldValue, newValue);
      ASSERT_TRUE(atomic.testAndSetRelaxed(newValue, value, oldValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(oldValue, newValue);
      
      ASSERT_TRUE(atomic.testAndSetAcquire(value, newValue, oldValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetAcquire(value, newValue, oldValue));
      ASSERT_EQ(oldValue, newValue);
      ASSERT_TRUE(atomic.testAndSetAcquire(newValue, value, oldValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(oldValue, newValue);
      
      ASSERT_TRUE(atomic.testAndSetRelease(value, newValue, oldValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetRelease(value, newValue, oldValue));
      ASSERT_EQ(oldValue, newValue);
      ASSERT_TRUE(atomic.testAndSetRelease(newValue, value, oldValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(oldValue, newValue);
      
      ASSERT_TRUE(atomic.testAndSetOrdered(value, newValue, oldValue));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_TRUE(!atomic.testAndSetOrdered(value, newValue, oldValue));
      ASSERT_EQ(oldValue, newValue);
      ASSERT_TRUE(atomic.testAndSetOrdered(newValue, value, oldValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(oldValue, newValue);
      
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testFetchAndStore)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      TypeParam newValue = ~static_cast<TypeParam>(value);
      AtomicInteger<TypeParam> atomic(value);
      
      ASSERT_EQ(atomic.fetchAndStoreRelaxed(newValue), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_EQ(atomic.fetchAndStoreRelaxed(value), static_cast<TypeParam>(newValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic.fetchAndStoreAcquire(newValue), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_EQ(atomic.fetchAndStoreAcquire(value), static_cast<TypeParam>(newValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic.fetchAndStoreRelease(newValue), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_EQ(atomic.fetchAndStoreRelease(value), static_cast<TypeParam>(newValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic.fetchAndStoreOrdered(newValue), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), newValue);
      ASSERT_EQ(atomic.fetchAndStoreOrdered(value), static_cast<TypeParam>(newValue));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testFetchAndAdd)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      TypeParam parcel1 = 41;
      TypeParam parcel2 = static_cast<TypeParam>(0-41);
      const bool needToPreventOverflow  = std::is_signed<TypeParam>::value && value > std::numeric_limits<TypeParam>::max() + parcel2;
      const bool needToPreventUnderflow = std::is_signed<TypeParam>::value && value < std::numeric_limits<TypeParam>::min() + parcel1;
      
      TypeParam newValue1 = static_cast<TypeParam>(value);
      if (!needToPreventOverflow) {
         newValue1 += parcel1;
      }
      TypeParam newValue2 = static_cast<TypeParam>(value);
      if (!needToPreventUnderflow) {
         newValue2 += parcel2;
      }
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndAddRelaxed(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndAddRelaxed(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndAddRelaxed(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndAddRelaxed(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndAddAcquire(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndAddAcquire(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndAddAcquire(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndAddAcquire(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndAddRelease(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndAddRelease(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndAddRelease(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndAddRelease(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndAddOrdered(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndAddOrdered(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndAddOrdered(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndAddOrdered(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic += parcel1, newValue1);
         ASSERT_EQ(atomic += parcel2, static_cast<TypeParam>(value));
      }
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic += parcel2, newValue2);
         ASSERT_EQ(atomic += parcel1, static_cast<TypeParam>(value));
      }
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testFetchAndSub)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      TypeParam parcel1 = 41;
      TypeParam parcel2 = static_cast<TypeParam>(0-41);
      const bool needToPreventOverflow  = std::is_signed<TypeParam>::value && value > std::numeric_limits<TypeParam>::max() - parcel1;
      const bool needToPreventUnderflow = std::is_signed<TypeParam>::value && value < std::numeric_limits<TypeParam>::min() - parcel2;
      
      TypeParam newValue1 = static_cast<TypeParam>(value);
      if (!needToPreventUnderflow) {
         newValue1 -= parcel1;
      }
      TypeParam newValue2 = static_cast<TypeParam>(value);
      if (!needToPreventOverflow) {
         newValue2 -= parcel2;
      }
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndSubRelaxed(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndSubRelaxed(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if(!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndSubRelaxed(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndSubRelaxed(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndSubAcquire(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndSubAcquire(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if(!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndSubAcquire(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndSubAcquire(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndSubRelease(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndSubRelease(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if(!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndSubRelease(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndSubRelease(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic.fetchAndSubOrdered(parcel1), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue1);
         ASSERT_EQ(atomic.fetchAndSubOrdered(parcel2), newValue1);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if(!needToPreventOverflow) {
         ASSERT_EQ(atomic.fetchAndSubOrdered(parcel2), static_cast<TypeParam>(value));
         ASSERT_EQ(atomic.load(), newValue2);
         ASSERT_EQ(atomic.fetchAndSubOrdered(parcel1), newValue2);
      }
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      if (!needToPreventUnderflow) {
         ASSERT_EQ(atomic -= parcel1, newValue1);
         ASSERT_EQ(atomic -= parcel2, static_cast<TypeParam>(value));
      }
      
      if (!needToPreventOverflow) {
         ASSERT_EQ(atomic -= parcel2, newValue2);
         ASSERT_EQ(atomic -= parcel1, static_cast<TypeParam>(value));
      }
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testFetchAndOr)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      TypeParam zero = 0;
      TypeParam one = 1;
      TypeParam minusOne = static_cast<TypeParam>(~0);
      
      ASSERT_EQ(atomic.fetchAndOrRelaxed(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndOrRelaxed(one), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.fetchAndOrRelaxed(minusOne), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.load(), minusOne);
      atomic.store(value);
      
      ASSERT_EQ(atomic.fetchAndOrAcquire(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndOrAcquire(one), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.fetchAndOrAcquire(minusOne), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.load(), minusOne);
      atomic.store(value);
      
      ASSERT_EQ(atomic.fetchAndOrRelease(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndOrRelease(one), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.fetchAndOrRelease(minusOne), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.load(), minusOne);
      atomic.store(value);
      
      ASSERT_EQ(atomic.fetchAndOrOrdered(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndOrOrdered(one), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.fetchAndOrOrdered(minusOne), static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic.load(), minusOne);
      atomic.store(value);
      
      ASSERT_EQ(atomic |= zero, static_cast<TypeParam>(value));
      ASSERT_EQ(atomic |= one, static_cast<TypeParam>(value | 1));
      ASSERT_EQ(atomic |= minusOne, minusOne);
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testFetchAndAnd)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      TypeParam zero = 0;
      TypeParam f = 0xf;
      TypeParam minusOne = static_cast<TypeParam>(~0);
      
      ASSERT_EQ(atomic.fetchAndAndRelaxed(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndAndRelaxed(f), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.fetchAndAndRelaxed(zero), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.load(), zero);
      atomic.store(value);
      
      ASSERT_EQ(atomic.fetchAndAndAcquire(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndAndAcquire(f), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.fetchAndAndAcquire(zero), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.load(), zero);
      atomic.store(value);
      
      ASSERT_EQ(atomic.fetchAndAndRelease(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndAndRelease(f), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.fetchAndAndRelease(zero), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.load(), zero);
      atomic.store(value);
      
      ASSERT_EQ(atomic.fetchAndAndOrdered(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndAndOrdered(f), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.fetchAndAndOrdered(zero), static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic.load(), zero);
      atomic.store(value);
      
      ASSERT_EQ(atomic &= minusOne, static_cast<TypeParam>(value));
      ASSERT_EQ(atomic &= f, static_cast<TypeParam>(value & 0xf));
      ASSERT_EQ(atomic &= zero, zero);
      ++begin;
   }
}

TYPED_TEST(AtomicIntegerTest, testFetchAndXor)
{
   using LargeInt = typename std::conditional<std::is_signed<TypeParam>::value, pdk::pint64, pdk::puint64>::type;
   std::list<LargeInt> data;
   init_test_data<TypeParam, LargeInt>(data);
   typename std::list<LargeInt>::iterator begin = data.begin();
   typename std::list<LargeInt>::iterator end = data.end();
   while (begin != end) {
      LargeInt value = *begin;
      AtomicInteger<TypeParam> atomic(value);
      TypeParam zero = 0;
      TypeParam pattern = static_cast<TypeParam>(PDK_UINT64_C(0xcccccccccccccccc));
      TypeParam minusOne = static_cast<TypeParam>(~0);
      
      ASSERT_EQ(atomic.fetchAndXorRelaxed(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorRelaxed(pattern), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.fetchAndXorRelaxed(pattern), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorRelaxed(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.fetchAndXorRelaxed(minusOne), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic.fetchAndXorAcquire(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorAcquire(pattern), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.fetchAndXorAcquire(pattern), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorAcquire(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.fetchAndXorAcquire(minusOne), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic.fetchAndXorRelease(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorRelease(pattern), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.fetchAndXorRelease(pattern), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorRelease(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.fetchAndXorRelease(minusOne), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic.fetchAndXorOrdered(zero), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorOrdered(pattern), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.fetchAndXorOrdered(pattern), static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.fetchAndXorOrdered(minusOne), static_cast<TypeParam>(value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.fetchAndXorOrdered(minusOne), static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic.load(), static_cast<TypeParam>(value));
      
      ASSERT_EQ(atomic ^= zero, static_cast<TypeParam>(value));
      ASSERT_EQ(atomic ^= pattern, static_cast<TypeParam>(value ^ pattern));
      ASSERT_EQ(atomic ^= pattern, static_cast<TypeParam>(value));
      ASSERT_EQ(atomic ^= minusOne, static_cast<TypeParam>(~value));
      ASSERT_EQ(atomic ^= minusOne, static_cast<TypeParam>(value));
      ++begin;
   }
}
