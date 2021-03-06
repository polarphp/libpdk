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
// Created by zzu_softboy on 2017/12/08.

#ifndef PDK_BASE_DS_ARRAY_DATA_TEST_SIMPLE_VECTOR_H
#define PDK_BASE_DS_ARRAY_DATA_TEST_SIMPLE_VECTOR_H

#include "pdk/base/ds/internal/ArrayData.h"
#include "pdk/base/ds/internal/ArrayDataPointer.h"

using pdk::ds::internal::ArrayData;
using pdk::ds::internal::TypedArrayData;
using pdk::ds::internal::ArrayDataPointer;
using pdk::ds::internal::ArrayDataPointerRef;

template <typename T>
class SimpleVector
{
private:
   using Data = TypedArrayData<T>;
public:
   using ValueType = T;
   using Iterator = typename Data::Iterator;
   using ConstIterator = typename Data::ConstIterator;
   using value_type = T;
   using iterator = Iterator;
   using const_iterator = ConstIterator;
public:
   SimpleVector()
   {}
   
   explicit SimpleVector(size_t size)
      : m_data(Data::allocate(size))
   {
      if (size) {
         m_data->appendInitialize(size);
      }
   }
   
   SimpleVector(size_t size, const T &data)
      : m_data(Data::allocate(size))
   {
      if (size) {
         m_data->copyAppend(size, data);
      }
   }
   
   SimpleVector(const T *begin, const T *end)
      : m_data(Data::allocate(end - begin))
   {
      if (end - begin) {
         m_data->copyAppend(begin, end);
      }
   }
   
   SimpleVector(ArrayDataPointerRef<T> ptr)
      : m_data(ptr)
   {}
   
   SimpleVector(Data *ptr)
      : m_data(ptr)
   {}
   
   bool empty() const
   {
      return m_data->m_size == 0;
   }
   
   bool isNull() const
   {
      return m_data.isNull();
   }
   
   bool isEmpty() const
   {
      return this->empty();
   }
   
   bool isStatic() const 
   {
      return m_data->m_ref.isStatic();
   }
   
   bool isShared() const
   {
      return m_data->m_ref.isShared();
   }
   
   bool isSharedWith(const SimpleVector &other) const
   {
      return m_data == other.m_data;
   }
#if !defined(PDK_NO_UNSHARABLE_CONTAINERS)
   bool isSharable() const
   {
      return m_data->m_ref.isSharable();
   }
   
   void setSharable(bool sharable)
   {
      m_data.setSharable(sharable);
   }
#endif
   size_t size() const
   {
      return m_data->m_size;
   }
   
   size_t capacity() const
   {
      return m_data->m_alloc;
   }
   
   Iterator begin() 
   {
      detach();
      return m_data->begin();
   }
   
   Iterator end()
   {
      detach();
      return m_data->end();
   }
   
   ConstIterator constBegin() const
   {
      return begin();
   }
   
   ConstIterator constEnd() const
   {
      return end();
   }
   
   ConstIterator begin() const
   {
      return m_data->constBegin();
   }
   
   ConstIterator end() const
   {
      return m_data->constEnd();
   }
   
   T &operator[](size_t i) 
   {
      PDK_ASSERT(i < static_cast<size_t>(m_data->m_size));
      detach();
      return begin()[i];
   }
   
   T &at(size_t i) 
   { 
      PDK_ASSERT(i < static_cast<size_t>(m_data->m_size)); 
      detach(); 
      return begin()[i]; 
   }
   
   const T &operator[](size_t i) const
   {
      PDK_ASSERT(i < static_cast<size_t>(m_data->m_size));
      return begin()[i];
   }
   
   const T &at(size_t i) const
   { 
      PDK_ASSERT(i < static_cast<size_t>(m_data->m_size)); 
      return begin()[i]; 
   }
   
   T &front()
   {
      PDK_ASSERT(!isEmpty());
      detach();
      return *begin();
   }
   
   T &back()
   {
      PDK_ASSERT(!isEmpty());
      detach();
      return *(end() - 1);
   }
   
   const T &front() const
   {
      PDK_ASSERT(!isEmpty());
      return *begin();
   }
   
   const T &back() const
   {
      PDK_ASSERT(!isEmpty());
      return *(end() - 1);
   }
   
   void reserve(size_t n)
   {
      if (n == 0) {
         return;
      }
      if (n <= capacity()) {
         if (m_data->m_capacityReserved) {
            return;
         }
         if (!m_data->m_ref.isShared()) {
            m_data->m_capacityReserved = 1;
            return;
         }
      }
      SimpleVector detached(Data::allocate(std::max(n, size()),
                                           m_data->detachFlags() | Data::CapacityReserved));
      if (size()) {
         detached.m_data->copyAppend(constBegin(), constEnd());
      }
      detached.swap(*this);
   }
   
   void resize(size_t newSize)
   {
      if (size() == newSize) {
         return;
      }
      if (m_data.needsDetach() || newSize > capacity()) {
         SimpleVector<T> detached(Data::allocate(m_data->detachCapacity(newSize), m_data->detachFlags()));
         if (newSize) {
            if (newSize < size()) {
               const T *const begin = constBegin();
               detached.m_data->copyAppend(begin, begin + newSize);
            } else {
               if (size()) {
                  const T *const begin = constBegin();
                  detached.m_data->copyAppend(begin, begin + size());
               }
               detached.m_data->appendInitialize(newSize);
            }
         }
         detached.swap(*this);
         return;
      }
      if (newSize > size()) {
         m_data->appendInitialize(newSize);
      } else {
         m_data->truncate(newSize);
      }
   }
   
