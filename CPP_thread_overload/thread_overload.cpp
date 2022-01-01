#include <iostream>
#include <string>
#include <thread>
#include <functional>
 
class MyClass
{
public:
	void test1() {
		std::cout << "test1 method" << std::endl;
	}
	void test2() {
		std::cout << "test2" << std::endl;
	}
	void test2(int i) {
		std::cout << "test2 " << i << std::endl;
	}
	void test2(std::string str) {
		std::cout << "test2 " << str << std::endl;
	}
};
 
int main()
{
	MyClass obj;
	std::thread t(&MyClass::test1, &obj);
	t.join();
 
	using Func = void(MyClass::*)();
	Func f1 = &MyClass::test2;
	std::thread t1(std::bind(f1, obj));  //obj->test2()

	void(MyClass::*f2)(int) = &MyClass::test2;
	std::thread t2(std::bind(f2, obj, 1992));//obj->test2(int)
 
	std::thread t3([&obj]() { obj.test2(1993); });//obj->test2() --->lambda expression!
 
	std::thread t4(static_cast<void(MyClass::*)(std::string)>(&MyClass::test2),&obj,"AAA");
 
	t1.join();
	t2.join();
	t3.join();
	t4.join();
 
	system("pause"); //msvc
	return 0;
}