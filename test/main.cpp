#include <qDebug>
#include <qtFunctionalHelpers.h>

// A quick set of tests for functionHelpers.h and qtFunctionalHelpers.h
// Mainly used to check for compile errors. Also checks for correct results at runtime, but
// admittedly not as thoroughly as it could.

using namespace std;

#define TEST(expr, val) \
	if (!((expr) == (val))) { \
		qDebug() << "Failure! line" << __LINE__ << #expr; \
	} else { \
		passedTests += 1; \
	} \
	totalTests += 1;

int timesTwo(int x) { return x*2; }
int timesX(int x, int y) { return x*y; }
bool isEven(int x) { return (x%2) == 0; }
bool isMultiple(int x, int y) { return (x%y) == 0; }
bool isLessThan(int x, int y) { return x < y; }

class Foo {
public:
	Foo(int a=0) : value(a) {};
	int getValue() const { return value; };
	int fooTimesTwo() const { return value*2; };
	Foo fooTimesFoo(const Foo &other) const { return Foo(value*other.value); };
	bool isEven() const { return ::isEven(value); };
	bool isLessThan(const Foo &other) const { return value < other.value; }
	bool operator==(const Foo &other) const { return value == other.value; }
	bool operator<(const Foo &other) const { return isLessThan(other); }
	bool operator>(const Foo &other) const { return value > other.value; }
	Foo operator+(const Foo &other) const { return Foo(value + other.value); }
	int value;
};

const std::list<int> listNumbers = {1,2,3,4,5};
const std::vector<int> vectorNumbers = {1,2,3,4,5};
const std::set<int> setNumbers = {1,2,3,4,5};
const QList<int> QListNumbers = {1,2,3,4,5};
const QVector<int> QVectorNumbers = {1,2,3,4,5};
const QSet<int> QSetNumbers = {1,2,3,4,5};
const QLinkedList<int> QLinkedListNumbers = {1,2,3,4,5};

const std::list<int> listEvenNumbers = {2,4,6,8,10};
const std::list<int> listOddNumbers = {1,3,5,7,9};

Foo fooA(1), fooB(2), fooC(3), fooD(4), fooE(5);
const std::list<Foo> listFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const std::list<Foo *> listFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const std::vector<Foo> vectorFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const std::vector<Foo *> vectorFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const QList<Foo> QListFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QList<Foo *> QListFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};
const QVector<Foo> QVectorFoos = {Foo(1), Foo(2), Foo(3), Foo(4), Foo(5)};
const QVector<Foo *> QVectorFooPtrs = {&fooA, &fooB, &fooC, &fooD, &fooE};

int passedTests = 0;
int totalTests = 0;

void testMap()
{
	const std::list<int> listExpected = listEvenNumbers;
	const std::vector<int> vectorExpected = {2,4,6,8,10};
	const std::set<int> setExpected = {2,4,6,8,10};
	const QList<int> QListExpected = {2,4,6,8,10};
	const QVector<int> QVectorExpected = {2,4,6,8,10};
	const QSet<int> QSetExpected = {2,4,6,8,10};
	const QLinkedList<int> QLinkedListExpected = {2,4,6,8,10};
	
	TEST(listMap(listNumbers, [] (int x) { return x*2; }), listExpected);
	TEST(listMap(listNumbers, &timesTwo), listExpected);
	TEST(listMap(listNumbers, std::bind(timesX, 2, std::placeholders::_1)), listExpected);
	TEST(listMap(listFoos, &Foo::fooTimesTwo), listExpected);
	TEST(listMap(listFooPtrs, &Foo::fooTimesTwo), listExpected);
	
	TEST(vectorMap(vectorNumbers, [] (int x) { return x*2; }), vectorExpected);
	TEST(setMap(setNumbers, [] (int x) { return x*2; }), setExpected);
	TEST(QListMap(QListNumbers, [] (int x) { return x*2; }), QListExpected);
	TEST(QVectorMap(QVectorNumbers, [] (int x) { return x*2; }), QVectorExpected);
	TEST(QSetMap(QSetNumbers, [] (int x) { return x*2; }), QSetExpected);
	TEST(QLinkedListMap(QLinkedListNumbers, [] (int x) { return x*2; }), QLinkedListExpected);
}

