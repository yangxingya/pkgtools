//
// @brief: smart ptr define, vc and gcc.
//


#ifndef SMARTPTR_DEF_H_
#define SMARTPTR_DEF_H_

#ifdef _MSC_VER
# include <memory>
# if _MSC_VER >= 1600   // vc2010 ---> c++11 supported, change namespace.
   using ::std::shared_ptr;
   using ::std::weak_ptr;
   using ::std::unique_ptr;
# elif _MSC_VER >= 1500 // vc2008 ---> c++ tr1 shared_ptr namespace.
   using ::std::tr1::shared_ptr;
   using ::std::tr1::weak_ptr;
   using ::std::auto_ptr;
#  define unique_ptr auto_ptr   // unique_ptr in vc2008 is auto_ptr.
# else
#  error not supported shared_ptr;
# endif
#else
# ifdef __GNUC__
#  error not write header special;
# endif
#endif

#endif // SMARTPTR_DEF_H_