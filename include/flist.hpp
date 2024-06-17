#ifndef PBULIST_H
#define PBULIST_H

#include "allocator.hpp"

namespace pbu
{

template <typename T>
class FListNode
{
public:
   T v;
   FListNode* next;

   FListNode(const T& _v)
       : v{_v}, next{nullptr}
   {
   }
   FListNode(T&& _v)
       : v{PBU_MOV(_v)}, next{nullptr}
   {
   }
};

template <typename T>
class FListIterator
{
   FListNode<T>* __current;

public:
   FListIterator(const FListNode<T>& it)
       : __current{&it}
   {
   }
   FListIterator(const FListNode<T>* it)
       : __current{it}
   {
   }
   FListIterator(FListNode<T>* it)
       : __current{it}
   {
   }

   T& operator*() { return __current->v; }
   FListIterator& operator++()
   {
      if (__current)
      {
         __current = __current->next;
      }
      return *this;
   }
   FListIterator operator++(int)
   {
      FListIterator temp = *this;
      ++(*this);
      return temp;
   }
   bool operator==(const FListIterator<T> o)
   {
      return __current == o.__current;
   }
   bool operator!=(const FListIterator<T> o)
   {
      return __current != o.__current;
   }
};

template <typename T, typename Allocator = pbu::Allocator<T>>
class FList
{
   using Node = FListNode<T>;

   pbu::Allocator<Node> __allocator;
   Node* __head;
   Node* __tail;

public:
   FList()
       : __head{nullptr}, __tail{nullptr}
   {
   }
   ~FList() { clear(); }

   void clear()
   {
      Node* current = __head;
      while (current)
      {
         Node* next = current->next;
         __allocator.destroy(current);
         __allocator.deallocate(current, 1);
         current = next;
      }
      __head = __tail = nullptr;
   }

   void add(const T& v)
   {
      Node* n = __allocator.allocate(1);
      __allocator.construct(n, v);
      if (!__head)
      {
         __head = n;
         __tail = n;
      }
      else
      {
         __tail->next = n;
         __tail       = n;
      }
   }

   void rem() { __allocator.destroy(__tail); }

   FListIterator<T> begin() noexcept { return FListIterator<T>(__head); }
   FListIterator<T> end() noexcept { return FListIterator<T>(__tail); }
   const FListIterator<T> begin() const noexcept
   {
      return FListIterator<T>(__head);
   }
   const FListIterator<T> end() const noexcept
   {
      return FListIterator<T>(__tail);
   }
};
};  // namespace pbu

#endif
