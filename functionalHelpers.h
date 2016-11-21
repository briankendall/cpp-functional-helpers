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
// 2. Oftentimes multiple overloaded versions of the function will be defined
//    to handle passing in pointers to member functions of classes. Sometimes
//    there are two versions: one that takes a container of objects and a
//    pointer to a member function from the objects' class, and one that takes
//    a container of object pointers and a pointer to a member function. In
//    some other cases there are four versions: the two cases above for
//    container classes that use two template arguments (such as the STL
//    containers) and two more for container classes that use one template
//    argument (such as Qt's container classes).
// 3. Some utility functions are defined and overloaded for all the various
//    supported container types in order to allow performing common tasks
//    such as adding an item to the container. In the case of a reserving
//    space in the container, it may do nothing for classes where such a task
//    is not necessary or applicable.
// 4. Generally functions in <functional> and <algoritms> are not used as
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
auto NAME(const T &list, F&& func) \
    -> RET_TYPE<typename std::decay<decltype(func(*list.begin()))>::type> \
{ \
    using U = typename std::decay<decltype(func(*list.begin()))>::type; \
    RET_TYPE<U> result; \
	_FunctionalHelpersUtils::reserveSize(result, list.size()); \
 \
	LOOP(auto const &item, list) { \
		_FunctionalHelpersUtils::addItem(result, func(item)); \
	} \
 \
    return result; \
} \
 \
template <class T, class F> \
auto NAME(const T &list, F&& func) \
    -> RET_TYPE<typename std::decay<decltype(((*list.begin()).*func)())>::type> \
{ \
	using U = typename std::decay<decltype(*list.begin())>::type; \
    return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
 \
template <class T, class F> \
auto NAME(const T &list, F&& func) \
    -> RET_TYPE<typename std::decay<decltype(((*list.begin())->*func)())>::type> \
{ \
	using U = typename std::decay<decltype(*list.begin())>::type; \
    return NAME(list, [=](U const &t){ return (t->*func)(); }); \
} \

__FH_general_map(listMap, std::list, __FH_standard_ranged_for)
__FH_general_map(vectorMap, std::vector, __FH_standard_ranged_for)
__FH_general_map(setMap, std::set, __FH_standard_ranged_for)

// compr

// Unlike a lot of the other functions in this file, compr has six overloaded
// versions in order to handle the following cases, as denoted by the type
// of argument that is passed in:
// 1. container of objects, pointer to member function, pointer to member
//    function
// 2. container of objects, pointer to member function, general callable
// 3. container of objects, general callable, pointer to member function
// 4. container of object pointers, pointer to member function, pointer to
//    member function
// 5. container of object pointers, pointer to member function, general
//    callable
// 6. container of object pointers, general callable, pointer to member
//    function

#define __FH_general_compr(NAME, RET_TYPE, LOOP) \
template <class T, class F1, class F2> \
auto NAME(const T &list, F1 &&func, F2 &&predicate) \
    -> RET_TYPE<typename std::decay<decltype(func(*list.begin()))>::type> \
{ \
    using U = typename std::decay<decltype(func(*list.begin()))>::type; \
    RET_TYPE<U> result; \
 \
	LOOP(auto const &item, list) { \
		if (predicate(item)) { \
			_FunctionalHelpersUtils::addItem(result, func(item)); \
		} \
	} \
 \
    return result; \
} \
 \
template <template<class, class> class T, class U, class V, class W> \
RET_TYPE<W> NAME(const T<U, V> &list, W (U::*func)() const, bool (U::*predicate)() const) \
{ \
	return NAME(list, [=] (U const &a){ return (a.*func)(); }, [=] (U const &a) { return (a.*predicate)(); }); \
} \
 \
template <template<class, class> class T, class U, class V, class W, class F> \
RET_TYPE<W> NAME(const T<U, V> &list, W (U::*func)() const, F &&predicate) \
{ \
	return NAME(list, [=] (U const &a){ return (a.*func)(); }, predicate); \
} \
 \
template <template<class, class> class T, class U, class V, class F> \
auto NAME(const T<U, V> &list, F &&func, bool (U::*predicate)() const) \
	-> RET_TYPE<typename std::decay<decltype(func(*list.begin()))>::type> \
{ \
	return NAME(list, func, [=] (U const &a) { return (a.*predicate)(); }); \
} \
 \
template <template<class, class> class T, class U, class V, class W> \
RET_TYPE<W> NAME(const T<U *, V> &list, W (U::*func)() const, bool (U::*predicate)() const) \
{ \
	return NAME(list, [=] (U const *a){ return (a->*func)(); }, [=] (U const *a) { return (a->*predicate)(); }); \
} \
 \
template <template<class, class> class T, class U, class V, class W, class F> \
RET_TYPE<W> NAME(const T<U *, V> &list, W (U::*func)() const, F &&predicate) \
{ \
	return NAME(list, [=] (U const *a){ return (a->*func)(); }, predicate); \
} \
 \
template <template<class, class> class T, class U, class V, class F> \
auto NAME(const T<U *, V> &list, F &&func, bool (U::*predicate)() const) \
	-> RET_TYPE<typename std::decay<decltype(func(*list.begin()))>::type> \
{ \
	return NAME(list, func, [=] (U const *a) { return (a->*predicate)(); }); \
}

__FH_general_compr(listCompr, std::list, __FH_standard_ranged_for)
__FH_general_compr(vectorCompr, std::vector, __FH_standard_ranged_for)
__FH_general_compr(setCompr, std::set, __FH_standard_ranged_for)

// filter

#define __FH_general_filter(NAME, RET_TYPE, LOOP) \
template <class T, class F> \
auto NAME(const T &list, F&& func) \
    -> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
    using U = typename std::decay<decltype(*list.begin())>::type; \
    RET_TYPE<U> result; \
 \
	LOOP(auto const &item, list) { \
		if (func(item)) { \
			_FunctionalHelpersUtils::addItem(result, item); \
		} \
	} \
 \
    return result; \
} \
 \
