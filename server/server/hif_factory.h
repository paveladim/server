#ifndef HIF_FACTORY_H
#define HIF_FACTORY_H

#include "history_factory.h"
#include "history_in_file.h"

class HIF_factory : public History_factory
{
public:
	History* create_history() override { return new History_in_file(); }
};

#endif
