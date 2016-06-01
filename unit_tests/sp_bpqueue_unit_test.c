#include "unit_test_util.h"
#include "../SPBPriorityQueue.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

static SPBPQueue quickRandomQueue(int capacity, int size) {
	SP_BPQUEUE_MSG message;
	int i, index;
	double val;
	SPListElement elem, temp;
	SPBPQueue queue = spBPQueueCreate(capacity);
	printf("Q : {");
	for (i = 0; i < size; i++) {
		val = (double)rand()/((double)RAND_MAX/100);
		index = (int)(rand() % 300);
		//printf("index : %d | val : %f \n",index,val);

		fflush(NULL);
		elem = spListElementCreate(index, val);
		ASSERT_TRUE(elem != NULL);
		message = spBPQueueEnqueue(queue, elem);
		printf("{%d | %f}",index,val);
		ASSERT_TRUE(message == SP_BPQUEUE_SUCCESS || message == SP_BPQUEUE_FULL);
		spListElementDestroy(elem);
		if (capacity > 0){
			temp = spBPQueuePeekLast(queue);
			ASSERT_TRUE(temp != NULL);
			spListElementDestroy(temp);
			temp = spBPQueuePeek(queue);
			ASSERT_TRUE(temp != NULL);
			spListElementDestroy(temp);
		}
	}
	printf(" \nEND\n");
	ASSERT_TRUE(queue!=NULL);
	return queue;
}

static SPBPQueue quickQueue(int capacity, int size, ...) {
	int i;
	va_list items;
	SPBPQueue queue = spBPQueueCreate(capacity);
	va_start(items, size);
	for (i = 0; i < size; i++) {
		spBPQueueEnqueue(queue, va_arg(items, SPListElement));
	}
	va_end(items);
	return queue;
}

static bool testBPQueueCreate() {
	SPBPQueue queue = spBPQueueCreate(7);
	ASSERT_TRUE(queue != NULL);
	spBPQueueDestroy(queue);
	return true;
}


