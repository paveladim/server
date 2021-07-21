#include "client.h"

int main() {
	client test;
	std::thread t(&client::handler, test);
	test.sender();
	t.join();
	return 0;
}