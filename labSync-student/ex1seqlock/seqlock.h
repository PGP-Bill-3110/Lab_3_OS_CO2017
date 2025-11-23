#ifndef PTHREAD_H
#include <pthread.h>
#endif

typedef struct pthread_seqlock { /* TODO: implement the structure */
   unsigned seq;
   pthread_mutex_t write_lock;
} pthread_seqlock_t;

static inline void pthread_seqlock_init(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   rw->seq = 0;
   pthread_mutex_init(&rw->write_lock, NULL);
}

static inline void pthread_seqlock_wrlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   pthread_mutex_lock(&rw->write_lock);
   rw->seq++; 
}

static inline void pthread_seqlock_wrunlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   rw->seq++;
   pthread_mutex_unlock(&rw->write_lock);
}

static inline unsigned pthread_seqlock_rdlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   unsigned seq;
   do {
       seq = rw->seq;
   } while (seq & 1); 
   return seq;
}
static inline unsigned pthread_seqlock_rdunlock(pthread_seqlock_t *rw)
{
   /* TODO: ... */
   return rw->seq;
 }

