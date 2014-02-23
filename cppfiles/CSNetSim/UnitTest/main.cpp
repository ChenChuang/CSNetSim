#include <stdlib.h>
#include <stdio.h>
#include "../sorted_list.h"

#include <UnitTest++.h>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// To add a test, simply put the following code in the a .cpp file of your choice:
//
// =================================
// Simple Test
// =================================
//
//  TEST(YourTestName)
//  {
//  }
//
// The TEST macro contains enough machinery to turn this slightly odd-looking syntax into legal C++, and automatically register the test in a global list. 
// This test list forms the basis of what is executed by RunAllTests().
//
// If you want to re-use a set of test data for more than one test, or provide setup/teardown for tests, 
// you can use the TEST_FIXTURE macro instead. The macro requires that you pass it a class name that it will instantiate, so any setup and teardown code should be in its constructor and destructor.
//
//  struct SomeFixture
//  {
//    SomeFixture() { /* some setup */ }
//    ~SomeFixture() { /* some teardown */ }
//
//    int testData;
//  };
// 
//  TEST_FIXTURE(SomeFixture, YourTestName)
//  {
//    int temp = testData;
//  }
//
// =================================
// Test Suites
// =================================
// 
// Tests can be grouped into suites, using the SUITE macro. A suite serves as a namespace for test names, so that the same test name can be used in two difference contexts.
//
//  SUITE(YourSuiteName)
//  {
//    TEST(YourTestName)
//    {
//    }
//
//    TEST(YourOtherTestName)
//    {
//    }
//  }
//
// This will place the tests into a C++ namespace called YourSuiteName, and make the suite name available to UnitTest++. 
// RunAllTests() can be called for a specific suite name, so you can use this to build named groups of tests to be run together.
// Note how members of the fixture are used as if they are a part of the test, since the macro-generated test class derives from the provided fixture class.
//
//
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// run all tests
int main(int argc, char **argv)
{
	int ret = UnitTest::RunAllTests();
	getchar();
	return ret;
}

class A
{
public:
	A(int n) : n(n) {};
	~A() {};
	bool operator <(const A& a){
		return this->n < a.n;
	}
	bool operator <=(const A& a){
		return this->n <= a.n;
	}
	bool operator ==(const A& a){
		return this->n == a.n;
	}
	bool operator >=(const A& a){
		return this->n >= a.n;
	}
	bool operator >(const A& a){
		return this->n > a.n;
	}
	bool operator !=(const A& a){
		return this->n != a.n;
	}
private:
	int n;
};

struct SortedListFixture
{
	SortedListFixture()
	{
		this->_list = new SortedList<A>();
		SortedList<A>* list = _list;
		A* a = new A(1);
		A* b = new A(2);
		A* c = new A(3);
		list->add(b);
		list->add(a);
		list->add(c);
	}
	~SortedListFixture()
	{
		delete this->_list;
	}
	SortedList<A>* _list;
};

TEST_FIXTURE(SortedListFixture, TestSortedList)
{
	SortedList<A>* list = _list;
	list->seek(-1);
	CHECK(1 == 1);
}
