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

__FH_general_map(QListMap, QList)
__FH_general_map(QVectorMap, QVector)
__FH_general_map(QSetMap, QSet)
__FH_general_map(QLinkedListMap, QLinkedList)

// compr

__FH_general_compr(QListCompr, QList)
__FH_general_compr(QVectorCompr, QVector)
__FH_general_compr(QSetCompr, QSet)
__FH_general_compr(QLinkedListCompr, QLinkedList)

// filter

__FH_general_filter(QListFilter, QList, Q_FOREACH)
__FH_general_filter(QVectorFilter, QVector, Q_FOREACH)
__FH_general_filter(QSetFilter, QSet, Q_FOREACH)
__FH_general_filter(QLinkedListFilter, QLinkedList, Q_FOREACH)

#endif // __QT_FUNCTIONAL_HELPERS_H__
