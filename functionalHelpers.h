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

#endif // __FUNCTIONAL_HELPERS_H__
