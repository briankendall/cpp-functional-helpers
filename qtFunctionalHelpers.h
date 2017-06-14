#ifndef QT_FUNCTIONAL_HELPERS_H_GUARD
#define QT_FUNCTIONAL_HELPERS_H_GUARD

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
    
    inline void addItem(QStringList &container, const QString &item)
    {
        container.append(item);
    }
}

#include <functionalHelpers.h>

// map
template <class F,
          class = FuncHelpUtils::enable_if_t<std::is_same<FuncHelpUtils::func_container_result<QStringList, F>, QString>::value> >
QStringList map(const QStringList &container, const F &func)
{
    return map<QStringList, QStringList, F>(container, func);
}

// filter
template <class F>
QStringList filter(const QStringList &container, const F &pred)
{
    return filter<QStringList, QStringList>(container, pred);
}

// reject
template <class F>
QStringList reject(const QStringList &container, const F &pred)
{
    return reject<QStringList, QStringList>(container, pred);
}

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

#if QT_VERSION < 0x050600

template <class T>
auto last(const T &container)
 -> FuncHelpUtils::decay_t<decltype(container.last())> 
{
    using U = FuncHelpUtils::decay_t<decltype(container.last())>;
    if (container.size() == 0) {
        return U();
    } else {
        return container.last();
    }
}

template <class T,
          class U,
          class = FuncHelpUtils::enable_if_t<!FuncHelpUtils::has_const_reverse_iterator<T>::value> >
U last(const T &container, const U &defaultValue)
{
    if (container.size() == 0) {
        return defaultValue;
    } else {
        return container.last();
    }
}

#endif // QT_VERSION < 0x050600

#endif // QT_FUNCTIONAL_HELPERS_H_GUARD
