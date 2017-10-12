#ifndef COMMON_THREAD_H_
#define COMMON_THREAD_H_

#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>

typedef pthread_t thread_t;
typedef pthread_mutex_t mutex_t;

/**
 * Crea un hilo de usuario usando la librería de hilos POSIX.
 * @param routine Rutina que ejecutará el hilo.
 * @param arg (Opcional) Argumento pasado a la rutina.
 * @return Identificador del hilo creado.
 */
thread_t _thread_create(void *(*routine)(void *), void *arg);
#define thread_create(...) __thread_create(__VA_ARGS__, 0, 0)
#define __thread_create(r, a, ...) _thread_create((void *(*)(void *))(r), (void *)(a))

/**
 * Establece la rutina que se ejecutará al recibir determinada señal.
 * @param signal Descriptor de la señal.
 * @param routine (Opcional) Rutina que se ejecutará.
 */
void _thread_signal_set(int signal, void (*routine)(int));
#define thread_signal_set(...) __thread_signal_set(__VA_ARGS__, 0, 0)
#define __thread_signal_set(s, r, ...) _thread_signal_set(s, (void (*)(int))(r))

/**
 * Manda una señal al hilo para cancelarlo y espera a que el hilo termine.
 * @param thread Hilo que se va a cancelar.
 */
void thread_kill(thread_t thread);

/**
 * Espera a que el hilo termine
 */
void thread_join(thread_t thread);

/**
 * Crea un semáforo de exclusión mutua (mutex).
 * @return Semáforo mutex.
 */
mutex_t thread_mutex(void);

/**
 * Bloquea un semáforo de exclusión mutua.
 * @param mutex Semáforo a bloquear.
 */
void thread_mutex_lock(mutex_t *mutex);

/**
 * Desbloquea un semáforo de exclusión mutua.
 * @param mutex Semáforo a desbloquear.
 */
void thread_mutex_unlock(mutex_t *mutex);

/**
 * Destruye un semáforo de exclusión mutua.
 * @param mutex Semáforo a destruir.
 */
void thread_mutex_destroy(mutex_t *mutex);

/**
 * Crea un semáforo contador.
 * @return Semáforo contador.
 */
sem_t thread_sem(unsigned value);

/**
 * Disminuye el valor de un semáforo contador.
 * Si el valor pasa a ser negativo, se bloquea.
 * @param sem Semáforo contador.
 */
void thread_sem_wait(sem_t *sem);

/**
 * Aumenta el valor de un semáforo contador.
 * @param sem Semáforo contador.
 */
void thread_sem_signal(sem_t *sem);

/**
 * Destruye un semáforo contador.
 * @param mutex Semáforo a destruir.
 */
void thread_sem_destroy(sem_t *sem);

#endif /* COMMON_THREAD_H_ */
