#include <qDebug>
#include <qtFunctionalHelpers.h>

// A quick set of tests for functionHelpers.h and qtFunctionalHelpers.h
// Mainly used to check for compile errors. Also checks for correct results at runtime, but
// admittedly not as thoroughly as it could.

using namespace std;

#define TEST(expr, val) \
	if ((expr) != (val)) { \
		qDebug() << "Failure! line" << __LINE__ << #expr; \
	} else { \
		passedTests += 1; \
	} \
	totalTests += 1;

int timesTwo(int x) { return x*2; }
int timesX(int x, int y) { return x*y; }
bool isEven(int x) { return (x%2) == 0; }
bool isMultiple(int x, int y) { return (x%y) == 0; }

class Foo {
public:
	Foo(int a) : value(a) {};
	int fooTimesTwo() const { return value*2; };
	bool isEven() const { return ::isEven(value); };
	bool operator==(const Foo &other) const { return value == other.value; }
	int value;
};

const std::list<int> listNumbers = {1,2,3,4,5};
const std::vector<int> vectorNumbers = {1,2,3,4,5};
const std::set<int> setNumbers = {1,2,3,4,5};
const QList<int> QListNumbers = {1,2,3,4,5};
const QVector<int> QVectorNumbers = {1,2,3,4,5};
const QSet<int> QSetNumbers = {1,2,3,4,5};
const QLinkedList<int> QLinkedListNumbers = {1,2,3,4,5};

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
	const std::list<int> listExpected = {2,4,6,8,10};
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

int main()
{
	testMap();
	testFilter();
	
	qDebug() << "Finished!" << passedTests << "/" << totalTests << "passed";
	return 0;
}
