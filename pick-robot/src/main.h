#ifndef MAIN_H
#define MAIN_H 1


//Avoid using this unless absolutely necessary
static inline void __dmb(void) {
	asm volatile ("dmb" ::: "memory");
}
void jsonInitialization();
void realTimeLoop();
void tick(long long int);
void reportStatus();

//Test functions
void testTargetGenerator();
#endif
