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
#include <forward_list>
#include <vector>
#include <set>
#include <string>

namespace _FunctionalHelpersUtils {
    // Default behavior of reserveSize is to do nothing
    template<class T>
    inline void reserveSize(T &container, int size)
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
    
    // And also STL strings
    template<class U>
    inline void reserveSize(std::basic_string<U> &container, int size)
    {
        container.reserve(size);
    }
    
    template<class T, class U>
    inline void addItem(T &container, const U &item)
    {
        container.push_back(item);
    }
    
    template<class U, class V>
    inline void addItem(std::set<U, V> &container, const U &item)
    {
        container.insert(item);
    }
    
    // Helper to determine whether there's a const_iterator for T.
    template<typename T>
    struct has_const_iterator {
    private:
        template<typename C> static char test(typename C::const_iterator*);
        template<typename C> static int  test(...);
    public:
        enum { value = sizeof(test<T>(0)) == sizeof(char) };
    };
    
    template<class F, class...Args>
    struct is_callable
    {
        template<class U> static auto test(U* p) -> decltype(std::ref(*p)(std::declval<Args>()...), void(), std::true_type());
        template<class U> static auto test(...) -> decltype(std::false_type());
        static constexpr bool value = decltype(test<F>(0))::value;
    };
}

// map

template <class T, class U, class F>
U map(const T &list, const F &func)
{
    using ItType = decltype(list.cbegin());
    U result;
    _FunctionalHelpersUtils::reserveSize(result, list.size());
    
    for(ItType it = list.cbegin(); it != list.cend(); ++it) {
        _FunctionalHelpersUtils::addItem(result, std::ref(func)(decltype(*it)(*it))); \
    }
    
    return result;
}

template <template <class...> class T, class U, class F, class ... Rest>
auto map(const T<U, Rest...> &list, const F &func)
    -> T<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type>
{
    using V = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type;
    return ::map<T<U>, T<V>, F>(list, func);
}

#define __FH_map_with_fully_specified_return_type(NAME, RET_TYPE) \
template <class T, class F> \
auto NAME(const T &list, const F &func) \
    -> RET_TYPE \
{ \
    using U = RET_TYPE; \
    return ::map<T, U, F>(list, func); \
}

#define __FH_map_with_specific_return_type(NAME, RET_TYPE) \
    __FH_map_with_fully_specified_return_type(NAME, \
        RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type>)

__FH_map_with_specific_return_type(listMap, std::list)
__FH_map_with_specific_return_type(vectorMap, std::vector)
__FH_map_with_specific_return_type(setMap, std::set)
__FH_map_with_fully_specified_return_type(stringMap, std::string)

// compr

template <class T, class U, class F1, class F2>
U compr(const T &list, const F1 &func, const F2 &predicate)
{
    using ItType = decltype(list.cbegin());
    U result;
    
    for(ItType it = list.cbegin(); it != list.cend(); ++it) {
        if (std::ref(predicate)(decltype(*it)(*it))) {
            _FunctionalHelpersUtils::addItem(result, std::ref(func)(decltype(*it)(*it)));
        }
    }
    
    return result;
}

template <template <class...> class T, class U, class F1, class F2>
auto compr(const T<U> &list, const F1 &func, const F2 &predicate)
-> T<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type>
{
    using V = typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type;
    return ::compr<T<U>, T<V>, F1, F2>(list, func, predicate);
}

#define __FH_compr_with_fully_specified_return_type(NAME, RET_TYPE) \
template <class T, class F1, class F2> \
auto NAME(const T &list, const F1 &func, const F2 &predicate) \
    -> RET_TYPE \
{ \
    using U = RET_TYPE; \
    return ::compr<T, U, F1, F2>(list, func, predicate); \
}

#define __FH_compr_with_specific_return_type(NAME, RET_TYPE) \
    __FH_compr_with_fully_specified_return_type(NAME, \
        RET_TYPE<typename std::decay<decltype(std::ref(func)(decltype(*list.begin())(*list.begin())))>::type>)

__FH_compr_with_specific_return_type(listCompr, std::list)
__FH_compr_with_specific_return_type(vectorCompr, std::vector)
__FH_compr_with_specific_return_type(setCompr, std::set)
__FH_compr_with_fully_specified_return_type(stringCompr, std::string)

// filter

