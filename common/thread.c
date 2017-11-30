#include "thread.h"
#include <errno.h>

thread_t _thread_create(void *(*routine)(void *), void *arg) {
	pthread_t thread;
	int ret = pthread_create(&thread, NULL, routine, arg);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
	return thread;
}

static void signal_dummy_action(int signal) {
	signal++;
}

void _thread_signal_set(int signal, void (*routine)(int)) {
	struct sigaction sa;
	sa.sa_flags = SA_SIGINFO;
	sa.sa_handler = routine != NULL ? routine : signal_dummy_action;

	int ret = sigaction(signal, &sa, NULL);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}

void thread_kill(thread_t thread) {
	//pthread_kill(thread, SIGTERM);
	pthread_join(thread, NULL);
}

int thread_join(thread_t thread) {
	int *resultado;
	pthread_join(thread, &resultado);
	return resultado == NULL ? 0 : *resultado;
}

mutex_t thread_mutex() {
	mutex_t mutex;
	int ret = pthread_mutex_init(&mutex, NULL);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
	return mutex;
}

void thread_mutex_lock(mutex_t *mutex) {
	int ret = pthread_mutex_lock(mutex);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}

void thread_mutex_unlock(mutex_t *mutex) {
	int ret = pthread_mutex_unlock(mutex);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}

void thread_mutex_destroy(mutex_t *mutex) {
	int ret = pthread_mutex_destroy(mutex);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}

sem_t thread_sem(unsigned value) {
	sem_t sem;
	sem_init(&sem, 0, value);
	return sem;
}

void thread_sem_wait(sem_t *sem) {
	int ret = sem_wait(sem);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}

void thread_sem_signal(sem_t *sem) {
	int ret = sem_post(sem);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}

void thread_sem_destroy(sem_t *sem) {
	int ret = sem_destroy(sem);
	if(ret != 0) {
		log_msg_error(strerror(ret));
		exit(EXIT_FAILURE);
	}
}