template <template<class, class> class T, class U, class V> \
auto NAME(const T<U, V> &list, bool (U::*func)() const) \
	-> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
	return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
 \
template <template<class, class> class T, class U, class V> \
auto NAME(const T<U *, V> &list, bool (U::*func)() const) \
	-> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
	return NAME(list, [=](U const *t){ return (t->*func)(); }); \
} \
 \
template <template<class> class T, class U> \
auto NAME(const T<U> &list, bool (U::*func)() const) \
   -> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
   return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
\
template <template<class> class T, class U> \
auto NAME(const T<U *> &list, bool (U::*func)() const) \
   -> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
   return NAME(list, [=](U const *t){ return (t->*func)(); }); \
} \

__FH_general_filter(listFilter, std::list, __FH_standard_ranged_for)
__FH_general_filter(vectorFilter, std::vector, __FH_standard_ranged_for)
__FH_general_filter(setFilter, std::set, __FH_standard_ranged_for)

// all of

template <class T, class F>
bool allOf(const T &list, F &&func)
{
	using U = decltype(list.cbegin());
	for(U it = list.cbegin(); it != list.cend(); ++it) {
		if (!func(*it)) {
			return false;
		}
	}

    return true;
}

template <template<class, class> class T, class U, class V>
bool allOf(const T<U, V> &list, bool (U::*func)() const)
{
	return allOf(list, [=](U const &t){ return (t.*func)(); });
}

template <template<class, class> class T, class U, class V>
bool allOf(const T<U *, V> &list, bool (U::*func)() const)
{
	return allOf(list, [=](U const *t){ return (t->*func)(); });
}

template <template<class> class T, class U>
bool allOf(const T<U> &list, bool (U::*func)() const)
{
	return allOf(list, [=](U const &t){ return (t.*func)(); });
}

template <template<class> class T, class U>
bool allOf(const T<U *> &list, bool (U::*func)() const)
{
	return allOf(list, [=](U const *t){ return (t->*func)(); });
}

// any of

template <class T, class F>
bool anyOf(const T &list, F &&func)
{
	using U = decltype(list.cbegin());
	for(U it = list.cbegin(); it != list.cend(); ++it) {
		if (func(*it)) {
			return true;
		}
	}

    return false;
}

template <template<class, class> class T, class U, class V>
bool anyOf(const T<U, V> &list, bool (U::*func)() const)
{
	return anyOf(list, [=](U const &t){ return (t.*func)(); });
}

