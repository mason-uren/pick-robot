/*
 * SharedMemory.cpp
 *
 *  Created on: Jul 16, 2018
 *      Author: michaelkaiser
 */

#include "SharedMemory.h"
#include "SharedMemory.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SharedMemory::SharedMemory() {
	/* Create shared memory object */

	robot_in_md = shm_open("robot_in", O_RDWR, 0666);
	pg_size = sysconf(_SC_PAGE_SIZE);

	/* Map one page */
	robot_in_addr = mmap(0, pg_size, PROT_WRITE | PROT_READ, MAP_SHARED, robot_in_md, 0);
	if (mlock(robot_in_addr, pg_size) != 0) {
		perror("mlock failure");
		exit(1);
	}

	robot_out_md = shm_open("robot_out", O_RDWR, 0666);
	/* Map one page */
	robot_out_addr = mmap(0, pg_size, PROT_WRITE | PROT_READ, MAP_SHARED, robot_out_md, 0);
	if (mlock(robot_out_addr, pg_size) != 0) {
		perror("mlock failure");
		exit(1);
	}
}

SharedMemory::~SharedMemory() {
	munmap(robot_in_addr, pg_size); /* Unmap the page */
	close(robot_in_md); /*   Close file   */
	shm_unlink("robot_in"); /* Unlink shared-memory object */
	munmap(robot_out_addr, pg_size); /* Unmap the page */
	close(robot_out_md); /*   Close file   */
	shm_unlink("robot_out"); /* Unlink shared-memory object */
}

bool SharedMemory::writeRobotIn(ROBOT_IN *block) {
	memcpy(robot_in_addr, block, sizeof(ROBOT_IN));
	return true;
}

bool SharedMemory::readRobotOut(ROBOT_OUT *status) {
	ROBOT_OUT temp;
	memcpy(&temp, robot_out_addr, sizeof(ROBOT_OUT));
	if (temp.block_number != status->block_number) {
		memcpy(status, &temp, sizeof(ROBOT_OUT));
		return true;
	}
	return false;
}

