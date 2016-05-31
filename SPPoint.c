#include "SPPoint.h"
#include <stdlib.h>
#include <assert.h>

//TODO - choose a convention regarding null comparison
//TODO - check if we should remove our assertions
struct sp_point_t {
	double* data;
	int dim;
	int index;
};

/*
 * creates a new double array with the same values of the given array
 * @data - the source array to be copied
 * @size - the size of the array
 */
double* copyData(double* data, int size)
{
	assert(data != NULL);
	int i;
	double* newData = (double*)calloc(size,sizeof(double));

	if (!newData)
		return NULL;

	for (i=0;i<size;i++)
		newData[i] = data[i];

	return newData;
}

SPPoint spPointCreate(double* data, int dim, int index){
	if (!data) //data is null
		return NULL;
	if (dim <= 0) //dimension illegal
		return NULL;
	if (index <0) //index illegal
		return NULL;
	SPPoint item = (SPPoint)calloc(1,sizeof(struct sp_point_t));
	if (!item) // allocation error
		return NULL;

	item->data = copyData(data,dim);
	if (item->data == NULL)
		return NULL; //allocation error
	item->dim = dim;
	item->index = index;

	return item;
}

SPPoint spPointCopy(SPPoint source){
	assert (source != NULL);
	double* dataCopy = copyData(source->data, source->dim);
	if (!dataCopy)
		return NULL;
	return spPointCreate(dataCopy,source->dim,source->index);
}

void spPointDestroy(SPPoint point)
{
	if (point != NULL)
	{
		if (point->data != NULL){
			free(point->data);
			point->data = NULL;
		}
		free(point);
		point = NULL;
	}
}

int spPointGetDimension(SPPoint point)
{
	assert(point != NULL);
	return point->dim;
}

int spPointGetIndex(SPPoint point)
{
	assert(point != NULL);
	return point->index;
}

double spPointGetAxisCoor(SPPoint point, int axis)
{
	assert(point != NULL && axis < point->dim);
	return point->data[axis];
}

double spPointL2SquaredDistance(SPPoint p, SPPoint q){
	assert(p != NULL && q != NULL && p->dim == q->dim);

	int dimIndex;
	double l2Dist, currentDist;
	l2Dist = 0;

	for (dimIndex = 0;dimIndex < p->dim ; dimIndex++){
		currentDist = p->data[dimIndex]-q->data[dimIndex];
		l2Dist += currentDist * currentDist;
		assert (l2Dist >= 0); // verify that no overflow occurred
	}

	return l2Dist;
}