template <class T, class U, class F>
U filter(const T &list, const F &predicate)
{
    using ItType = decltype(list.cbegin());
    U result;
    
    for(ItType it = list.cbegin(); it != list.cend(); ++it) {
        if (std::ref(predicate)(decltype(*it)(*it))) {
            _FunctionalHelpersUtils::addItem(result, *it);
        }
    }
    
    return result;
}

template <class T, class F>
T filter(const T &list, const F &predicate)
{
    return ::filter<T, T, F>(list, predicate);
}

#define __FH_filter_with_specific_return_type(NAME, RET_TYPE) \
template <class T, class F> \
auto NAME(const T &list, const F &func) \
    -> RET_TYPE<typename std::decay<decltype(*list.begin())>::type> \
{ \
    using U = RET_TYPE<typename std::decay<decltype(*list.begin())>::type>; \
    return ::filter<T, U, F>(list, func); \
}

__FH_filter_with_specific_return_type(listFilter, std::list)
__FH_filter_with_specific_return_type(vectorFilter, std::vector)
__FH_filter_with_specific_return_type(setFilter, std::set)

// all of

template <class T, class F>
bool allOf(T const &list, const F &f)
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
bool anyOf(T const &list, const F &f)
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

namespace _FunctionalHelpersUtils {
    template <class T, class F>
    auto extremumBase(const T &list, const F &comp)
        -> const typename std::decay<decltype(*list.begin())>::type *
    {
        using U = typename std::decay<decltype(*list.begin())>::type;
        using V = decltype(list.cbegin());
        const U *extremumValue = nullptr;
        
        for(V it = list.cbegin(); it != list.cend(); ++it) {
            if (!extremumValue || std::ref(comp)(decltype(*it)(*it), *extremumValue)) {
                extremumValue = &(*it);
            }
        }
    
        return extremumValue;
    }
}

template <class T, class F>
auto extremum(const T &list, const F &comp)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    const U *extremumValue = _FunctionalHelpersUtils::extremumBase(list, comp);
    
    if (extremumValue) {
        return *extremumValue;
    } else {
        return U();
    }
}

template <class T, class F, class U>
auto extremum(const T &list, const F &comp, const U &defaultVal)
    -> typename std::decay<decltype(*list.begin())>::type
{
    const U *extremumValue = _FunctionalHelpersUtils::extremumBase(list, comp);
    
    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// min(container)

template <class T>
auto min(const T &list)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    return extremum(list, [] (const U &a, const U &b) {return a < b; });
}

template <class T, class U>
auto min(const T &list, const U &defaultVal)
  -> typename std::enable_if<!_FunctionalHelpersUtils::is_callable<U, typename std::decay<decltype(*list.begin())>::type>::value, U>::type
{
    return extremum(list, [] (const U &a, const U &b) {return a < b; }, defaultVal);
}

// min(container, func)

namespace _FunctionalHelpersUtils {
    template <class T, class F>
    auto minBase(const T &list, const F &func)
      -> const typename std::decay<decltype(*list.begin())>::type *
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
    
        return extremumValue;
    }
}

template <class T, class F>
auto min(const T &list, const F &func)
  -> typename std::enable_if<_FunctionalHelpersUtils::is_callable<F, typename std::decay<decltype(*list.begin())>::type>::value,
                             typename std::decay<decltype(*list.begin())>::type>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    const U *extremumValue = _FunctionalHelpersUtils::minBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return U();
    }
}

template <class T, class F, class U>
U min(const T &list, const F &func, const U &defaultVal)
{
    const U *extremumValue = _FunctionalHelpersUtils::minBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// max(container)

template <class T>
auto max(const T &list)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    return extremum(list, [] (const U &a, const U &b) {return a > b; });
}

template <class T, class U>
auto max(const T &list, const U &defaultVal)
  -> typename std::enable_if<!_FunctionalHelpersUtils::is_callable<U, typename std::decay<decltype(*list.begin())>::type>::value, U>::type
{
    return extremum(list, [] (const U &a, const U &b) {return a > b; }, defaultVal);
}

// max(container, func)

namespace _FunctionalHelpersUtils {
    template <class T, class F>
    auto maxBase(const T &list, const F &func)
        -> const typename std::decay<decltype(*list.begin())>::type *
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
    
        return extremumValue;
    }
}

template <class T, class F>
auto max(const T &list, const F &func)
  -> typename std::enable_if<_FunctionalHelpersUtils::is_callable<F, typename std::decay<decltype(*list.begin())>::type>::value,
                             typename std::decay<decltype(*list.begin())>::type>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    const U *extremumValue = _FunctionalHelpersUtils::maxBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return U();
    }
}

