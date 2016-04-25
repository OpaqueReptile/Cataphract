#pragma once
#include<iostream>
#define ASIO_STANDALONE 
#define ASIO_HAS_STD_ADDRESSOF
#define ASIO_HAS_STD_ARRAY
#define ASIO_HAS_CSTDINT
#define ASIO_HAS_STD_SHARED_PTR
#define ASIO_HAS_STD_TYPE_TRAITS

using namespace std;
class Server
{
	int a, b, c;
public:
	void read() { cin >> a >> b >> c; }
	void show() { cout << "Hello World"; }
};

