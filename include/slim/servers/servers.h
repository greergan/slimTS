#ifndef __SLIM__SERVERS__H
#define __SLIM__SERVERS__H
namespace slim::servers {
	static std::vector<std::future<void>> server_promises;
	void initialize();
	void start();
	void prometheus();
	void typescript();
}
#endif