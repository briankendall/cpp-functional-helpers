#ifndef __QT_FUNCTIONAL_HELPERS_H__
#define __QT_FUNCTIONAL_HELPERS_H__

// For some reason it's necessary to include <vector> before some of Qt's
// headers (like <QString>), otherwise we'll get the error "too few arguments
// for class template 'vector'" when using versions of a function that take
// template-template arguments.
//
// NB: if you get that error, move your include of <qtFunctionalHelpers.h>
// above <vector>

#include <vector>
#include <QVector>
#include <QList>
#include <QSet>
#include <QLinkedList>

// These need to be declared before functionalHelpers.h is included so that the macros
// it declares will have access to them.
namespace FuncHelpUtils {
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

// sorted

// Adding an overload for convenient sorting of QStringLists
inline QStringList sorted(const QStringList &list, Qt::CaseSensitivity sensitivity=Qt::CaseSensitive)
{
    QStringList result = list;
    result.sort(sensitivity);
    return result;
}

// contains

template <class T>
bool contains(const QSet<T> &container, const T &val)
{
    return container.contains(val);
}

// reversed

// Special casing reversed() for Qt containers in order to achieve
// compatibility with Qt versions before 5.6 as they do not implement crbegin()
// or crend()

template <template <class> class Container, class T, template <class> class Iterator>
Container<T> reversed(const Container<T> &container)
{
    Container<T> result;
    Iterator<T> it(container);
    it.toBack();
    
    while(it.hasPrevious()) {
        FuncHelpUtils::addItem(result, it.previous());
    }
    
    return result;
}

template <class T>
QList<T> reversed(const QList<T> &container)
{
    return reversed<QList, T, QListIterator>(container);
}

template <class T>
QLinkedList<T> reversed(const QLinkedList<T> &container)
{
    return reversed<QLinkedList, T, QLinkedListIterator>(container);
}

template <class T>
QVector<T> reversed(const QVector<T> &container)
{
    return reversed<QVector, T, QVectorIterator>(container);
}

// last

// Special casing last() for Qt containers in order to achieve compatibility
// with Qt versions before 5.6 as they do not implement crbegin() or crend()

template <template <class> class T, class U>
U last(const T<U> &container)
{
    if (container.size() == 0) {
        return U();
    } else {
        return container.last();
    }
}

template <template <class> class T, class U>
U last(const T<U> &container, const U &defaultValue)
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return container.last();
    }
}

// range

__FH_range_with_return_type(QListRange, QList)
__FH_range_with_return_type(QVectorRange, QVector)
__FH_range_with_return_type(QSetRange, QSet)
__FH_range_with_return_type(QLinkedListRange, QLinkedList)

// mapRange

__FH_mapRange_with_return_type(QListMapRange, QList)
__FH_mapRange_with_return_type(QVectorMapRange, QVector)
__FH_mapRange_with_return_type(QSetMapRange, QSet)
__FH_mapRange_with_return_type(QLinkedListMapRange, QLinkedList)

// flatten

__FH_flatten_with_return_type(QListFlatten, QList)
__FH_flatten_with_return_type(QVectorFlatten, QVector)
__FH_flatten_with_return_type(QSetFlatten, QSet)
__FH_flatten_with_return_type(QLinkedListFlatten, QLinkedList)


#endif // __QT_FUNCTIONAL_HELPERS_H__

