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
// Created by softboy on 2018/03/05.


#ifndef PDK_KERNEL_INTERNAL_CORE_COMMAND_LINE_ARGS_PRIVATE_H
#define PDK_KERNEL_INTERNAL_CORE_COMMAND_LINE_ARGS_PRIVATE_H

#include "pdk/global/Global.h"
#include "pdk/base/ds/StringList.h"
#include "pdk/base/lang/String.h"

#if defined(PDK_OS_WIN)
#  ifdef PDK_OS_WIN32
#    include "pdk/global/Windows.h" // first to suppress min, max macros.
#    include <shlobj.h>
#  else
#    include <vector>
#    include "pdk/global/Windows.h"
#  endif

namespace pdk {
namespace kernel {
namespace internal {

using pdk::lang::String;
using pdk::ds::StringList;

namespace {

inline StringList win_cmd_args(const String &cmdLine)
{
   StringList result;
   int size;
   if (wchar_t **argv = CommandLineToArgvW((const wchar_t *)cmdLine.utf16(), &size)) {
      result.reserve(size);
      wchar_t **argvEnd = argv + size;
      for (wchar_t **a = argv; a < argvEnd; ++a) {
         result.append(String::fromWCharArray(*a));
      }
      LocalFree(argv);
   }
   return result;
}

} // anonymous namespace

} // internal
} // kernel
} // pdk

#endif // PDK_OS_WIN

#endif // PDK_KERNEL_INTERNAL_CORE_COMMAND_LINE_ARGS_PRIVATE_H
