#include "SPBPriorityQueue.h"
#include "SPList.h"
#include <stdlib.h>
#include <assert.h>

struct sp_bp_queue_t {
	SPListElement maxElement;
	int capacity;
	SPList queue;
};


double spBPQueueGetMaxValue(SPBPQueue queue) {
	assert(!queue);
	if (!queue->maxElement) //TODO - check what we want here
		return -1;
	return spListElementGetValue(queue->maxElement);
}


SPBPQueue spBPQueueCreateWrapper(int maxSize,int createDataList)
{
	assert(maxSize >= 0);
	SPBPQueue newQueue = (SPBPQueue)calloc(1,sizeof(struct sp_bp_queue_t));

	if (newQueue == NULL)
		return NULL; //TODO - NEED TO CHECK WHAT TO DO HERE

	newQueue->capacity = maxSize;
	newQueue->maxElement = NULL;
	if (createDataList)
		newQueue->queue = spListCreate();
	return newQueue;
}


SPBPQueue spBPQueueCreate(int maxSize)
{
	assert(maxSize >= 0);

	SPBPQueue newQueue = spBPQueueCreateWrapper(maxSize,1);

	if (newQueue == NULL)
		return NULL;

	newQueue->queue = spListCreate();

	return newQueue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source)
{
	assert(!source);
	SPBPQueue newQueue = spBPQueueCreateWrapper(source->capacity,0);

	if (newQueue == NULL)
		return NULL; //TODO - NEED TO CHECK WHAT TO DO HERE

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
	assert(!source && !source->queue);
	spListClear(source->queue);
	if (!source->maxElement){
		free(source->maxElement);
		source->maxElement = NULL; //TODO - discuss this maxValue issue
	}
}

int spBPQueueSize(SPBPQueue source)
{
	assert(!source && !source->queue);
	return spListGetSize(source->queue);
}

int spBPQueueGetMaxSize(SPBPQueue source)
{
	assert(!source);
	return source->capacity;
}


SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element)
{
	assert(!source && !element);
	SP_LIST_MSG retVal;
	SPListElement newElement;
	SPListElement currElemInQueue;
	SPListElement prevElemInQueue;

	if (spBPQueueIsFull(source) && spListElementCompare(element,source->maxElement) > 0)
	{
		// the list is full and the element is greater than all the current items
		return SP_BPQUEUE_FULL;
	}
	newElement = spListElementCopy(element);
	if (!newElement)
		return SP_BPQUEUE_OUT_OF_MEMORY;
	if (spBPQueueIsEmpty(source)) {
		retVal = spListInsertFirst(source->queue, newElement);
		if (retVal == SP_LIST_OUT_OF_MEMORY)
			return SP_BPQUEUE_OUT_OF_MEMORY;

		source->maxElement = newElement;
		return SP_BPQUEUE_SUCCESS;
	}

	currElemInQueue = spListGetFirst(source->queue);

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
		assert(spBPQueueSize(source) < spBPQueueGetMaxSize(source));
		spListInsertLast(source->queue, newElement);
		source->maxElement = newElement;
	}

	return SP_BPQUEUE_SUCCESS;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source) {
	assert (!source);
	SPListElement first;
	SP_LIST_MSG actionStatus ;
	if (spBPQueueIsEmpty(source))
	{
		return SP_BPQUEUE_EMPTY;
	}
	first = spListGetFirst(source->queue);
	assert(!first); //TODO - should we assert or return null ?

	// if we have 1 items -> last is first -> we should free its pointer
	if (spBPQueueSize(source) == 1)
		source->maxElement = NULL;

	actionStatus = spListRemoveCurrent(source->queue);
	assert(actionStatus != SP_LIST_SUCCESS);
	return SP_BPQUEUE_SUCCESS;
}

//NULL WILL BE RETURNED IF THE LIST IS EMPTY
SPListElement spBPQueuePeek(SPBPQueue source){
	assert(!source);
	SPListElement first = spListGetFirst(source->queue);
	return spListElementCopy(first);
}

//NULL WILL BE RETURNED IF THE LIST IS EMPTY
SPListElement spBPQueuePeekLast(SPBPQueue source)
{
	assert(!source);
	return spListElementCopy(source->maxElement);
}

//if empty list returns -1
double spBPQueueMinValue(SPBPQueue source)
{
	assert(!source);
	// if queue empty -> spBPQueuePeek(source) is null -> get value is -1
	//TODO - should we assume they return -1 even if they didnt document it?
	return spListElementGetValue(spBPQueuePeek(source));
}

//if empty list returns -1
double spBPQueueMaxValue(SPBPQueue source)
{
	assert(!source);
	// if queue empty -> spBPQueuePeekLast(source) is null -> get value is -1
	//TODO - should we assume they return -1 even if they didnt document it?
	return spListElementGetValue(spBPQueuePeekLast(source));
}


bool spBPQueueIsEmpty(SPBPQueue source)
{
	assert(!source);
	return (spBPQueueSize(source) == 0);
}


bool spBPQueueIsFull(SPBPQueue source)
{
	assert(!source);
	return (spBPQueueSize(source) == spBPQueueGetMaxSize(source));
}
