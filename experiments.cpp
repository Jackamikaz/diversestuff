// experiments.cpp : définit le point d'entrée pour l'application console.
//

#include "stdafx.h"
#include <iostream>
#include <string>
#include <vector>
#include <type_traits>

using namespace std;

template<typename T>
class Test {
public:
	T value;
	void print() {
		cout << value << endl;
	}
};

template<typename T> struct is_vector : public false_type {};

template<typename T, typename A>
struct is_vector<vector<T, A>> : public true_type {};

template <typename T>
class CorrectPrint {
	T& toprint;

	void print(true_type) {
		for (auto &obj : toprint) {
			obj.print();
		}
	}
	void print(false_type) {
		toprint.print();
	}
public:
	CorrectPrint(T& val) : toprint(val) {}
	void print() {
		print(is_vector<T>{});
	}
};

#define CALL_PRINT(val) CorrectPrint<decltype(val)>(val).print()

int main()
{
	Test<int> i;
	i.value = 100;
	Test<string> s;
	s.value.assign("salut");

	vector<Test<double>> vec;
	vec.push_back(Test<double>());
	vec.push_back(Test<double>());
	vec.push_back(Test<double>());

	double v = 0.0;
	for (auto &o : vec) {
		v += 1.0;
		o.value = v;
	}

	CALL_PRINT(i);
	CALL_PRINT(s);
	CALL_PRINT(vec);

	system("PAUSE");

    return 0;
}