   void prepend(const_iterator first, const_iterator last)
   {
      if (!m_data->m_size) {
         append(first, last);
         return;
      }
      if (first == last) {
         return;
      }
      T *const begin = m_data->begin();
      if (m_data.needsDetach()
          || (capacity() - size() < static_cast<size_t>(last - first))) {
         SimpleVector<T> detached(Data::allocate(
                                     m_data->detachCapacity(size() + (last - first)),
                                     m_data->detachFlags() | Data::Grow));
         detached.m_data->copyAppend(first, last);
         detached.m_data->copyAppend(begin, begin + m_data->m_size);
         detached.swap(*this);
         return;
      }
      m_data->insert(begin, first, last);
   }
   
   void append(const_iterator first, const_iterator last)
   {
      if (first == last) {
         return;
      }
      if (m_data.needsDetach()
          || (capacity() - size() < static_cast<size_t>(last - first))) {
         SimpleVector<T> detached(Data::allocate(
                                     m_data->detachCapacity(size() + (last - first)),
                                     m_data->detachFlags() | Data::Grow));
         if (m_data->m_size) {
            const T *const begin = constBegin();
            detached.m_data->copyAppend(begin, begin + m_data->m_size);
         }
         detached.m_data->copyAppend(first, last);
         detached.swap(*this);
         return;
      }
      m_data->copyAppend(first, last);
   }
   
   void insert(int position, const_iterator first, const_iterator last)
   {
      if (position < 0) {
         position += m_data->m_size + 1;
      }
      if (position <= 0) {
         prepend(first, last);
         return;
      }
      if (static_cast<size_t>(position) >= size()) {
         append(first, last);
         return;
      }
      if (first == last) {
         return;
      }
      const Iterator begin = m_data->begin();
      const Iterator where = begin + position;
      const Iterator end = begin + m_data->m_size;
      if (m_data.needsDetach()
          || (capacity() - size() < static_cast<size_t>(last - first))) {
         SimpleVector<T> detached(Data::allocate(
                                     m_data->detachCapacity(size() + (last - first)),
                                     m_data->detachFlags() | Data::Grow));
         if (position) {
            detached.m_data->copyAppend(begin, where);
         }
         detached.m_data->copyAppend(first, last);
         detached.m_data->copyAppend(where, end);
         detached.swap(*this);
         return;
      }
      
      if ((first >= where && first < end)
          || (last > where && last <= end)) {
         // Copy overlapping data first and only then shuffle it into place
         iterator start = m_data->begin() + position;
         iterator middle = m_data->end();
         m_data->copyAppend(first, last);
         std::rotate(start, middle, m_data->end());
         return;
      }
      m_data->insert(where, first, last);
   }
   
   void erase(Iterator first, Iterator last)
   {
      if (first == last) {
         return;
      }
      const T *const begin = m_data->begin();
      const T *const end = begin + m_data->m_size;
      if (m_data.needsDetach()) {
         SimpleVector<T> detached(Data::allocate(
                                     m_data->detachCapacity(size() - (last - first)),
                                     m_data->detachFlags()));
         if (first != begin) {
            detached.m_data->copyAppend(begin, first);
         }
         detached.m_data->copyAppend(last, end);
         detached.swap(*this);
         return;
      }
      if (last == end) {
         m_data->truncate(end - first);
      } else {
         m_data->erase(first, last);
      }
   }
   
   void clear()
   {
      m_data.clear();
   }
   
   void detach()
   {
      m_data.detach();
   }
   
   void swap(SimpleVector<T> &other)
   {
      std::swap(m_data, other.m_data);
   }
   
   static SimpleVector fromRawData(const T *data, size_t size,
                                   ArrayData::AllocationOptions options = Data::Default)
   {
      return SimpleVector(Data::fromRawData(data, size, options));
   }
private:
   ArrayDataPointer<T> m_data;
};

template <typename T>
void swap(SimpleVector<T> &v1, SimpleVector<T> &v2)
{
   v1.swap(v2);
}

template <typename T>
inline bool operator ==(const SimpleVector<T> &lhs, const SimpleVector<T> &rhs)
{
   if (lhs.isSharedWith(rhs)) {
      return true;
   }
   if (lhs.size() != rhs.size()) {
      return false;
   }
   return std::equal(lhs.begin(), lhs.end(), rhs.begin());
}

template <typename T>
inline bool operator !=(const SimpleVector<T> &lhs, const SimpleVector<T> &rhs)
{
   return !(lhs == rhs);
}

template <typename T>
inline bool operator <(const SimpleVector<T> &lhs, const SimpleVector<T> &rhs)
{
   return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}

template <typename T>
inline bool operator >(const SimpleVector<T> &lhs, const SimpleVector<T> &rhs)
{
   return rhs < lhs;
}

template <typename T>
inline bool operator <=(const SimpleVector<T> &lhs, const SimpleVector<T> &rhs)
{
   return !(rhs < lhs);
}

template <typename T>
inline bool operator >=(const SimpleVector<T> &lhs, const SimpleVector<T> &rhs)
{
   return !(lhs < rhs);
}

#endif