static bool testBPQueueCopy() {
	ASSERT_TRUE(spBPQueueCopy(NULL) == NULL);
	SPBPQueue queue = spBPQueueCreate(1);
	SPBPQueue copy = spBPQueueCopy(queue);
	ASSERT_TRUE(copy != NULL);
	ASSERT_TRUE(0 == spBPQueueSize(copy));

	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	spBPQueueEnqueue(queue, e1);

	ASSERT_TRUE(0 == spBPQueueSize(copy));
	SPBPQueue queue2 = quickQueue(17, 4, e4, e1, e3, e2);
	SPBPQueue copy2 = spBPQueueCopy(queue2);



	ASSERT_TRUE(spBPQueueMaxValue(queue2) == spBPQueueMaxValue(copy2));
	ASSERT_TRUE(4 == spBPQueueSize(copy2));
	ASSERT_TRUE(17 == spBPQueueGetMaxSize(copy2));

	SPListElement temp;
	temp = spBPQueuePeek(copy2);
	ASSERT_TRUE(spListElementCompare(e1, temp) == 0);
	spListElementDestroy(temp);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	ASSERT_TRUE(!spBPQueueIsFull(copy2));
	temp = spBPQueuePeek(copy2);
	ASSERT_TRUE(spListElementCompare(e2, temp) == 0);
	spListElementDestroy(temp);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	temp = spBPQueuePeek(copy2);
	ASSERT_TRUE(spListElementCompare(e3, temp) == 0);
	spListElementDestroy(temp);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	temp = spBPQueuePeek(copy2);
	ASSERT_TRUE(spListElementCompare(e4, temp) == 0);
	spListElementDestroy(temp);
	temp = NULL;
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	temp = spBPQueuePeek(copy2);
	ASSERT_TRUE(temp == NULL);
	spListElementDestroy(temp);


	spBPQueueDestroy(queue);
	spBPQueueDestroy(queue2);
	spBPQueueDestroy(copy);
	spBPQueueDestroy(copy2);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

static bool testBPQueueSize() {
	SPListElement temp;
	SPBPQueue queue = spBPQueueCreate(2);
	ASSERT_TRUE(0 == spBPQueueSize(queue));
	SPListElement e1 = spListElementCreate(1, 1.0);
	spBPQueueEnqueue(queue, e1);
	ASSERT_TRUE(1 == spBPQueueSize(queue));
	spListElementSetIndex(e1, 2);
	spListElementSetValue(e1, 2.0);
	spBPQueueEnqueue(queue, e1);
	ASSERT_TRUE(2 == spBPQueueSize(queue));
	temp = spBPQueuePeek(queue);
	spListElementDestroy(temp);
	spBPQueueDequeue(queue);
	ASSERT_TRUE(1 == spBPQueueSize(queue));
	spBPQueueDestroy(queue);
	spListElementDestroy(e1);
	return true;
}

static bool testBPQueueMaxSize() {
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	SPBPQueue queue = quickQueue(5, 4, e2, e3, e1, e4);
	ASSERT_TRUE(4 == spBPQueueSize(queue));
	ASSERT_TRUE(5 == spBPQueueGetMaxSize(queue));
	ASSERT_TRUE(spBPQueueSize(queue) < spBPQueueGetMaxSize(queue));
	spBPQueueDestroy(queue);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}


static bool testBPQueuePeekFirstandLast() {
	SPBPQueue queue = spBPQueueCreate(0);
	ASSERT_TRUE(spBPQueuePeek(queue) == NULL);
	ASSERT_TRUE(spBPQueuePeekLast(queue) == NULL);
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	SPBPQueue queue2 = quickQueue(5, 4, e2, e3, e1, e4);
	SPListElement first = spBPQueuePeek(queue2) , temp;
	ASSERT_TRUE(spListElementCompare(e1, first) == 0);
	temp = spBPQueuePeek(queue2);
	ASSERT_TRUE(spListElementCompare(first, temp) == 0);
	spListElementDestroy(temp);
	spListElementDestroy(first);
	SPListElement last = spBPQueuePeekLast(queue2);
	ASSERT_TRUE(spListElementCompare(e4, last) == 0);
	temp = spBPQueuePeekLast(queue2);
	ASSERT_TRUE(spListElementCompare(last, temp) == 0);
	spListElementDestroy(last);
	spListElementDestroy(temp);
	spBPQueueDestroy(queue);
	spBPQueueDestroy(queue2);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

static bool testBPQueueMinMaxValue() {
	SPBPQueue queue = spBPQueueCreate(0);
	ASSERT_TRUE(spBPQueueMinValue(queue) == -1);
	ASSERT_TRUE(spBPQueueMaxValue(queue) == -1);
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	SPBPQueue queue2 = quickQueue(5, 4, e2, e3, e1, e4);
	ASSERT_TRUE(spBPQueueMinValue(queue2) == 1);
	ASSERT_TRUE(spBPQueueMaxValue(queue2) == 4);
	spBPQueueDestroy(queue);
	spBPQueueDestroy(queue2);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

static bool testBPQueueClear() {
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	SPBPQueue queue = quickQueue(7, 4, e3, e4, e2, e1);
	spBPQueueClear(queue);
	ASSERT_TRUE(0 == spBPQueueSize(queue));
	ASSERT_TRUE(7 == spBPQueueGetMaxSize(queue));
	SPBPQueue queue2 = spBPQueueCreate(19);
	spBPQueueClear(queue2);
	ASSERT_TRUE(0 == spBPQueueSize(queue2));
	spBPQueueDestroy(queue);
	spBPQueueDestroy(queue2);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

static bool testBPQueueDestroy() {
	spBPQueueDestroy(NULL);
	return true;
}

static bool testSorted(){
	SPListElement prevElement, currentElement;
	SP_BPQUEUE_MSG message;
	int size,max_size, counter,i, numOfTests = 3000;
	SPBPQueue queue = NULL;
	for (i = 0 ; i< numOfTests ; i++)
	{
		printf("Test #%d\n",i);
		size = (int)(rand() % 20);
		max_size = (int)(rand() % 20);
		printf("size: %d | max_size: %d \n",size,max_size);
		counter = 0;
		ASSERT_TRUE(queue == NULL);
		queue = quickRandomQueue(max_size, size);
		ASSERT_TRUE(queue != NULL);
		printf("counter : %d | size: %d | max_size: %d | getSize : %d | getMaxSize : %d \n",
					counter,size,max_size,spBPQueueSize(queue), spBPQueueGetMaxSize(queue));
		ASSERT_TRUE(queue != NULL);
		if (!spBPQueueIsEmpty(queue)){
			currentElement = spBPQueuePeek(queue);
			message	= spBPQueueDequeue(queue);
			ASSERT_TRUE(message == SP_BPQUEUE_SUCCESS);
			counter++;
		}
		while (!spBPQueueIsEmpty(queue)){
			spListElementDestroy(prevElement);
			prevElement = spListElementCopy(currentElement);
			currentElement = spBPQueuePeek(queue);
			message	= spBPQueueDequeue(queue);
			ASSERT_TRUE(prevElement != NULL);
			ASSERT_TRUE(currentElement != NULL);
			ASSERT_TRUE(message == SP_BPQUEUE_SUCCESS);
			ASSERT_TRUE(spListElementCompare(prevElement,currentElement)<=0);
			counter++;
		}
		printf("counter : %d | size: %d | max_size: %d | getSize : %d | getMaxSize : %d \n",
				counter,size,max_size,spBPQueueSize(queue), spBPQueueGetMaxSize(queue));
		ASSERT_TRUE(counter == (size < max_size ? size : max_size) );
		ASSERT_TRUE(spBPQueueIsEmpty(queue));
		spListElementDestroy(currentElement);
		spListElementDestroy(prevElement);
		spBPQueueDestroy(queue);
		queue = NULL;
	}
	return true;
}

static bool testCase1()
{
	SPListElement e1 = spListElementCreate(49 , 60.628071);
	SPListElement e2 = spListElementCreate(95 , 0.589007);
	SPListElement e3 = spListElementCreate(16 , 10.061953);
	SPListElement e4 = spListElementCreate(205 , 86.324656);
	SPListElement e5 = spListElementCreate(82 , 74.733726);
	SPListElement e6 = spListElementCreate(234 , 38.010804);
	SPListElement e7 = spListElementCreate(1 , 55.281228);
	SPListElement e8 = spListElementCreate(271 , 95.571764);
	SPListElement e9 = spListElementCreate(263 , 17.658010);
	SPBPQueue queue = quickQueue(17, 9, e1,e2,e3,e4,e5,e6,e7,e8,e9);
	spBPQueueDestroy(queue);
	spListElementDestroy(e1); e1 = NULL;
	spListElementDestroy(e2); e2 = NULL;
	spListElementDestroy(e3); e3 = NULL;
	spListElementDestroy(e4); e4 = NULL;
	spListElementDestroy(e5); e5 = NULL;
	spListElementDestroy(e6); e6 = NULL;
	spListElementDestroy(e7); e7 = NULL;
	spListElementDestroy(e8); e8 = NULL;
	spListElementDestroy(e9); e9 = NULL;
	return true;

}

int main() {
	RUN_TEST(testBPQueueCreate);
	RUN_TEST(testBPQueueCopy);
	RUN_TEST(testBPQueueSize);
	RUN_TEST(testBPQueueMaxSize);
	RUN_TEST(testBPQueuePeekFirstandLast);
	RUN_TEST(testBPQueueMinMaxValue);
	RUN_TEST(testBPQueueClear);
	RUN_TEST(testBPQueueDestroy);
	RUN_TEST(testCase1);
	RUN_TEST(testSorted);
	return 0;
}
