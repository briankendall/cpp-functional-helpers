#ifndef __QT_FUNCTIONAL_HELPERS_H__
#define __QT_FUNCTIONAL_HELPERS_H__

#include <QVector>
#include <QList>
#include <QSet>
#include <QLinkedList>

// These need to be declared before functionalHelpers.h is included so that the macros
// it declares will have access to them.
namespace _FunctionalHelpersUtils {
    template<class U>
    inline void reserveSize(QVector<U> &container, int size)
    {
        container.reserve(size);
    }
    
    template<class U>
    inline void addItem(QSet<U> &container, const U &item)
    {
        container.insert(item);
    }
}

#include <functionalHelpers.h>

// map

__FH_map_with_specific_return_type(QListMap, QList)
__FH_map_with_specific_return_type(QVectorMap, QVector)
__FH_map_with_specific_return_type(QSetMap, QSet)
__FH_map_with_specific_return_type(QLinkedListMap, QLinkedList)

// compr

__FH_compr_with_specific_return_type(QListCompr, QList)
__FH_compr_with_specific_return_type(QVectorCompr, QVector)
__FH_compr_with_specific_return_type(QSetCompr, QSet)
__FH_compr_with_specific_return_type(QLinkedListCompr, QLinkedList)

// filter

__FH_filter_with_specific_return_type(QListFilter, QList)
__FH_filter_with_specific_return_type(QVectorFilter, QVector)
__FH_filter_with_specific_return_type(QSetFilter, QSet)
__FH_filter_with_specific_return_type(QLinkedListFilter, QLinkedList)

// contains

template <class T>
bool contains(const QSet<T> &container, const T &val)
{
    return container.contains(val);
}

// omit

__FH_omit_with_specific_return_type(QListOmit, QList)
__FH_omit_with_specific_return_type(QVectorOmit, QVector)
__FH_omit_with_specific_return_type(QSetOmit, QSet)
__FH_omit_with_specific_return_type(QLinkedListOmit, QLinkedList)

#endif // __QT_FUNCTIONAL_HELPERS_H__
