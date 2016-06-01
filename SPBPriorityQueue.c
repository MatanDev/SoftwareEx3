#include "SPBPriorityQueue.h"
#include "SPList.h"
#include <stdlib.h>
#include <assert.h>
#include <stdio.h> //TODO - remove at production

/*
 * A structure used in order to handle the queue data type
 * maxElement - a list element representing the maximum element in the queue
 * capacity - an integer representing a size limit for the queue
 * queue - a list used to store the queue items
 */
struct sp_bp_queue_t {
	SPListElement maxElement;
	int capacity;
	SPList queue;
};


/**
 * Allocates a new queue.
 * This function creates a new empty queue,
 * Using a flag regarding the creation of the internal data list, if the flag is on
 * the method will create a new list, otherwise it will use a copy of the given source queue
 * @param maxSize - a limit for the size of the queue
 * @param source_queue - the given queue, this parameter should be NULL if the createNewList flag is on
 * @param createNewList - a flag used to indicate whether to create the internal list
 * @return
 * 	NULL - If allocations failed or maxSize < 0 or createNewList flag is off and source_queue is NULL
 * 	A new queue in case of success, with respect to the createNewList flag
 */
SPBPQueue spBPQueueCreateWrapper(int maxSize, SPBPQueue source_queue, bool createNewList)
{
	SPBPQueue newQueue;

	if (maxSize < 0)
		return NULL;

	newQueue = (SPBPQueue)calloc(1,sizeof(struct sp_bp_queue_t));

	if (newQueue == NULL) //allocation error
		return NULL;

	newQueue->capacity = maxSize;


	if (createNewList)
	{
		newQueue->queue = spListCreate();
		newQueue->maxElement = NULL;
	}
	else
	{
		newQueue->queue = spListCopy(source_queue->queue);
		newQueue->maxElement = spListElementCopy(source_queue->maxElement);
	}

	//allocation error, or given source queue is NULL and createNewList if false
	if (newQueue->queue == NULL)
		return NULL;

	return newQueue;
}

SPBPQueue spBPQueueCreate(int maxSize)
{
	if (maxSize < 0)
		return NULL;

	return spBPQueueCreateWrapper(maxSize, NULL, true);
}

SPBPQueue spBPQueueCopy(SPBPQueue source)
{
	if (source == NULL)
		return NULL;

	return spBPQueueCreateWrapper(source->capacity, source , false);
}

void spBPQueueDestroy(SPBPQueue source)
{
	if (source != NULL) {
		if (source->queue != NULL)
			spListDestroy(source->queue);
		spListElementDestroy(source->maxElement);
		source->maxElement = NULL;
		free(source);
		source = NULL;
	}
}

void spBPQueueClear(SPBPQueue source)
{
	if (source && source->queue)
	{
		spListClear(source->queue);
		if (!source->maxElement){
			spListElementDestroy(source->maxElement);
			source->maxElement = NULL;
		}
	}
}

int spBPQueueSize(SPBPQueue source)
{
	if(source == NULL || source->queue == NULL)
		return -1;
	return spListGetSize(source->queue);
}

int spBPQueueGetMaxSize(SPBPQueue source)
{
	if (source == NULL)
		return -1;
	return source->capacity;
}

/*
 * The method gets a queue and an element and inserts the element to the queue
 * Pre assumptions:
 *  source is empty, source != NULL , newElement != NULL
 * @param source - the given queue to work on
 * @param newElement - the given element to insert
 * @return
 * SP_BPQUEUE_OUT_OF_MEMORY - in case of allocation error
 * SP_BPQUEUE_SUCCESS - in case the new is inserted correctly
 */
SP_BPQUEUE_MSG spBPQueueInsertIfEmpty(SPBPQueue source, SPListElement newElement){
	SP_LIST_MSG retVal;
	retVal = spListInsertFirst(source->queue, newElement);
	if (retVal == SP_LIST_OUT_OF_MEMORY)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	source->maxElement = newElement;
	return SP_BPQUEUE_SUCCESS;
}

