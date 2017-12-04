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
// Created by softboy on 2017/12/04.

#ifndef PDK_M_BASE_DS_INTERNAL_ARRAYDATA_H
#define PDK_M_BASE_DS_INTERNAL_ARRAYDATA_H

#include "pdk/utils/RefCount.h"

namespace pdk {
namespace ds {
namespace internal {

using pdk::utils::RefCount;

struct PDK_CORE_EXPORT ArrayData
{
   RefCount m_ref;
   int m_size;
   uint m_alloc: 31;
   uint m_capacityReserved: 1;
   pdk::ptrdiff m_offset;
   static const ArrayData sm_sharedNull[2];
   
   enum AllocationOption 
   {
      CapacityReserved = 0x1,
#if !defined(PDK_NO_UNSHARED_CONTAINERS)
      Unsharable = 0x02,
#endif
      RawData = 0x04,
      Grow = 0x08,
      Default = 0
   };
   
   PDK_DECLARE_FLAGS(AllocationOptions, AllocationOption);
   
   void *data()
   {
      PDK_ASSERT(m_size == 0 || m_offset < 0 || static_cast<size_t>(m_offset) >= sizeof(ArrayData));
      return reinterpret_cast<char *>(this) + m_offset;
   }
   
   const void *data() const
   {
      PDK_ASSERT(m_size == 0 || m_offset < 0 || static_cast<size_t>(m_offset) >= sizeof(ArrayData));
      return reinterpret_cast<const char *>(this) + m_offset;
   }
   
   bool isMutable() const
   {
      return m_alloc != 0;
   }
   
   size_t detachCapacity(size_t newSize) const
   {
      if (m_capacityReserved && newSize < m_alloc) {
         return m_alloc; 
      }
      return newSize;
   }
   
   AllocationOptions detachFlags() const
   {
      AllocationOptions result;
      if (m_capacityReserved) {
         result |= CapacityReserved;
      }
      return result;
   }
   
   AllocationOptions cloneFlags() const
   {
      AllocationOptions result;
      if (m_capacityReserved) {
         result |= CapacityReserved;
      }
      return result;
   }
   
   static ArrayData *allocate(size_t objectSize, size_t alignment,
                              size_t capacity, AllocationOptions options = Default) noexcept PDK_REQUIRED_RESULT;
   static void deallocate(ArrayData *data, size_t objectSize, size_t alignment) noexcept;
   static ArrayData *getSharedNull() noexcept
   {
      return const_cast<ArrayData *>(sm_sharedNull);
   }
};

//PDK_DECLARE_OPERATORS_FOR_FLAGS(ArrayData::AllocationOptions)

template <typename T>
struct TypedArrayData : ArrayData
{
   class Iterator
   {
   public:
      using IteratorCategory = std::random_access_iterator_tag;
      using DifferenceType = int;
      using ValueType = T;
      using Pointer = T *;
      using Reference = T &;
   
   public:
      inline Iterator()
         : m_pointer(nullptr)
      {}
      
      inline Iterator(T *next)
         : m_pointer(next)
      {}
      
      inline T &operator*() const
      {
         return *m_pointer;
      }
      
      inline T *operator->() const
      {
         return m_pointer;
      }
      
      inline T &operator[](int index) const
      {
         return *(m_pointer + index);
      }
      
      inline bool operator==(const Iterator &other) const
      {
         return m_pointer == other.m_pointer;
      }
      
      inline bool operator!=(const Iterator &other) const
      {
         return m_pointer != other.m_pointer;
      }
      
      inline bool operator<(const Iterator &other) const
      {
         return m_pointer < other.m_pointer;
      }
      
      inline bool operator<=(const Iterator &other) const
      {
         return m_pointer <= other.m_pointer;
      }
      
      inline bool operator>(const Iterator &other) const
      {
         return m_pointer > other.m_pointer;
      }
      