template <class T, class F, class U>
U max(const T &list, const F &func, const U &defaultVal)
{
    const U *extremumValue = _FunctionalHelpersUtils::maxBase(list, func);

    if (extremumValue) {
        return *extremumValue;
    } else {
        return defaultVal;
    }
}

// reduce

template <class T, class F>
auto reduce(const T &list, const F &func)
    -> typename std::decay<decltype(*list.begin())>::type
{
    using U = typename std::decay<decltype(*list.begin())>::type;
    using V = decltype(list.cbegin());
    V it = list.cbegin();
    
    if (it == list.cend()) {
        return U();
    }
    
    U memo = *it;
    ++it;
    
    while(it != list.cend()) {
        memo = std::ref(func)(U(memo), decltype(*it)(*it));
        ++it;
    }

    return memo;
}

template <class T, class F, class U>
U reduce(const T &list, const F &func, U memo)
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
    return reduce(list, [] (const U &a, const U &b) { return a+b; });
}

// sorted

// Since std::sort requires a random access iterator, we have to define
// different versions of sorted for both std::list and std::forward_list, and
// any classes derived from them. This is accomplished by using std::enable_if
// when determining the return type.

template <class T>
auto sorted(const T &list)
    -> typename std::enable_if<!std::is_base_of<std::list<typename T::value_type>, T>::value &&
                               !std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    std::sort(result.begin(), result.end());
    return result;
}

template <class T, class F>
auto sorted(const T &list, const F &comp)
    -> typename std::enable_if<!std::is_base_of<std::list<typename T::value_type>, T>::value &&
                               !std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    std::sort(result.begin(), result.end(), std::ref(comp));
    return result;
}

template <class T>
auto sorted(const T &list)
    -> typename std::enable_if<std::is_base_of<std::list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort();
    return result;
}

template <class T, class F>
auto sorted(const T &list, const F &comp)
    -> typename std::enable_if<std::is_base_of<std::list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort(std::ref(comp));
    return result;
}

template <class T>
auto sorted(const T &list)
    -> typename std::enable_if<std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort();
    return result;
}

template <class T, class F>
auto sorted(const T &list, const F &comp)
    -> typename std::enable_if<std::is_base_of<std::forward_list<typename T::value_type>, T>::value, T>::type
{
    T result(list);
    result.sort(std::ref(comp));
    return result;
}

// contains

template <class T, class U>
bool contains(const T &container, const U &val)
{
    using ItType = decltype(container.cbegin());
    
    for(ItType it = container.cbegin(); it != container.cend(); ++it) {
        if ((*it) == val) {
            return true;
        }
    }
    
    return false;
}

template <class T, class U>
bool contains(const std::set<T, U> &container, const T &val)
{
    return container.find(val) != container.end();
}

// omit

template <class T, class U, class V>
typename std::enable_if<!_FunctionalHelpersUtils::has_const_iterator<V>::value, U>::type
omit(const T &container, const V &omitted)
{
    using ItType = decltype(container.cbegin());
    U result;
    
    for(ItType it = container.cbegin(); it != container.cend(); ++it) {
        if (!((*it) == omitted)) {
            _FunctionalHelpersUtils::addItem(result, *it);
        }
    }
    
    return result;
}

template <class T, class V>
typename std::enable_if<!_FunctionalHelpersUtils::has_const_iterator<V>::value, T>::type
omit(const T &container, const V &omitted)
{
    return ::omit<T, T, V>(container, omitted);
}


template <class T, class U, class V>
typename std::enable_if<_FunctionalHelpersUtils::has_const_iterator<V>::value, U>::type
omit(const T &container, const V &omitted)
{
    using ItType = decltype(container.cbegin());
    U result;
    
    for(ItType it = container.cbegin(); it != container.cend(); ++it) {
        if (!contains(omitted, *it)) {
            _FunctionalHelpersUtils::addItem(result, *it);
        }
    }
    
    return result;
}

template <class T, class V>
typename std::enable_if<_FunctionalHelpersUtils::has_const_iterator<V>::value, T>::type
omit(const T &container, const V &omitted)
{
    return ::omit<T, T, V>(container, omitted);
}

