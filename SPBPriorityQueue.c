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
SPBPQueue spBPQueueCreateWrapper(int maxSize,int createDataList)
{
	SPBPQueue newQueue;

	if (maxSize < 0)
		return NULL;

	newQueue = (SPBPQueue)calloc(1,sizeof(struct sp_bp_queue_t));

	if (newQueue == NULL)
		return NULL;

	newQueue->capacity = maxSize;
	newQueue->maxElement = NULL;

	if (createDataList)
	{
		newQueue->queue = spListCreate();
		if (newQueue->queue == NULL)
			return NULL;
	}
	return newQueue;
}

SPBPQueue spBPQueueCreate(int maxSize)
{
	SPBPQueue newQueue;

	if (maxSize < 0)
		return NULL;

	newQueue = spBPQueueCreateWrapper(maxSize,1);

	if (newQueue == NULL)
		return NULL;

	newQueue->queue = spListCreate();

	return newQueue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source)
{
	if (source == NULL)
		return NULL;

	SPBPQueue newQueue = spBPQueueCreateWrapper(source->capacity,0);

	if (newQueue == NULL)
		return NULL;

	newQueue->queue = spListCopy(source->queue);

	if (newQueue->queue == NULL)
		return NULL;

	return newQueue;
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

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element)
{
	SPListElement newElement, currElemInQueue, prevElemInQueue;

	if (source == NULL || element == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	// the list is full and the element is greater than all the current items
	if (spBPQueueIsFull(source) && spListElementCompare(element,source->maxElement) > 0)
		return SP_BPQUEUE_FULL;

	newElement = spListElementCopy(element);
	if (!newElement)
		return SP_BPQUEUE_OUT_OF_MEMORY;

	if (spBPQueueIsEmpty(source)) {
		return spBPQueueInsertIfEmpty(source,newElement);
	}

	currElemInQueue = spListGetFirst(source->queue);

	spBPQueueInsertItem(source, newElement);

	while (currElemInQueue != NULL &&
			spListElementCompare(currElemInQueue,newElement) <= 0  ){
			//spListElementGetValue(currElemInQueue) <= spListElementGetValue(newElement)) {
		currElemInQueue = spListGetNext(source->queue);
	}
	// if we are here - spListElementGetValue(currElemInQueue) > spListElementGetValue(newElement)
	if (currElemInQueue != NULL)
	{
		spListInsertBeforeCurrent(source->queue, newElement);
		if (spBPQueueIsFull(source)) // we are at full capacity
		{
			while (currElemInQueue != source->maxElement)
			{
				prevElemInQueue = currElemInQueue;
				currElemInQueue = spListGetNext(source->queue);
			}
			spListRemoveCurrent(source->queue);
			spListElementDestroy(source->maxElement);
			source->maxElement = prevElemInQueue;
		}
	}
	else // we are not at full capacity because we took care of it before
	{
		//assert(spBPQueueSize(source) < spBPQueueGetMaxSize(source));
		spListInsertLast(source->queue, newElement); //TODO - this returns a message that we need to bubble up
		source->maxElement = newElement;
	}

	return SP_BPQUEUE_SUCCESS;
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
