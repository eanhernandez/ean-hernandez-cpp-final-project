#include<iostream>
#include <boost/type_traits.hpp>
#include <vector>
using namespace std;

template<class T>
void f(T i, vector<int> v)
{
	for(i=0; i<v.size();i++	)
	{
		cout << v[i] << endl;
	}
}

int main()
{
	vector<int> v;
	v.push_back(1);
	v.push_back(2);
	v.push_back(3);
	v.push_back(4);
	f(v.size(),v);

	system("pause");
	return 0;
}