/*
 * The method removes the last element in the queue (to prevent capacity overflow)
 * The method is given a queue and an element
 * representing the current list iterator of the internal list used by the queue
 * it iterate over the list items, removes the last one (the maximum) and update the current maximum
 * Pre-assumptions
 *  source != NULL, list is at full capacity and currElemInQueue != NULL
 *  @param source - the given queue from which we should remove the last item
 *  @param currElemInQueue - a pointer to the current iterator of the internal list in the queue
 */
void spBPQueueHandleFullCapacity(SPBPQueue source, SPListElement currElemInQueue) {
	SPListElement prevElemInQueue;
	SP_LIST_MSG listMessage;
	assert(currElemInQueue != NULL); //TODO - remove at production
	assert(spBPQueueSize(source) == 0 || source->maxElement != NULL); //TODO - remove at production

	//assert(spBPQueueSize(source) -1 <= spBPQueueGetMaxSize(source));
	currElemInQueue = spListGetFirst(source->queue);

	while (currElemInQueue!= NULL && spListElementCompare(currElemInQueue, source->maxElement) != 0)
	{
		prevElemInQueue = currElemInQueue;
		currElemInQueue = spListGetNext(source->queue);
		assert(spBPQueueSize(source) == 0 || source->maxElement != NULL); //TODO - remove at production
	}

	//TODO - decide what to do with invalid current regarding spListRemoveCurrent

	listMessage = spListRemoveCurrent(source->queue);
	assert(listMessage == SP_LIST_SUCCESS);
	if (spBPQueueSize(source) -1 >= spBPQueueGetMaxSize(source)) //TODO - remove at production
	{
		printf("problem : size %d | max_size : %d \n",
				spBPQueueSize(source), spBPQueueGetMaxSize(source));
		printf("prev : { %d : %f } -------  maxElement : { %d : %f }\n",
				spListElementGetIndex(prevElemInQueue),
				spListElementGetValue(prevElemInQueue),
				spListElementGetIndex(source->maxElement),
				spListElementGetValue(source->maxElement)
				);

	}

	spListElementDestroy(source->maxElement);
	assert(prevElemInQueue!= NULL); //TODO - remove at production
	source->maxElement = spListElementCopy(prevElemInQueue);


}


/*
 * The method inserts an item to the queue before a given item,
 * assuming the insertion is not at the end of the queue and the queue is not empty
 * Pre-assumptions:
 * - source != NULL and source->list not empty and element != NULL and currElemInQueue != NULL
 *  @param source - the given queue from which we should insert the item
 *  @param element - the element we should insert to the queue
 *  @param currElemInQueue - a pointer to the current iterator of the internal list in the queue
 *  @return
 *  SP_BPQUEUE_OUT_OF_MEMORY - in case of memory error
 *  SP_BPQUEUE_SUCCESS - in case the item was successfully inserted to the queue
 */
SP_BPQUEUE_MSG spBPQueueInsertNotEmptyNotLast(SPBPQueue source,
		SPListElement element, SPListElement currElemInQueue) {
	SP_LIST_MSG retVal;
	retVal = spListInsertBeforeCurrent(source->queue, element);

	//TODO - decide what to do with invalid current
	if (retVal == SP_LIST_INVALID_CURRENT || retVal == SP_LIST_OUT_OF_MEMORY)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	// TODO - needs to be we were on full capacity
	// 2 options - one: check if we are over capacity by 1
	// two: change the ordering to check this first and then insert
	//if (spBPQueueIsFull(source)) // we are at full capacity
	if (spBPQueueSize(source) > spBPQueueGetMaxSize(source))
		spBPQueueHandleFullCapacity(source, currElemInQueue);

	return SP_BPQUEUE_SUCCESS;
}

/*
 * The method inserts an item to the queue at its end,
 * assuming the queue is not empty
 * Pre-assumptions:
 * - source != NULL and source->list not empty and not at (full capacity - 1) and element != NULL
 *  @param source - the given queue to which we should insert the item
 *  @param element - the element we should insert to the queue
 *  @return
 *  SP_BPQUEUE_OUT_OF_MEMORY - in case of memory error
 *  SP_BPQUEUE_SUCCESS - in case the item was successfully inserted to the queue
 */
SP_BPQUEUE_MSG spBPQueueInsertNotEmptyButLast(SPBPQueue source, SPListElement element) {
	SP_LIST_MSG retVal;
	retVal = spListInsertLast(source->queue, element);

	if (retVal == SP_LIST_OUT_OF_MEMORY)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	source->maxElement = element;

	return SP_BPQUEUE_SUCCESS;
}

