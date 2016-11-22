// functionalHelpers.h

// Providing convenient functional-style helper functions for C++ structures
// that can be invoked without using STL iterators and will return a new
// structure rather than modifying an existing one in place.
//
// These functions are defined using a couple of methods to make them general
// purpose and not require any template arguments.
// 1. In some cases general versions of the functions are defined as a
//    precompiler macro that takes the name of the container class it will
//    return as one of its parameters.
// 2. Some utility functions are defined and overloaded for all the various
//    supported container types in order to allow performing common tasks
//    such as adding an item to the container. In the case of a reserving
//    space in the container, it may do nothing for classes where such a task
//    is not necessary or applicable.
// 3. Generally functions in <functional> and <algoritms> are not used as
//    they don't always work consistently with Qt's container classes across
//    all of the versions of Qt.
//
// See the documentation for an explanation of how to use these functions.

#ifndef __FUNCTIONAL_HELPERS_H__
#define __FUNCTIONAL_HELPERS_H__

#include <list>
#include <vector>
#include <set>

namespace _FunctionalHelpersUtils {
	// Default behavior of reserveSize is to do nothing
	template<template<class> class T, class U>
	inline void reserveSize(T<U> &container, int size)
	{
		(void)container;
		(void)size;
	}
	
	template<template<class, class> class T, class U, class V>
	inline void reserveSize(T<U, V> &container, int size)
	{
		(void)container;
		(void)size;
	}
	
	template<template<class, class, class> class T, class U, class V, class W>
	inline void reserveSize(T<U, V, W> &container, int size)
	{
		(void)container;
		(void)size;
	}
	
	// In the case of vectors, we actually do want reserveSize() to do something.
	template<class U, class V>
	inline void reserveSize(std::vector<U, V> &container, int size)
	{
		container.reserve(size);
	}
	
	template<template<class> class T, class U>
	inline void addItem(T<U> &container, const U &item)
	{
		container.push_back(item);
	}
	
	template<template<class, class> class T, class U, class V>
	inline void addItem(T<U, V> &container, const U &item)
	{
		container.push_back(item);
	}
	
	template<class U, class V>
	inline void addItem(std::set<U, V> &container, const U &item)
	{
		container.insert(item);
	}
}

// Some of the functions defined using preprocessor macros take another macro
// as a parameter that allows looping over the container. We do this instead of
// just universally using a ranged for because apparently Qt's own macro
// (Q_FOREACH) gets better performance on Qt's container classes than a regular
// ranged for does. So we can pass the following macro in when defining a
// function for an STL container so that it uses a ranged for, and pass in
// Q_FOREACH when defining functions for Qt's container classes.
#define __FH_standard_ranged_for(x, y) for(x : y)

// map

#define __FH_general_map(NAME, RET_TYPE, LOOP) \
template <class T, class F> \
auto NAME(const T &list, F &&func) \
	-> RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type> \
{ \
	using U = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type; \
    RET_TYPE<U> result; \
	_FunctionalHelpersUtils::reserveSize(result, list.size()); \
 \
	for(auto const &item : list) { \
		_FunctionalHelpersUtils::addItem(result, std::ref(func)(decltype(item)(item))); \
	} \
 \
    return result; \
}

__FH_general_map(listMap, std::list, __FH_standard_ranged_for)
__FH_general_map(vectorMap, std::vector, __FH_standard_ranged_for)
__FH_general_map(setMap, std::set, __FH_standard_ranged_for)


// compr

#define __FH_general_compr(NAME, RET_TYPE, LOOP) \
template <class T, class F1, class F2> \
auto NAME(const T &list, F1 &&func, F2 &&predicate) \
    -> RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type> \
{ \
    using U = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type; \
    RET_TYPE<U> result; \
 \
	LOOP(auto const &item, list) { \
		if (std::ref(predicate)(decltype(item)(item))) { \
			_FunctionalHelpersUtils::addItem(result, std::ref(func)(decltype(item)(item))); \
		} \
	} \
 \
    return result; \
}

