//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2013. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/container for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/container/detail/config_begin.hpp>
#include <algorithm>
#include <memory>
#include <deque>
#include <iostream>
#include <functional>
#include <list>

#include <boost/container/deque.hpp>
#include <boost/container/allocator.hpp>
#include <boost/container/node_allocator.hpp>
#include <boost/container/adaptive_pool.hpp>

#include "print_container.hpp"
#include "check_equal_containers.hpp"
#include "dummy_test_allocator.hpp"
#include "movable_int.hpp"
#include <boost/move/utility.hpp>
#include <boost/move/iterator.hpp>
#include <boost/container/detail/mpl.hpp>
#include <boost/container/detail/type_traits.hpp>
#include <string>
#include "emplace_test.hpp"
#include "propagate_allocator_test.hpp"
#include "vector_test.hpp"
#include "default_init_test.hpp"
#include <boost/detail/no_exceptions_support.hpp>

using namespace boost::container;

namespace boost {
namespace container {

//Explicit instantiation to detect compilation errors
template class boost::container::deque
 < test::movable_and_copyable_int
 , test::simple_allocator<test::movable_and_copyable_int> >;

template class boost::container::deque
 < test::movable_and_copyable_int
 , test::dummy_test_allocator<test::movable_and_copyable_int> >;

template class boost::container::deque
 < test::movable_and_copyable_int
 , std::allocator<test::movable_and_copyable_int> >;

template class boost::container::deque
   < test::movable_and_copyable_int
   , allocator<test::movable_and_copyable_int> >;

template class boost::container::deque
   < test::movable_and_copyable_int
   , adaptive_pool<test::movable_and_copyable_int> >;

template class boost::container::deque
   < test::movable_and_copyable_int
   , node_allocator<test::movable_and_copyable_int> >;

}}

//Function to check if both sets are equal
template<class V1, class V2>
bool deque_copyable_only(V1 *, V2 *, container_detail::false_type)
{
   return true;
}