void testCompr()
{
	const std::list<int> listExpected = {4,8};
	const std::vector<int> vectorExpected = {4,8};
	const std::set<int> setExpected = {4,8};
	const QList<int> QListExpected = {4,8};
	const QVector<int> QVectorExpected = {4,8};
	const QSet<int> QSetExpected = {4,8};
	const QLinkedList<int> QLinkedListExpected = {4,8};
	
	TEST(listCompr(listNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), listExpected);
	TEST(listCompr(listNumbers, &timesTwo, &isEven), listExpected);
	TEST(listCompr(listNumbers, std::bind(timesX, 2, std::placeholders::_1),
	               std::bind(isMultiple, std::placeholders::_1, 2)),
	     listExpected);
	
	TEST(listCompr(listFoos, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
	TEST(listCompr(listFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), listExpected);
	TEST(listCompr(listFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), listExpected);
	TEST(listCompr(listFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), listExpected);
	TEST(listCompr(listFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), listExpected);
	TEST(listCompr(listFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), listExpected);
	TEST(QListCompr(QListFoos, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
	TEST(QListCompr(QListFoos, &Foo::fooTimesTwo, [] (const Foo &a) { return (a.value%2) == 0; }), QListExpected);
	TEST(QListCompr(QListFoos, [] (const Foo &a) { return a.fooTimesTwo(); }, &Foo::isEven), QListExpected);
	TEST(QListCompr(QListFooPtrs, &Foo::fooTimesTwo, &Foo::isEven), QListExpected);
	TEST(QListCompr(QListFooPtrs, &Foo::fooTimesTwo, [] (const Foo *a) { return (a->value%2) == 0; }), QListExpected);
	TEST(QListCompr(QListFooPtrs, [] (const Foo *a) { return a->fooTimesTwo(); }, &Foo::isEven), QListExpected);
	
	TEST(vectorCompr(vectorNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), vectorExpected);
	TEST(setCompr(setNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), setExpected);
	TEST(QListCompr(QListNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QListExpected);
	TEST(QVectorCompr(QVectorNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QVectorExpected);
	TEST(QSetCompr(QSetNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}), QSetExpected);
	TEST(QLinkedListCompr(QLinkedListNumbers, [] (int x) { return x*2; }, [] (int x) { return (x%2) == 0;}),
	     QLinkedListExpected);
}

void testFilter()
{
	const std::list<int> listExpected = {2,4};
	const std::vector<int> vectorExpected = {2,4};
	const std::set<int> setExpected = {2,4};
	const QList<int> QListExpected = {2,4};
	const QVector<int> QVectorExpected = {2,4};
	const QSet<int> QSetExpected = {2,4};
	const QLinkedList<int> QLinkedListExpected = {2,4};
	
	TEST(listFilter(listNumbers, [] (int x) { return (x%2) == 0; }), listExpected);
	TEST(listFilter(listNumbers, &isEven), listExpected);
	TEST(listFilter(listNumbers, std::bind(isMultiple, std::placeholders::_1, 2)), listExpected);
	TEST(listFilter(listFoos, &Foo::isEven), list<Foo>({fooB, fooD}));
	TEST(listFilter(listFooPtrs, &Foo::isEven), list<Foo *>({&fooB, &fooD}));
	
	TEST(vectorFilter(vectorNumbers, [] (int x) { return (x%2) == 0; }), vectorExpected);
	TEST(setFilter(setNumbers, [] (int x) { return (x%2) == 0; }), setExpected);
	TEST(QListFilter(QListNumbers, [] (int x) { return (x%2) == 0; }), QListExpected);
	TEST(QVectorFilter(QVectorNumbers, [] (int x) { return (x%2) == 0; }), QVectorExpected);
	TEST(QSetFilter(QSetNumbers, [] (int x) { return (x%2) == 0; }), QSetExpected);
	TEST(QLinkedListFilter(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), QLinkedListExpected);
}

void testAllOf()
{
	TEST(allOf(listNumbers, [] (int x) { return (x%2) == 0; }), false);
	TEST(allOf(listNumbers, &isEven), false);
	TEST(allOf(listNumbers, std::bind(isMultiple, std::placeholders::_1, 2)), false);
	TEST(allOf(listFoos, &Foo::isEven), false);
	TEST(allOf(listFooPtrs, &Foo::isEven), false);
	
	TEST(allOf(listEvenNumbers, [] (int x) { return (x%2) == 0; }), true);
	
	TEST(allOf(vectorNumbers, [] (int x) { return (x%2) == 0; }), false);
	TEST(allOf(setNumbers, [] (int x) { return (x%2) == 0; }), false);
	TEST(allOf(QListNumbers, [] (int x) { return (x%2) == 0; }), false);
	TEST(allOf(QVectorNumbers, [] (int x) { return (x%2) == 0; }), false);
	TEST(allOf(QSetNumbers, [] (int x) { return (x%2) == 0; }), false);
	TEST(allOf(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), false);
}

void testAnyOf()
{
	TEST(anyOf(listNumbers, [] (int x) { return (x%2) == 0; }), true);
	TEST(anyOf(listNumbers, &isEven), true);
	TEST(anyOf(listNumbers, std::bind(isMultiple, std::placeholders::_1, 2)), true);
	TEST(anyOf(listFoos, &Foo::isEven), true);
	TEST(anyOf(listFooPtrs, &Foo::isEven), true);
	
	TEST(anyOf(listOddNumbers, [] (int x) { return (x%2) == 0; }), false);
	
	TEST(anyOf(vectorNumbers, [] (int x) { return (x%2) == 0; }), true);
	TEST(anyOf(setNumbers, [] (int x) { return (x%2) == 0; }), true);
	TEST(anyOf(QListNumbers, [] (int x) { return (x%2) == 0; }), true);
	TEST(anyOf(QVectorNumbers, [] (int x) { return (x%2) == 0; }), true);
	TEST(anyOf(QSetNumbers, [] (int x) { return (x%2) == 0; }), true);
	TEST(anyOf(QLinkedListNumbers, [] (int x) { return (x%2) == 0; }), true);
}

void testExtremum()
{
	TEST(extremum(listNumbers, [] (int x, int y) { return x < y; }), 1);
	TEST(extremum(listNumbers, &isLessThan), 1);
	TEST(extremum(listFoos, &Foo::isLessThan), fooA);
	TEST(extremum(listFooPtrs, &Foo::isLessThan), &fooA);
	
	TEST(extremum(vectorNumbers, [] (int x, int y) { return x < y; }), 1);
	TEST(extremum(setNumbers, [] (int x, int y) { return x < y; }), 1);
	TEST(extremum(QListNumbers, [] (int x, int y) { return x < y; }), 1);
	TEST(extremum(QVectorNumbers, [] (int x, int y) { return x < y; }), 1);
	TEST(extremum(QSetNumbers, [] (int x, int y) { return x < y; }), 1);
	TEST(extremum(QLinkedListNumbers, [] (int x, int y) { return x < y; }), 1);
}

void testMin()
{
	TEST(min(listNumbers), 1);
	TEST(min(listFoos), fooA);
	TEST(min(vectorNumbers), 1);
	TEST(min(setNumbers), 1);
	TEST(min(QListNumbers), 1);
	TEST(min(QVectorNumbers), 1);
	TEST(min(QSetNumbers), 1);
	TEST(min(QLinkedListNumbers), 1);
	
	TEST(min(listFoos, [] (const Foo &a) { return a.value; }), fooA);
	TEST(min(listFoos, &Foo::getValue), fooA);
	TEST(min(listFooPtrs, &Foo::getValue), &fooA);
}

void testMax()
{
	TEST(max(listNumbers), 5);
	TEST(max(listFoos), fooE);
	TEST(max(vectorNumbers), 5);
	TEST(max(setNumbers), 5);
	TEST(max(QListNumbers), 5);
	TEST(max(QVectorNumbers), 5);
	TEST(max(QSetNumbers), 5);
	TEST(max(QLinkedListNumbers), 5);
	
	TEST(max(listFoos, [] (const Foo &a) { return a.value; }), fooE);
	TEST(max(listFoos, &Foo::getValue), fooE);
	TEST(max(listFooPtrs, &Foo::getValue), &fooE);
}

void testReduce()
{
	TEST(reduce(listNumbers, [] (int x, int y) { return x*y; }, 1), 120);
	TEST(reduce(listNumbers, &timesX, 1), 120);
	TEST(reduce(listFoos, &Foo::fooTimesFoo, Foo(1)), Foo(120));
	
	TEST(reduce(vectorNumbers, [] (int x, int y) { return x*y; }, 1), 120);
	TEST(reduce(setNumbers, [] (int x, int y) { return x*y; }, 1), 120);
	TEST(reduce(QListNumbers, [] (int x, int y) { return x*y; }, 1), 120);
	TEST(reduce(QVectorNumbers, [] (int x, int y) { return x*y; }, 1), 120);
	TEST(reduce(QSetNumbers, [] (int x, int y) { return x*y; }, 1), 120);
	TEST(reduce(QLinkedListNumbers, [] (int x, int y) { return x*y; }, 1), 120);
}

void testSum()
{
	TEST(sum(listNumbers, 0), 15);
	TEST(sum(listNumbers, 100), 115);
	TEST(sum(listNumbers), 15);
	TEST(sum(vectorNumbers), 15);
	TEST(sum(setNumbers), 15);
	TEST(sum(QListNumbers), 15);
	TEST(sum(QVectorNumbers), 15);
	TEST(sum(QSetNumbers), 15);
	TEST(sum(QLinkedListNumbers), 15);
	TEST(sum(listFoos), Foo(15));
	TEST(sum(listFoos, Foo(100)), Foo(115));
	
	TEST(sum(list<long long>({1,2,3,4,5})), 15);
	TEST(sum(list<float>({1.,2.,3.,4.,5.})), 15.0);
	TEST(sum(list<unsigned char>({'\1','\2','\3','\4','\5'})), (unsigned char)15);
}

int main()
{
	testMap();
	testFilter();
	testAllOf();
	testAnyOf();
	testExtremum();
	testMin();
	testMax();
	testReduce();
	testSum();
	testCompr();
	
	qDebug() << "Finished!" << passedTests << "/" << totalTests << "passed";
	return 0;
}
