#ifndef HISTORY_FACTORY_H
#define HISTORY_FACTORY_H

#include "definitions.h"
#include "history.h"
#include "history_in_file.h"
#include "history_on_queue.h"

class History_factory
{
public:
	History* create_history(const bool& choice) {
		if (choice) return new History_on_queue(10);
		else return new History_in_file;
	}
};

#endif
