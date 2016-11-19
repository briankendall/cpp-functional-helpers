#ifndef __FUNCTIONAL_HELPERS_H__
#define __FUNCTIONAL_HELPERS_H__

#include <list>
#include <vector>
#include <set>

namespace _FunctionalHelpersUtils {
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

#define __FH_general_allOf(NAME, RET_TYPE, LOOP) \
template <class T, class F> \
bool NAME(const T &list, F &&func) \
{ \
	LOOP(auto const &item, list) { \
		if (!func(item)) { \
			return false; \
		} \
	} \
 \
    return true; \
} \
\
template <template<class, class> class T, class U, class V> \
bool NAME(const T<U, V> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
\
template <template<class, class> class T, class U, class V> \
bool NAME(const T<U *, V> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const *t){ return (t->*func)(); }); \
} \
\
template <template<class> class T, class U> \
bool NAME(const T<U> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
\
template <template<class> class T, class U> \
bool NAME(const T<U *> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const *t){ return (t->*func)(); }); \
} \
	
__FH_general_allOf(listAllOf, std::list, __FH_standard_ranged_for)
__FH_general_allOf(vectorAllOf, std::vector, __FH_standard_ranged_for)
__FH_general_allOf(setAllOf, std::set, __FH_standard_ranged_for)

// any of

#define __FH_general_anyOf(NAME, RET_TYPE, LOOP) \
template <class T, class F> \
bool NAME(const T &list, F &&func) \
{ \
	LOOP(auto const &item, list) { \
		if (func(item)) { \
			return true; \
		} \
	} \
 \
    return false; \
} \
\
template <template<class, class> class T, class U, class V> \
bool NAME(const T<U, V> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
\
template <template<class, class> class T, class U, class V> \
bool NAME(const T<U *, V> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const *t){ return (t->*func)(); }); \
} \
\
template <template<class> class T, class U> \
bool NAME(const T<U> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const &t){ return (t.*func)(); }); \
} \
\
template <template<class> class T, class U> \
bool NAME(const T<U *> &list, bool (U::*func)() const) \
{ \
	return NAME(list, [=](U const *t){ return (t->*func)(); }); \
} \
	
__FH_general_anyOf(listAnyOf, std::list, __FH_standard_ranged_for)
__FH_general_anyOf(vectorAnyOf, std::vector, __FH_standard_ranged_for)
__FH_general_anyOf(setAnyOf, std::set, __FH_standard_ranged_for)

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


#endif // __FUNCTIONAL_HELPERS_H__
