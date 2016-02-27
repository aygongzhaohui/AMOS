#include <iostream>

class A
{
public:
	static void Test()
	{
		std::cout << "A::Test callled" << std::endl;
	}
};


class B : public A
{
};


int main()
{
	B::Test();
	return 0;
}
