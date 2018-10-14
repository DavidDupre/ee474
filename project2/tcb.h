#ifndef _TCB_H_
#define _TCB_H_


typedef struct {
    void *data;
    void (*task)(void *data);
} TCB;


#endif  /* _TCB_H_ */