#define __FH_omit_with_specific_return_type(NAME, RET_TYPE) \
template <class T, class V> \
auto NAME(const T &container, const V &omitted) \
    -> RET_TYPE<typename std::decay<decltype(*container.begin())>::type> \
{ \
    using U = RET_TYPE<typename std::decay<decltype(*container.begin())>::type>; \
    return omit<T, U, V>(container, omitted); \
}

__FH_omit_with_specific_return_type(listOmit, std::list)
__FH_omit_with_specific_return_type(vectorOmit, std::vector)
__FH_omit_with_specific_return_type(setOmit, std::set)

// reversed

template <class T>
T reversed(const T &container)
{
    using ItType = decltype(container.crbegin());
    T result;
    
    for(ItType it = container.crbegin(); it != container.crend(); ++it) {
        _FunctionalHelpersUtils::addItem(result, *it);
    }
    
    return result;
}

// first

template <class T>
auto first(const T &container)
 -> typename std::decay<decltype(*container.cbegin())>::type
{
    using U = typename std::decay<decltype(*container.begin())>::type;
    
    if (container.size() == 0) {
        return U();
    } else {
        return *(container.cbegin());
    }
}

template <class T, class U>
auto first(const T &container, const U &defaultValue)
 -> typename std::decay<decltype(*container.cbegin())>::type
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return *(container.cbegin());
    }
}

// last

template <class T>
auto last(const T &container)
 -> typename std::decay<decltype(*container.crbegin())>::type
{
    using U = typename std::decay<decltype(*container.begin())>::type;
    
    if (container.size() == 0) {
        return U();
    } else {
        return *(container.crbegin());
    }
}

template <class T, class U>
auto last(const T &container, const U &defaultValue)
 -> typename std::decay<decltype(*container.crbegin())>::type
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return *(container.crbegin());
    }
}

// range

#define __FH_range_with_return_type(NAME, RET_TYPE) \
inline RET_TYPE<int> NAME(int start, int end, int inc=1) \
{ \
    RET_TYPE<int> result; \
    int sign = (inc < 0) ? -1 : 1; \
     \
    for(int i = start; (i*sign) < (end*sign); i += inc) { \
        _FunctionalHelpersUtils::addItem(result, i); \
    } \
     \
    return result; \
} \
 \
inline RET_TYPE<int> NAME(int end) \
{ \
    return NAME(0, end); \
}

__FH_range_with_return_type(listRange, std::list)
__FH_range_with_return_type(vectorRange, std::vector)
__FH_range_with_return_type(setRange, std::set)

// mapRange

#define __FH_mapRange_with_return_type(NAME, RET_TYPE) \
template <class F1> \
auto NAME(int start, int end, int inc, const F1 &func) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    using U = typename std::decay<decltype(std::ref(func)(0))>::type; \
    RET_TYPE<U> result; \
    int sign = (inc < 0) ? -1 : 1; \
     \
    for(int i = start; (i*sign) < (end*sign); i += inc) { \
        _FunctionalHelpersUtils::addItem(result, std::ref(func)(i)); \
    } \
     \
    return result; \
} \
 \
template <class F1, class F2> \
auto NAME(int start, int end, int inc, const F1 &func, const F2 &predicate) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    using U = typename std::decay<decltype(std::ref(func)(0))>::type; \
    RET_TYPE<U> result; \
    int sign = (inc < 0) ? -1 : 1; \
     \
    for(int i = start; (i*sign) < (end*sign); i += inc) { \
        if (std::ref(predicate)(i)) { \
            _FunctionalHelpersUtils::addItem(result, std::ref(func)(i)); \
        } \
    } \
     \
    return result; \
} \
 \
template <class F1> \
auto NAME(int start, int end, const F1 &func) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(start, end, 1, func); \
} \
 \
template <class F1, class F2> \
auto NAME(int start, int end, const F1 &func, const F2 &predicate) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(start, end, 1, func, predicate); \
} \
 \
template <class F1> \
auto NAME(int end, const F1 &func) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(0, end, 1, func); \
} \
 \
template <class F1, class F2> \
auto NAME(int end, const F1 &func, const F2 &predicate) \
-> RET_TYPE<typename std::decay<decltype(std::ref(func)(0))>::type> \
{ \
    return NAME(0, end, 1, func, predicate); \
}

__FH_mapRange_with_return_type(listMapRange, std::list)
__FH_mapRange_with_return_type(vectorMapRange, std::vector)
__FH_mapRange_with_return_type(setMapRange, std::set)

#endif // __FUNCTIONAL_HELPERS_H__