template <template<class, class> class T, class U, class V>
bool anyOf(const T<U *, V> &list, bool (U::*func)() const)
{
	return anyOf(list, [=](U const *t){ return (t->*func)(); });
}

template <template<class> class T, class U>
bool anyOf(const T<U> &list, bool (U::*func)() const)
{
	return anyOf(list, [=](U const &t){ return (t.*func)(); });
}

template <template<class> class T, class U>
bool anyOf(const T<U *> &list, bool (U::*func)() const)
{
	return anyOf(list, [=](U const *t){ return (t->*func)(); });
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
		if (!extremumValue || comp(*it, *extremumValue)) {
			extremumValue = &(*it);
		}
	}

    return *extremumValue;
}

template <template<class, class> class T, class U, class V>
auto extremum(const T<U, V> &list, bool (U::*func)(const U &) const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return extremum(list, [=](U const &a, U const &b){ return (a.*func)(b); });
}

template <template<class, class> class T, class U, class V>
auto extremum(const T<U *, V> &list, bool (U::*func)(const U &) const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return extremum(list, [=](U const *a, U const *b){ return (a->*func)(*b); });
}

template <template<class> class T, class U>
auto extremum(const T<U> &list, bool (U::*func)(const U &) const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return extremum(list, [=](U const &a, U const &b){ return (a.*func)(b); });
}

template <template<class> class T, class U>
auto extremum(const T<U *> &list, bool (U::*func)(const U &) const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return extremum(list, [=](U const *a, U const *b){ return (a->*func)(*b); });
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
	using W = typename std::decay<decltype(func(*list.begin()))>::type;
	const U *extremumValue = nullptr;
	W extremumComparator;
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		W currentComparator = func(*it);
		
		if (!extremumValue || currentComparator < extremumComparator) {
			extremumValue = &(*it);
			extremumComparator = currentComparator;
		}
	}

    return *extremumValue;
}

template <template<class, class> class T, class U, class V, class W>
auto min(const T<U, V> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return min(list, [=](U const &a){ return (a.*func)(); });
}

template <template<class, class> class T, class U, class V, class W>
auto min(const T<U *, V> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return min(list, [=](U const *a){ return (a->*func)(); });
}

template <template<class> class T, class U, class W>
auto min(const T<U> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return min(list, [=](U const &a){ return (a.*func)(); });
}

template <template<class> class T, class U, class W>
auto min(const T<U *> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return min(list, [=](U const *a){ return (a->*func)(); });
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
	using W = typename std::decay<decltype(func(*list.begin()))>::type;
	const U *extremumValue = nullptr;
	W extremumComparator;
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		W currentComparator = func(*it);
		
		if (!extremumValue || currentComparator > extremumComparator) {
			extremumValue = &(*it);
			extremumComparator = currentComparator;
		}
	}

    return *extremumValue;
}

template <template<class, class> class T, class U, class V, class W>
auto max(const T<U, V> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return max(list, [=](U const &a){ return (a.*func)(); });
}

template <template<class, class> class T, class U, class V, class W>
auto max(const T<U *, V> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return max(list, [=](U const *a){ return (a->*func)(); });
}

template <template<class> class T, class U, class W>
auto max(const T<U> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return max(list, [=](U const &a){ return (a.*func)(); });
}

template <template<class> class T, class U, class W>
auto max(const T<U *> &list, W (U::*func)() const)
	-> typename std::decay<decltype(*list.begin())>::type
{
	return max(list, [=](U const *a){ return (a->*func)(); });
}

// reduce

template <class T, class F, class U>
auto reduce(const T &list, F &&func, U memo)
	-> typename std::decay<decltype(*list.begin())>::type
{
	using V = decltype(list.cbegin());
	
	for(V it = list.cbegin(); it != list.cend(); ++it) {
		memo = func(memo, *it);
	}

    return memo;
}

template <template<class, class> class T, class U, class V>
U reduce(const T<U, V> &list, U (U::*func)(const U &) const, const U &memo)
{
	return reduce(list, [=](U const &a, U const &b){ return (a.*func)(b); }, memo);
}

template <template<class> class T, class U>
U reduce(const T<U> &list, U (U::*func)(const U &) const, const U &memo)
{
	return reduce(list, [=](U const &a, U const &b){ return (a.*func)(b); }, memo);
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