      inline bool operator>=(const Iterator &other) const
      {
         return m_pointer >= other.m_pointer;
      }
      
      inline Iterator &operator++()
      {
         ++m_pointer;
         return *this;
      }
      
      inline Iterator operator++(int)
      {
         T *orig = this;
         ++m_pointer;
         return orig;
      }
      
      inline Iterator &operator--()
      {
         --m_pointer;
         return *this;
      }
      
      inline Iterator operator--(int)
      {
         T *orig = this;
         --m_pointer;
         return orig;
      }
      
      inline Iterator operator+(int value) const
      {
         return Iterator(m_pointer + value);
      }
      
      inline Iterator operator-(int value) const
      {
         return Iterator(m_pointer - value);
      }
      
      inline Iterator &operator+=(int value)
      {
         m_pointer += value;
         return *this;
      }
      
      inline int operator-(const Iterator &value) const
      {
         return m_pointer - value.m_pointer;
      }
      
      inline Iterator &operator-=(int value)
      {
         m_pointer -= value;
         return *this;
      }
      
      inline operator T*() const
      {
         return m_pointer;
      }
   public:
      T *m_pointer;
   };
   
   friend class Iterator;
   
   class ConstIterator
   {
   public:
      using IteratorCategory = std::random_access_iterator_tag;
      using DifferenceType = int;
      using ValueType = T;
      using Pointer = const T *;
      using Reference = const T &;
   
   public:
      inline ConstIterator()
         : m_pointer(nullptr)
      {}
      
      inline ConstIterator(const T *next)
         : m_pointer(next)
      {}
      
      inline explicit ConstIterator(const ConstIterator &other)
         : m_pointer(other.m_pointer)
      {}
      
      inline const T &operator*() const
      {
         return *m_pointer;
      }
      
      inline const T *operator->() const
      {
         return m_pointer;
      }
      
      inline const T &operator[](int index) const
      {
         return *(m_pointer + index);
      }
      
      inline bool operator==(const ConstIterator &other) const
      {
         return m_pointer == other.m_pointer;
      }
      
      inline bool operator!=(const ConstIterator &other) const
      {
         return m_pointer != other.m_pointer;
      }
      
      inline bool operator<(const ConstIterator &other) const
      {
         return m_pointer < other.m_pointer;
      }
      
      inline bool operator<=(const ConstIterator &other) const
      {
         return m_pointer <= other.m_pointer;
      }
      
      inline bool operator>(const ConstIterator &other) const
      {
         return m_pointer > other.m_pointer;
      }
      
      inline bool operator>=(const ConstIterator &other) const
      {
         return m_pointer >= other.m_pointer;
      }
      
      inline ConstIterator &operator++()
      {
         ++m_pointer;
         return *this;
      }
      
      inline ConstIterator &operator++(int)
      {
         T *orig = m_pointer;
         ++m_pointer;
         return orig;
      }
      
      inline ConstIterator &operator--()
      {
         --m_pointer;
         return *this;
      }
      
      inline ConstIterator &operator--(int)
      {
         T *orig = m_pointer;
         --m_pointer;
         return orig;
      }
      
      inline ConstIterator &operator+=(int value)
      {
         m_pointer += value;
         return *this;
      }

      inline ConstIterator &operator-=(int value)
      {
         m_pointer -= value;
         return *this;
      }
      
      inline ConstIterator operator-(int value) const
      {
         return ConstIterator(m_pointer - value);
      }
      
      inline ConstIterator operator+(int value) const
      {
         return ConstIterator(m_pointer + value);
      }
      
      inline int operator-(const ConstIterator &value) const
      {
         return m_pointer - value.m_pointer;
      }
      
      inline operator const T *() const
      {
         return m_pointer;
      }
   public:
      const T *m_pointer;
   };
   
   friend class ConstIterator;
};

} // internal
} // ds
} // pdk

#endif // PDK_M_BASE_DS_INTERNAL_ARRAYDATA_H