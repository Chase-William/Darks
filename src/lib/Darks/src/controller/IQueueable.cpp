#include "IQueueable.h"

namespace Darks::Controller {
	QueueSyncInfo::QueueSyncInfo(
		std::queue<IQueueable*>* work_queue,
		std::mutex& work_queue_mutex,
		std::condition_variable& queue_var
	) :
		work_queue_(work_queue),
		work_queue_mutex_(work_queue_mutex),
		queue_var_(queue_var)
	{ }
}