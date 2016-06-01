#include "SPBPriorityQueue.h"
#include "SPList.h"
#include <stdlib.h>
#include <assert.h>

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

// TODO - change documentation a little
/**
 * Allocates a new queue.
 * This function creates a new empty queue,
 * Using a flag regarding the creation of the internal data list
 * @param maxSize - a limit for the size of the queue
 * @param createDataList - a flag used to indicate whether to create the internal list
 * @return
 * 	NULL - If allocations failed or maxSize < 0
 * 	A new queue in case of success.
 */
SPBPQueue spBPQueueCreateWrapper(int maxSize, SPBPQueue source)
{
	SPBPQueue newQueue;

	if (maxSize < 0)
		return NULL;

	newQueue = (SPBPQueue)calloc(1,sizeof(struct sp_bp_queue_t));

	if (newQueue == NULL)
		return NULL;

	newQueue->capacity = maxSize;
	newQueue->maxElement = NULL;

	if (source == NULL)
		newQueue->queue = spListCreate();
	else
		newQueue->queue = spListCopy(source->queue);

	if (newQueue->queue == NULL)
		return NULL;

	return newQueue;
}

SPBPQueue spBPQueueCreate(int maxSize)
{
	if (maxSize < 0)
		return NULL;

	return spBPQueueCreateWrapper(maxSize, NULL);
}

SPBPQueue spBPQueueCopy(SPBPQueue source)
{
	if (source == NULL)
		return NULL;

	return spBPQueueCreateWrapper(source->capacity, source);
}

void spBPQueueDestroy(SPBPQueue source)
{
	if (source != NULL) {
		if (source->queue != NULL)
			spListDestroy(source->queue);
		free(source);
		source = NULL;
	}
}

void spBPQueueClear(SPBPQueue source)
{
	if (!source && !source->queue)
	{
		spListClear(source->queue);
		if (!source->maxElement){
			free(source->maxElement);
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

SP_BPQUEUE_MSG spBPQueueInsertIfEmpty(SPBPQueue source, SPListElement newElement){
	SP_LIST_MSG retVal;
	retVal = spListInsertFirst(source->queue, newElement);
	if (retVal == SP_LIST_OUT_OF_MEMORY)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	source->maxElement = newElement;
	return SP_BPQUEUE_SUCCESS;
}

void spBPQueueHandleFullCapacity(SPBPQueue source, SPListElement currElemInQueue) {
	SPListElement prevElemInQueue;

	while (spListElementCompare(currElemInQueue, source->maxElement))
	{
		prevElemInQueue = currElemInQueue;
		currElemInQueue = spListGetNext(source->queue);
	}

	//TODO - decide what to do with invalid current regarding spListRemoveCurrent
	spListRemoveCurrent(source->queue);
	spListElementDestroy(source->maxElement);
	source->maxElement = prevElemInQueue;
}

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

SP_BPQUEUE_MSG spBPQueueInsertNotEmptyButLast(SPBPQueue source, SPListElement element) {
	SP_LIST_MSG retVal;
	retVal = spListInsertLast(source->queue, element);

	if (retVal == SP_LIST_OUT_OF_MEMORY)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	source->maxElement = element;

	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element)
{
	SPListElement newElement, currElemInQueue;

	if (source == NULL || source->queue == NULL || element == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	// the list is full and the element is greater than all the current items
	if (spBPQueueIsFull(source) && spListElementCompare(element, source->maxElement) > 0)
		return SP_BPQUEUE_FULL;

	newElement = spListElementCopy(element);

	if (!newElement)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	if (spBPQueueIsEmpty(source))
		return spBPQueueInsertIfEmpty(source,newElement);

	// TODO - exclude all lines from here till end to "spBPQueueInsertNotEmpty"?
	currElemInQueue = spListGetFirst(source->queue);

	// TODO - check if < or <=
	while (currElemInQueue != NULL && spListElementCompare(currElemInQueue, newElement) <= 0)
		currElemInQueue = spListGetNext(source->queue);

	if (currElemInQueue != NULL) // currElemInQueue > newElement
		return spBPQueueInsertNotEmptyNotLast(source, newElement, currElemInQueue);

	// we are not at full capacity because we took care of it before
	return spBPQueueInsertNotEmptyButLast(source, newElement);
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	SPListElement first;
	SP_LIST_MSG actionStatus;

	if (source == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

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
