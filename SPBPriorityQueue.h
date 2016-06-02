#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

void PrintQueue(SPBPQueue source); //TODO - remove at production

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new queue.
 * This function creates a new empty queue.
 * @param maxSize - a limit for the size of the queue
 * @return
 * 	NULL - If allocations failed or maxSize < 0
 * 	A new queue in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target queue.
 *
 * The new copy will contains the same capacity limit and a copy of the queue items
 *
 * @param source The target source to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A queue containing the same capacity and the same queue items.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates an existing queue. Clears all elements by using the
 * stored free function.
 *
 * @param source - queue to be deallocated. If list is NULL nothing will be
 * done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from the queue.
 *
 * The elements are deallocated using the list clear items
 * @param source -  Target queue to remove all element from
 * does nothing if source is NULL or the source internal list is NULL
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in the queue.
 *
 * @param source - The target which size is requested.
 * @return
 * -1 if a NULL pointer was sent or in case of internal NULL pointer
 * Otherwise the number of elements in the queue.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * Returns the maximum limit (capacity) of elements in the queue.
 *
 * @param source - The target which max size is requested.
 * @return
 * -1 if a NULL pointer was sent
 * Otherwise the capacity of the queue.
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Insert a new item to the queue
 * at the suitable place, while keeping the internal list sorted
 * without violating the capacity limit
 *s
 * @param source - The target which the enqueue is requested on.
 * @param element - the element to insert to the queue
 *
 * @return
 * 	SP_BPQUEUE_OUT_OF_MEMORY - in case of memory allocation error
 *	SP_BPQUEUE_FULL - in case the queue is at full capacity and the requested
 *					  element is greater than than all the elements in the queue
 *	SP_BPQUEUE_INVALID_ARGUMENT - in case source is NULL or element is NULL
 *	SP_BPQUEUE_SUCCESS - in case the element was successfully inserted to the queue
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);

/**
 * Removes the minimal item from the queue
 *
 * @param source - The target which the dequeue is requested on.
 *
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if source is NULL
 * SP_BPQUEUE_EMPTY if the queue if empty
 * SP_BPQUEUE_SUCCESS the element has been removed successfully
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);

/**
 * The method is used to get a copy of the first (minimum) item in the queue.
 * @param source - The target which the check is requested on.
 * @return
 * NULL if source is NULL or queue is empty, otherwise returns the
 * a hard copy of the first (minimum) item in the queue.
 */
SPListElement spBPQueuePeek(SPBPQueue source);

/**
 * The method is used to get a copy of the last (maximum) item in the queue.
 * @param source - The target which the check is requested on.
 * @return
 * NULL if source is NULL or queue is empty, otherwise returns the
 * a hard copy of the last (maximum) item in the queue.
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);

/**
 * The method is used to get the minimum value of the items in the queue.
 * @param source - The target which the check is requested on.
 * @return
 * -1 if source is NULL or queue is empty, otherwise returns the
 * minimum value of the items in the queue.
 */
double spBPQueueMinValue(SPBPQueue source);

/**
 * The method is used to get the maximum value of the items in the queue.
 * @param source - The target which the check is requested on.
 * @return
 * -1 if source is NULL or queue is empty, otherwise returns the
 * maximum value of the items in the queue.
 */
double spBPQueueMaxValue(SPBPQueue source);

/**
 * Check if the queue is empty.
 * Assert source != NULL
 * @param source - The target which the check is requested on.
 * @return
 * true iff the queue is empty
 */
bool spBPQueueIsEmpty(SPBPQueue source);

/**
 * Check if the queue is at full capacity.
 * Assert source != NULL
 * @param source - The target which the check is requested on.
 * @return
 * true iff the queue is at full capacity
 */
bool spBPQueueIsFull(SPBPQueue source);

#endif
