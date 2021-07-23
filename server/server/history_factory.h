#ifndef HISTORY_FACTORY_H
#define HISTORY_FACTORY_H

#include "history.h"

class History_factory
{
public:
	virtual History* create_history() = 0;
};

#endif
