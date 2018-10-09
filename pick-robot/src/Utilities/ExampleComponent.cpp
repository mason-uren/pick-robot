#include "ExampleComponent.h"

#include <unistd.h>

ExampleComponent::ExampleComponent(int objNum) {
	objectNumber = objNum;
}

ExampleComponent::ExampleComponent() {
	objectNumber = 0;
}

ExampleComponent::~ExampleComponent() {
}

void ExampleComponent::step(long long int clockTicks) {

	if (clockTicks % 3 == objectNumber) {
//		printf("%d: Clock is %lld\n", objectNumber, clockTicks);
		usleep((unsigned int) 900);
	}
}

void ExampleComponent::reportStatus(void *ptr) {

}

void ExampleComponent::emergencyStop() {
	//Do nothing
}
