#ifndef __QT_FUNCTIONAL_HELPERS_H__
#define __QT_FUNCTIONAL_HELPERS_H__

#include <functionalHelpers.h>
#include <QVector>
#include <QList>
#include <QSet>
#include <QLinkedList>

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

// map

__FH_general_map(QListMap, QList, Q_FOREACH)
__FH_general_map(QVectorMap, QVector, Q_FOREACH)
__FH_general_map(QSetMap, QSet, Q_FOREACH)
__FH_general_map(QLinkedListMap, QLinkedList, Q_FOREACH)

#endif // __QT_FUNCTIONAL_HELPERS_H__
