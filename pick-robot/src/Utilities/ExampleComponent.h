#ifndef SRC_UTILITIES_EXAMPLECOMPONENT_H_
#define SRC_UTILITIES_EXAMPLECOMPONENT_H_

#include "ComponentInterface.h"

class ExampleComponent: public ComponentInterface {
private:
	int objectNumber;
public:
	ExampleComponent();
	ExampleComponent(int objectNumber);
	virtual ~ExampleComponent();
	void step(long long int clockTicks);
	void reportStatus(void *);
	void emergencyStop();
};

#endif
