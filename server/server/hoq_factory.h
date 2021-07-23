#ifndef HOQ_FACTORY_H
#define HOQ_FACTORY_H

#include "history_factory.h"
#include "history_on_queue.h"

class HOQ_factory : public History_factory
{
public:
	History* create_history() override { return new History_on_queue(10); }
};

#endif