//Function to check if both sets are equal
template<class V1, class V2>
bool deque_copyable_only(V1 *cntdeque, V2 *stddeque, container_detail::true_type)
{
   typedef typename V1::value_type IntType;
   std::size_t size = cntdeque->size();
   stddeque->insert(stddeque->end(), 50, 1);
   cntdeque->insert(cntdeque->end(), 50, IntType(1));
   if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
   {
      IntType move_me(1);
      stddeque->insert(stddeque->begin()+size/2, 50, 1);
      cntdeque->insert(cntdeque->begin()+size/2, 50, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
   }
   {
      IntType move_me(2);
      cntdeque->assign(cntdeque->size()/2, boost::move(move_me));
      stddeque->assign(stddeque->size()/2, 2);
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
   }
   {
      IntType move_me(1);
      stddeque->clear();
      cntdeque->clear();
      stddeque->insert(stddeque->begin(), 50, 1);
      cntdeque->insert(cntdeque->begin(), 50, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
      stddeque->insert(stddeque->begin()+20, 50, 1);
      cntdeque->insert(cntdeque->begin()+20, 50, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
      stddeque->insert(stddeque->begin()+20, 20, 1);
      cntdeque->insert(cntdeque->begin()+20, 20, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
   }
   {
      IntType move_me(1);
      stddeque->clear();
      cntdeque->clear();
      stddeque->insert(stddeque->end(), 50, 1);
      cntdeque->insert(cntdeque->end(), 50, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
      stddeque->insert(stddeque->end()-20, 50, 1);
      cntdeque->insert(cntdeque->end()-20, 50, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
      stddeque->insert(stddeque->end()-20, 20, 1);
      cntdeque->insert(cntdeque->end()-20, 20, boost::move(move_me));
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
   }

   return true;
}

//Test recursive structures
class recursive_deque
{
public:

   recursive_deque & operator=(const recursive_deque &x)
   {  this->deque_ = x.deque_;   return *this; }

   int id_;
   deque<recursive_deque> deque_;
};

template<class IntType>
bool do_test()
{
   //Test for recursive types
   {
      deque<recursive_deque> recursive_deque_deque;
   }

   {
      //Now test move semantics
      deque<recursive_deque> original;
      deque<recursive_deque> move_ctor(boost::move(original));
      deque<recursive_deque> move_assign;
      move_assign = boost::move(move_ctor);
      move_assign.swap(original);
   }

   //Alias deque types
   typedef deque<IntType>  MyCntDeque;
   typedef std::deque<int> MyStdDeque;
   const int max = 100;
   BOOST_TRY{
      MyCntDeque *cntdeque = new MyCntDeque;
      MyStdDeque *stddeque = new MyStdDeque;
      for(int i = 0; i < max*100; ++i){
         IntType move_me(i);
         cntdeque->insert(cntdeque->end(), boost::move(move_me));
         stddeque->insert(stddeque->end(), i);
      }
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      cntdeque->clear();
      stddeque->clear();

      for(int i = 0; i < max*100; ++i){
         IntType move_me(i);
         cntdeque->push_back(boost::move(move_me));
         stddeque->push_back(i);
      }
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      cntdeque->clear();
      stddeque->clear();

      for(int i = 0; i < max*100; ++i){
         IntType move_me(i);
         cntdeque->push_front(boost::move(move_me));
         stddeque->push_front(i);
      }
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      typename MyCntDeque::iterator it;
      typename MyCntDeque::const_iterator cit = it;
      (void)cit;

      cntdeque->erase(cntdeque->begin()++);
      stddeque->erase(stddeque->begin()++);
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      cntdeque->erase(cntdeque->begin());
      stddeque->erase(stddeque->begin());
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      {
         //Initialize values
         IntType aux_vect[50];
         for(int i = 0; i < 50; ++i){
            IntType move_me (-1);
            aux_vect[i] = boost::move(move_me);
         }
         int aux_vect2[50];
         for(int i = 0; i < 50; ++i){
            aux_vect2[i] = -1;
         }

         cntdeque->insert(cntdeque->end()
                           ,boost::make_move_iterator(&aux_vect[0])
                           ,boost::make_move_iterator(aux_vect + 50));
         stddeque->insert(stddeque->end(), aux_vect2, aux_vect2 + 50);
         if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

         for(int i = 0; i < 50; ++i){
            IntType move_me (i);
            aux_vect[i] = boost::move(move_me);
         }
         for(int i = 0; i < 50; ++i){
            aux_vect2[i] = i;
         }

         cntdeque->insert(cntdeque->begin()+cntdeque->size()
                           ,boost::make_move_iterator(&aux_vect[0])
                           ,boost::make_move_iterator(aux_vect + 50));
         stddeque->insert(stddeque->begin()+stddeque->size(), aux_vect2, aux_vect2 + 50);
         if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

         for(int i = 0, j = static_cast<int>(cntdeque->size()); i < j; ++i){
            cntdeque->erase(cntdeque->begin());
            stddeque->erase(stddeque->begin());
         }
         if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
      }
      {
         IntType aux_vect[50];
         for(int i = 0; i < 50; ++i){
            IntType move_me(-1);
            aux_vect[i] = boost::move(move_me);
         }
         int aux_vect2[50];
         for(int i = 0; i < 50; ++i){
            aux_vect2[i] = -1;
         }
         cntdeque->insert(cntdeque->begin()
                           ,boost::make_move_iterator(&aux_vect[0])
                           ,boost::make_move_iterator(aux_vect + 50));
         stddeque->insert(stddeque->begin(), aux_vect2, aux_vect2 + 50);
         if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;
      }

      if(!deque_copyable_only(cntdeque, stddeque
                     ,container_detail::bool_<boost::container::test::is_copyable<IntType>::value>())){
         return false;
      }

      cntdeque->erase(cntdeque->begin());
      stddeque->erase(stddeque->begin());

      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      for(int i = 0; i < max; ++i){
         IntType move_me(i);
         cntdeque->insert(cntdeque->begin(), boost::move(move_me));
         stddeque->insert(stddeque->begin(), i);
      }
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return false;

      //Test insertion from list
      {
         std::list<int> l(50, int(1));
         cntdeque->insert(cntdeque->begin(), l.begin(), l.end());
         stddeque->insert(stddeque->begin(), l.begin(), l.end());
         if(!test::CheckEqualContainers(cntdeque, stddeque)) return 1;
         cntdeque->assign(l.begin(), l.end());
         stddeque->assign(l.begin(), l.end());
         if(!test::CheckEqualContainers(cntdeque, stddeque)) return 1;
      }

      cntdeque->resize(100);
      stddeque->resize(100);
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return 1;

      cntdeque->resize(200);
      stddeque->resize(200);
      if(!test::CheckEqualContainers(cntdeque, stddeque)) return 1;

      delete cntdeque;
      delete stddeque;
   }
   BOOST_CATCH(std::exception &ex){
      #ifndef BOOST_NO_EXCEPTIONS
      std::cout << ex.what() << std::endl;
      #endif
      return false;
   }
   BOOST_CATCH_END

   std::cout << std::endl << "Test OK!" << std::endl;
   return true;
}

template<class VoidAllocator>
struct GetAllocatorCont
{
   template<class ValueType>
   struct apply
   {
      typedef deque< ValueType
                    , typename allocator_traits<VoidAllocator>
                        ::template portable_rebind_alloc<ValueType>::type
                    > type;
   };
};

template<class VoidAllocator>
int test_cont_variants()
{
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<int>::type MyCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::movable_int>::type MyMoveCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::movable_and_copyable_int>::type MyCopyMoveCont;
   typedef typename GetAllocatorCont<VoidAllocator>::template apply<test::copyable_int>::type MyCopyCont;

   if(test::vector_test<MyCont>())
      return 1;
   if(test::vector_test<MyMoveCont>())
      return 1;
   if(test::vector_test<MyCopyMoveCont>())
      return 1;
   if(test::vector_test<MyCopyCont>())
      return 1;
   return 0;
}

bool test_support_for_initialization_list()
{
#if !defined(BOOST_NO_CXX11_HDR_INITIALIZER_LIST)
   const std::initializer_list<int> il = {1, 10, 11};
   const deque<int> expectedDeque(il.begin(), il.end());

   const deque<int> testConstructor = il;
   if(testConstructor != expectedDeque)
      return false;

   deque<int> testAssignmentOperator = {11, 12, 23};
   testAssignmentOperator = il;
   if(testConstructor != expectedDeque)
      return false;

   deque<int> testAssignmentMethod = {11, 12, 23};
   testAssignmentMethod.assign(il);
   if(testConstructor != expectedDeque)
      return false;

   deque<int> testInsertMethod = {11};
   testInsertMethod.insert(testInsertMethod.cbegin(), {12, 23});
   if(testConstructor != expectedDeque)
      return false;

   return true;
#endif
   return true;

}

int main ()
{
   if(!do_test<int>())
      return 1;

   if(!do_test<test::movable_int>())
      return 1;

   if(!do_test<test::movable_and_copyable_int>())
      return 1;

   if(!do_test<test::copyable_int>())
      return 1;

   //Test non-copy-move operations
   {
      deque<test::non_copymovable_int> d;
      d.emplace_back();
      d.emplace_front(1);
      d.resize(10);
      d.resize(1);
   }

   ////////////////////////////////////
   //    Allocator implementations
   ////////////////////////////////////
   //       std:allocator
   if(test_cont_variants< std::allocator<void> >()){
      std::cerr << "test_cont_variants< std::allocator<void> > failed" << std::endl;
      return 1;
   }
   //       boost::container::allocator
   if(test_cont_variants< allocator<void> >()){
      std::cerr << "test_cont_variants< allocator<void> > failed" << std::endl;
      return 1;
   }
   //       boost::container::node_allocator
   if(test_cont_variants< node_allocator<void> >()){
      std::cerr << "test_cont_variants< node_allocator<void> > failed" << std::endl;
      return 1;
   }
   //       boost::container::adaptive_pool
   if(test_cont_variants< adaptive_pool<void> >()){
      std::cerr << "test_cont_variants< adaptive_pool<void> > failed" << std::endl;
      return 1;
   }
   ////////////////////////////////////
   //    Default init test
   ////////////////////////////////////
   if(!test::default_init_test< deque<int, test::default_init_allocator<int> > >()){
      std::cerr << "Default init test failed" << std::endl;
      return 1;
   }

   if(!test_support_for_initialization_list())
      return 1;

   ////////////////////////////////////
   //    Emplace testing
   ////////////////////////////////////
   const test::EmplaceOptions Options = (test::EmplaceOptions)(test::EMPLACE_BACK | test::EMPLACE_FRONT | test::EMPLACE_BEFORE);

   if(!boost::container::test::test_emplace
      < deque<test::EmplaceInt>, Options>())
      return 1;
   ////////////////////////////////////
   //    Allocator propagation testing
   ////////////////////////////////////
   if(!boost::container::test::test_propagate_allocator<deque>())
      return 1;

   return 0;
}

#include <boost/container/detail/config_end.hpp>
