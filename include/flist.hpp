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

   FListNode& operator++() {
      return next;
   }
   FListNode operator++(int) {}
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
         __allocator.destroy(next);
         __allocator.deallocate(next, 1);
         current = next;
      }
      __head = __tail = nullptr;
   }

   void add_back(const T& v)
   {
      Node* n = __allocator.allocate(1);
      __allocator.construct(n, v);
      if (__tail)
      {
         __tail->next = n;
      }
      else
      {
         __head = n;
      }
      __tail = n;
   }
   void add_front(T&& v) {}

   T* begin() noexcept { return &__head->v; }

   T* end() noexcept { return &__tail->v; }
};

};  // namespace pbu

#endif
