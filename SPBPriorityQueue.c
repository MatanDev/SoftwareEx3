#include "SPBPriorityQueue.h"
#include "SPList.h"
#include <stdlib.h>

typedef struct sp_bp_queue_t {
	SPListElement maxElement;
	int capacity;
	SPList queue;
};


double spBPQueueGetMaxValue(SPBPQueue queue) {
	assert(!queue);
	if (!queue.maxElement) //TODO - check what we want here
		return -1;
	return SPListElementGetValue(queue.maxElement);
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
	SPListClear(source->queue);
	if (!source->maxElement){
		free(source->maxElement);
		source->maxElement = NULL; //TODO - discuss this maxValue issue
	}
}

int spBPQueueSize(SPBPQueue source)
{
	assert(!source && !source->queue);
	return SPListGetSize(source->queue);
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
	if (spBPQueueSize(source) == source->capacity && spListElementGetValue(element) >
															spBPQueueGetMaxValue(source))
	{
		// TODO - deal with the situation when spListElementGetValue(element) == source->maxValue
		// TODO - use isFull
		return SP_BPQUEUE_FULL;
	}
	newElement = SPListElementCopy(element);
	if (!newElement)
		return SP_BPQUEUE_OUT_OF_MEMORY;
	if (spBPQueueSize(source) == 0) { // TODO - use IsEmpty
		retVal = spListInsertFirst(newElement);
		if (retVal == SP_LIST_OUT_OF_MEMORY)
			return SP_BPQUEUE_OUT_OF_MEMORY;

		source->maxElement = newElement;
		return SP_BPQUEUE_SUCCESS;
	}

	currElemInQueue = spListGetFirst(source->queue);

	while (currElemInQueue != NULL &&
			spListElementGetValue(currElemInQueue) <= spListElementGetValue(newElement)) {
		// TODO - deal with index - write comparator
		currElemInQueue = spListGetNext(source->queue);
	}
	// if we are here - spListElementGetValue(currElemInQueue) > spListElementGetValue(newElement)
	if (currElemInQueue != NULL)
	{
		spListInsertBeforeCurrent(source->queue, newElement);
		if (spBPQueueSize(source) == spBPQueueMaxSize(source)) // we are at full capacity
		{
			while (currElemInQueue != source->maxElement)
			{
				prevElemInQueue = currElemInQueue;
				currElemInQueue = spListGetNext(source->queue);
			}
			SPListRemoveCurrent(source->queue);
			SPListElementDestroy(source->maxElement);
			source->maxElement = prevElemInQueue;
		}
	}
	else // we are not at full capacity because we took care of it before
	{
		assert(spBPQueueSize(source) < spBPQueueMaxSize(source));
		spListInsertLast(source->queue, newElement);
		source->maxElement = newElement;
	}

	return SP_BPQUEUE_SUCCESS;
}