__FH_general_compr(listCompr, std::list, __FH_standard_ranged_for)
__FH_general_compr(vectorCompr, std::vector, __FH_standard_ranged_for)
__FH_general_compr(setCompr, std::set, __FH_standard_ranged_for)

// filter

#define __FH_general_filter(NAME, RET_TYPE, LOOP) \
template <class T, class F> \
auto NAME(const T &list, F &&func) \
    -> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
    using U = typename std::decay<decltype(*list.begin())>::type; \
    RET_TYPE<U> result; \
 \
	LOOP(auto const &item, list) { \
		if (std::ref(func)(decltype(item)(item))) { \
			_FunctionalHelpersUtils::addItem(result, item); \
		} \
	} \
 \
    return result; \
}

__FH_general_filter(listFilter, std::list, __FH_standard_ranged_for)
__FH_general_filter(vectorFilter, std::vector, __FH_standard_ranged_for)
__FH_general_filter(setFilter, std::set, __FH_standard_ranged_for)

// all of

template <class T, class F>
bool allOf(T const &list, F &&f)
{
	using U = decltype(list.cbegin());
	for(U it = list.cbegin(); it != list.cend(); ++it) {
		if (!std::ref(f)(decltype(*it)(*it))) {
			return false;
		}
	}
	return true;
}

// any of

template <class T, class F>
bool anyOf(T const &list, F &&f)
{
	using U = decltype(list.cbegin());
	for(U it = list.cbegin(); it != list.cend(); ++it) {
		if (std::ref(f)(decltype(*it)(*it))) {
			return true;
		}
	}
	return false;
}

// extremum

template <class T, class F>
auto extremum(const T &list, F &&comp)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using U = typename std::decay<decltype(*list.begin())>::type;
	using V = decltype(list.cbegin());
	const U *extremumValue = nullptr;
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		if (!extremumValue || std::ref(comp)(decltype(*it)(*it), *extremumValue)) {
			extremumValue = &(*it);
		}
	}

    return *extremumValue;
}

// min(container)

template <class T>
auto min(const T &list)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using U = typename std::decay<decltype(*list.begin())>::type;
	return extremum(list, [] (const U &a, const U &b) {return a < b; });
}

// min(container, func)

template <class T, class F>
auto min(const T &list, F &&func)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using U = typename std::decay<decltype(*list.begin())>::type;
	using V = decltype(list.cbegin());
	using W = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type;
	const U *extremumValue = nullptr;
	W extremumComparator;
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		W currentComparator = std::ref(func)(decltype(*it)(*it));
		
		if (!extremumValue || currentComparator < extremumComparator) {
			extremumValue = &(*it);
			extremumComparator = currentComparator;
		}
	}

    return *extremumValue;
}

// max(container)

template <class T>
auto max(const T &list)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using U = typename std::decay<decltype(*list.begin())>::type;
	return extremum(list, [] (const U &a, const U &b) {return a > b; });
}

// max(container, func)

template <class T, class F>
auto max(const T &list, F &&func)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using U = typename std::decay<decltype(*list.begin())>::type;
	using V = decltype(list.cbegin());
	using W = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type;
	const U *extremumValue = nullptr;
	W extremumComparator;
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		W currentComparator = std::ref(func)(decltype(*it)(*it));
		
		if (!extremumValue || currentComparator > extremumComparator) {
			extremumValue = &(*it);
			extremumComparator = currentComparator;
		}
	}

    return *extremumValue;
}

// reduce

template <class T, class F, class U>
auto reduce(const T &list, F &&func, U memo)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using V = decltype(list.cbegin());
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		memo = std::ref(func)(U(memo), decltype(*it)(*it));
	}

    return memo;
}

// sum

template <class T, class U>
auto sum(const T &list, U memo)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return reduce(list, [] (const U &a, const U &b) { return a+b; }, memo);
}

template <class T>
auto sum(const T &list)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using U = typename std::decay<decltype(*list.begin())>::type;
	U memo = U();
	return reduce(list, [] (const U &a, const U &b) { return a+b; }, memo);
}

#endif // __FUNCTIONAL_HELPERS_H__