/*
 * The method inserts an item to the queue,
 * assuming the queue is not empty
 * Pre-assumptions:
 * - source != NULL and source->list not empty and newElement != NULL
 *  @param source - the given queue to which we should insert the item
 *  @param newElement - the element we should insert to the queue
 *  @return
 *  SP_BPQUEUE_OUT_OF_MEMORY - in case of memory error
 *  SP_BPQUEUE_SUCCESS - in case the item was successfully inserted to the queue
 */
SP_BPQUEUE_MSG spBPQueueInsertNotEmpty(SPBPQueue source, SPListElement newElement){
	SPListElement currElemInQueue = spListGetFirst(source->queue);

	assert(currElemInQueue != NULL && newElement != NULL); //TODO - remove at production
	// TODO - check if < or <=
	while (currElemInQueue != NULL && spListElementCompare(currElemInQueue, newElement) <= 0)
	{
		currElemInQueue = spListGetNext(source->queue);
		assert( newElement != NULL); //TODO - remove at production
	}

	if (currElemInQueue != NULL) // currElemInQueue > newElement
		return spBPQueueInsertNotEmptyNotLast(source, newElement, currElemInQueue);

	// we are not at full capacity because we took care of it before
	return spBPQueueInsertNotEmptyButLast(source, newElement);
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element)
{
	SPListElement newElement;

	if (source == NULL || source->queue == NULL || element == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	if (spBPQueueGetMaxSize(source) == 0)
		return SP_BPQUEUE_FULL; //TODO - maybe this should be success

	assert(element != NULL); //TODO - remove at production
	assert(source != NULL); //TODO - remove at production
	assert(spBPQueueSize(source) == 0 || source->maxElement != NULL); //TODO - remove at production

	// the list is full and the element is greater than all the current items
	if (spBPQueueIsFull(source) && spListElementCompare(element, source->maxElement) > 0){
		return SP_BPQUEUE_FULL; //TODO - track the forum to verify if we need to return success
		assert(element != NULL); //TODO - remove at production
		assert(spBPQueueSize(source) == 0 || source->maxElement != NULL); //TODO - remove at production

	}
	newElement = spListElementCopy(element);

	if (!newElement)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	if (spBPQueueIsEmpty(source))
		return spBPQueueInsertIfEmpty(source,newElement);
	//insert to a non empty queue

	return spBPQueueInsertNotEmpty(source, newElement);
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	SPListElement first;
	SP_LIST_MSG actionStatus;

	if (source == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	assert(source); //TODO - remove before production
	if (spBPQueueIsEmpty(source))
		return SP_BPQUEUE_EMPTY;

	first = spListGetFirst(source->queue);
	if (!first)
		return SP_BPQUEUE_EMPTY;

	// if we have 1 items -> last is first -> we should free its pointer
	if (spBPQueueSize(source) == 1)
		source->maxElement = NULL;

	actionStatus = spListRemoveCurrent(source->queue);

	if (actionStatus != SP_LIST_SUCCESS)
		return SP_BPQUEUE_EMPTY;

	return SP_BPQUEUE_SUCCESS;
}

SPListElement spBPQueuePeek(SPBPQueue source){
	SPListElement first;
	if (source == NULL)
		return NULL;
	first = spListGetFirst(source->queue);
	return spListElementCopy(first);
}

SPListElement spBPQueuePeekLast(SPBPQueue source)
{
	if (source == NULL)
		return NULL;
	return spListElementCopy(source->maxElement);
}

double spBPQueueMinValue(SPBPQueue source)
{
	if (source == NULL)
		return -1;
	return spListElementGetValue(spBPQueuePeek(source));
}

double spBPQueueMaxValue(SPBPQueue source)
{
	if (source == NULL)
		return -1;
	return spListElementGetValue(spBPQueuePeekLast(source));
}

bool spBPQueueIsEmpty(SPBPQueue source)
{
	assert(source);
	return (spBPQueueSize(source) == 0);
}

bool spBPQueueIsFull(SPBPQueue source)
{
	assert(source);
	return (spBPQueueSize(source) == spBPQueueGetMaxSize(source));
}
