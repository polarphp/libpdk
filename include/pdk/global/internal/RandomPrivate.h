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
// Created by softboy on 2018/02/24.

#ifndef PDK_GLOBAL_INTERNAL_RANDOM_PRIVATE_H
#define PDK_GLOBAL_INTERNAL_RANDOM_PRIVATE_H

#include "pdk/global/Global.h"
#include "pdk/pal/kernel/Simd.h"

namespace pdk {
namespace internal {

using pdk::os::thread::BasicAtomicInteger;

enum class RandomGeneratorControl : long
{
   UseSystemRNG = 1,
   SkipSystemRNG = 2,
   SkipHWRNG = 4,
   SetRandomData = 8,
   
   // 28 bits
   RandomDataMask = 0xfffffff0
};

enum class RNGType 
{
   SystemRNG = 0,
   MersenneTwister = 1
};

PDK_CORE_EXPORT BasicAtomicInteger<uint> sg_randomdeviceControl = PDK_BASIC_ATOMIC_INITIALIZER(0U);

inline bool pdk_has_hwrng()
{
#if defined(PDK_PROCESSOR_X86) && PDK_COMPILER_SUPPORTS_HERE(RDRND)
    return CPU_HAS_FEATURE(RDRND);
#else
    return false;
#endif
}

} // internal
} // pdk

#endif // PDK_GLOBAL_INTERNAL_RANDOM_PRIVATE_H
