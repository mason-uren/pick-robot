/*
 * SharedMemory.cpp
 *
 *  Created on: Jul 16, 2018
 *      Author: michaelkaiser
 */

#include "SharedMemory.h"
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

SharedMemory::SharedMemory() {
	pg_size = sysconf(_SC_PAGE_SIZE);
	/* Create shared memory object */
	robot_in_md = shm_open("robot_in", O_CREAT | O_RDWR, 0666);


	if ((ftruncate(robot_in_md, pg_size)) == -1) { /* Set the size */
		perror("ftruncate failure");
		exit(1);
	}
	/* Map one page */
	robot_in_addr = mmap(0, pg_size, PROT_WRITE | PROT_READ, MAP_SHARED, robot_in_md, 0);
	memset(robot_in_addr, 0, pg_size);

	if (mlock(robot_in_addr, pg_size) != 0) {
		perror("mlock failure");
		exit(1);
	}

	robot_out_md = shm_open("robot_out", O_CREAT | O_RDWR, 0666);

	if ((ftruncate(robot_out_md, pg_size)) == -1) { /* Set the size */
		perror("ftruncate failure");
		exit(1);
	}
	/* Map one page */
	robot_out_addr = mmap(0, pg_size, PROT_WRITE | PROT_READ, MAP_SHARED, robot_out_md, 0);
	memset(robot_out_addr, 0, pg_size);

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

bool SharedMemory::readRobotIn(ROBOT_IN *block) {
	ROBOT_IN temp;
	memcpy(&temp, robot_in_addr, sizeof(ROBOT_IN));
	if (temp.block_number != block->block_number) {
		memcpy(block, &temp, sizeof(ROBOT_IN));
		return true;
	}
	return false;
}

bool SharedMemory::writeRobotOut(ROBOT_OUT *status) {
	memcpy(robot_out_addr, status, sizeof(ROBOT_OUT));
	return true;
}

