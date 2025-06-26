#include <future>
#include <vector>
#include <slim/servers/launcher.h>
namespace slim::servers::start {
	static std::vector<std::future<void>> server_promises;
	void initialize();
	void less();
	void prometheus();
	void typescript();
}

void slim::servers::start::typescript() {
	server_promises.push_back(
		slim::service::launcher::launch(

		);
	)
}