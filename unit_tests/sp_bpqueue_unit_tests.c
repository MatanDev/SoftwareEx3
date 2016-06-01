#include "unit_test_util.h"
#include "../SPBPriorityQueue.h"
#include <stdarg.h>
#include <stdio.h>

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
	ASSERT_TRUE(spBPQueueIsEmpty(copy));
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	spBPQueueEnqueue(queue, e1);
	ASSERT_TRUE(0 == spBPQueueSize(copy));
	SPBPQueue queue2 = quickQueue(4, 4, e4, e1, e3, e2);
	//SPBPQueue queue2 = quickQueue(4, e1, e2, e3, e4);
	printf("%d\n", spBPQueueSize(queue2));
	SPBPQueue copy2 = spBPQueueCopy(queue2);
	printf("%d\n", spBPQueueSize(copy2));
	ASSERT_TRUE(4 == spBPQueueSize(copy2));
	ASSERT_TRUE(spBPQueueIsFull(copy2));
	ASSERT_TRUE(spListElementCompare(e1, spBPQueuePeek(copy2)) == 0);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	ASSERT_TRUE(!spBPQueueIsFull(copy2));
	ASSERT_TRUE(spListElementCompare(e2, spBPQueuePeek(copy2)) == 0);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	ASSERT_TRUE(spListElementCompare(e3, spBPQueuePeek(copy2)) == 0);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	ASSERT_TRUE(spListElementCompare(e4, spBPQueuePeek(copy2)) == 0);
	ASSERT_TRUE(spBPQueueDequeue(copy2) == SP_BPQUEUE_SUCCESS);
	ASSERT_TRUE(spBPQueuePeek(copy2) == NULL);
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
	SPBPQueue queue = spBPQueueCreate(2);
	ASSERT_TRUE(0 == spBPQueueSize(queue));
	SPListElement e1 = spListElementCreate(1, 1.0);
	spBPQueueEnqueue(queue, e1);
	ASSERT_TRUE(1 == spBPQueueSize(queue));
	spListElementSetIndex(e1, 2);
	spListElementSetValue(e1, 2.0);
	spBPQueueEnqueue(queue, e1);
	ASSERT_TRUE(2 == spBPQueueSize(queue));
	spBPQueuePeek(queue);
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


static bool testBPQueuePeek() {
	SPBPQueue queue = spBPQueueCreate(0);
	ASSERT_TRUE(spBPQueuePeek(queue) == NULL);
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	SPBPQueue queue2 = quickQueue(5, 4, e2, e3, e1, e4);
	SPListElement first = spBPQueuePeek(queue2);
	ASSERT_TRUE(spListElementCompare(e1, first) == 0);
	ASSERT_TRUE(spListElementCompare(first, spBPQueuePeek(queue2)) == 0);
	spBPQueueDestroy(queue);
	spBPQueueDestroy(queue2);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

static bool testBPQueuePeekLast() {
	SPBPQueue queue = spBPQueueCreate(0);
	ASSERT_TRUE(spBPQueuePeekLast(queue) == NULL);
	SPListElement e1 = spListElementCreate(1, 1.0);
	SPListElement e2 = spListElementCreate(2, 2.0);
	SPListElement e3 = spListElementCreate(3, 3.0);
	SPListElement e4 = spListElementCreate(4, 4.0);
	SPBPQueue queue2 = quickQueue(5, 4, e2, e3, e1, e4);
	SPListElement last = spBPQueuePeekLast(queue2);
	ASSERT_TRUE(spListElementCompare(e4, last) == 0);
	ASSERT_TRUE(spListElementCompare(last, spBPQueuePeekLast(queue2)) == 0);
	spBPQueueDestroy(queue);
	spBPQueueDestroy(queue2);
	spListElementDestroy(e1);
	spListElementDestroy(e2);
	spListElementDestroy(e3);
	spListElementDestroy(e4);
	return true;
}

int main() {
	RUN_TEST(testBPQueueCreate);
	RUN_TEST(testBPQueueCopy);
	RUN_TEST(testBPQueueSize);
	RUN_TEST(testBPQueueMaxSize);
	RUN_TEST(testBPQueuePeek);
	RUN_TEST(testBPQueuePeekLast);
	return 0;
}
