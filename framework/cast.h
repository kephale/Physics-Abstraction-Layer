///\file
/// Replacement header for boost::polymorphic_downcast and boost::polymorphic_cast.

#ifndef PAL_CAST_H
#define PAL_CAST_H

#ifdef PAL_USE_BOOST
	// Directly use boost library
	#include <boost/cast.hpp>

	using boost::polymorphic_cast;
	//using boost::polymorphic_downcast;
#define polymorphic_downcast polymorphic_cast

#else	// PAL_USE_BOOST
	// Use a modified copy of boost library
	#include <assert.h>

	//  It has been demonstrated numerous times that MSVC 6.0 fails silently at link
	//  time if you use a template function which has template parameters that don't
	//  appear in the function's argument list.
	# if defined(MSVC_VER) && MSVC_VER < 1300
	#  define BOOST_EXPLICIT_DEFAULT_TARGET , ::boost::type<Target>* = 0
	# else
	#  define BOOST_EXPLICIT_DEFAULT_TARGET
	# endif


	//  Runtime checked polymorphic downcasts and crosscasts.
	//  Suggested in The C++ Programming Language, 3rd Ed, Bjarne Stroustrup,
	//  section 15.8 exercise 1, page 425.

	template <class Target, class Source>
	inline Target polymorphic_cast(Source* x BOOST_EXPLICIT_DEFAULT_TARGET)
	{
		Target tmp = dynamic_cast<Target>(x);
		if ( tmp == 0 ) throw std::bad_cast();
		return tmp;
	}


	//  Contributed by Dave Abrahams

	//template <class Target, class Source>
	//inline Target polymorphic_downcast(Source* x BOOST_EXPLICIT_DEFAULT_TARGET)
	//{
	//	assert( dynamic_cast<Target>(x) == x );  // detect logic error
	//	return static_cast<Target>(x);
	//}


///\todo Refactor the code and really use a polymorphic_downcast.
#define polymorphic_downcast polymorphic_cast		// TEMPORARY: code needs refactoring.

#endif	// PAL_USE_BOOST

#endif	// PAL_CAST_H
