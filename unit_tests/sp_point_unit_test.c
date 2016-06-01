#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define epsilon 0.00001

//Checks if copy Works
bool pointBasicCopyTest() {
	int i;
	double data[2] = { 1.0, 1.0 };
	int dim = 2;
	int index = 1;
	SPPoint p = spPointCreate(data, dim, index);
	SPPoint q = spPointCopy(p);
	ASSERT_TRUE(spPointGetIndex(p) == spPointGetIndex(q));
	ASSERT_TRUE(spPointGetDimension(p) == spPointGetDimension(q));
	for (i = 0; i < spPointGetDimension(p); i++) {
		ASSERT_TRUE(spPointGetAxisCoor(p, i) == spPointGetAxisCoor(q, i));
	}
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}
bool pointBasicL2Distance() {
	double data1[2] = { 1.0, 1.0 };
	double data2[2] = { 1.0, 0.0 };
	int dim1 = 2;
	int dim2 = 2;
	int index1 = 1;
	int index2 = 1;
	SPPoint p = spPointCreate((double *)data1, dim1, index1);
	SPPoint q = spPointCreate((double *)data2, dim2, index2);
	ASSERT_TRUE(spPointL2SquaredDistance(p,p) == 0.0);
	ASSERT_TRUE(spPointL2SquaredDistance(q,q) == 0.0);
	ASSERT_FALSE(spPointL2SquaredDistance(p,q) == 0.0);
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}

bool pointCreateInvalidArgumentsTest(){
	SPPoint p;
	double data[4] = { 1.0, 2.0 , 5.5 , 60 };
	int dim, index;

	dim = 5;
	index = 4;
	p = spPointCreate(NULL,dim,index);
	ASSERT_TRUE(p == NULL);
	spPointDestroy(p);

	dim = 0;
	index = 4;
	p = spPointCreate((double*)data,dim,index);
	ASSERT_TRUE(p == NULL);
	spPointDestroy(p);

	dim = 4;
	index = -1;
	p = spPointCreate((double*)data,dim,index);
	ASSERT_TRUE(p == NULL);
	spPointDestroy(p);
	return true;
}

bool pointBasicL2Distance2() {
	double data1[4] = { 1.0, 2.0 , 5.5 , 60 };
	double data2[4] = { 1.0, 0.0 , 0.0 , 6.8 };
	int dim1 = 4;
	int dim2 = 4;
	int index1 = 1;
	int index2 = 1;
	SPPoint p = spPointCreate((double *)data1, dim1, index1);
	SPPoint q = spPointCreate((double *)data2, dim2, index2);
	ASSERT_TRUE(spPointL2SquaredDistance(p,q) - 2864.49 < epsilon &&
			spPointL2SquaredDistance(p,q) - 2864.49 > -epsilon);
	spPointDestroy(p);
	spPointDestroy(q);
	return true;
}

bool pointCopyTest() {
	int i;
	double data[4] = { 2.2, 11.99 , 1.0 , 5 };
	int dim = 4;
	int index = 5;
	SPPoint p1 = spPointCreate(data, dim, index);
	SPPoint p2 = spPointCopy(p1);
	SPPoint p3 = spPointCopy(p1);
	SPPoint p4 = spPointCopy(p2);


	ASSERT_TRUE(spPointGetDimension(p1) == dim
			&& spPointGetDimension(p2) == dim
			&& spPointGetDimension(p3) == dim
			&& spPointGetDimension(p4) == dim );


	ASSERT_TRUE(spPointGetIndex(p1) == index
			&& spPointGetIndex(p2) == index
			&& spPointGetIndex(p3) == index
			&& spPointGetIndex(p4) == index );


	for (i = 0; i < dim ; i++) {
		ASSERT_TRUE(spPointGetAxisCoor(p1, i) == data[i]);
		ASSERT_TRUE(spPointGetAxisCoor(p2, i) == data[i]);
		ASSERT_TRUE(spPointGetAxisCoor(p3, i) == data[i]);
		ASSERT_TRUE(spPointGetAxisCoor(p4, i) == data[i]);
	}

	spPointDestroy(p2);
	spPointDestroy(p1);
	spPointDestroy(p3);
	spPointDestroy(p4);
	return true;
}

SPPoint getRandomPoint(int dim)
{
	int i;
	SPPoint p;
	double* data = (double*)calloc(dim, sizeof(double));
	for (i=0 ;i< dim ; i++)
	{
		data[i] = ((double)rand()/((double)RAND_MAX/100));
	}
	p = spPointCreate(data,dim,1);
	free(data);
	return p;
}


bool pointTestDistanceNotNegative()
{
	int dim , i;
	double p1p2;
	SPPoint p1,p2;

	for (i=0 ; i < 100 ; i++){
		dim = 1 + (int)(rand() % 15);
		p1 = getRandomPoint(dim);
		p2 = getRandomPoint(dim);

		p1p2 = spPointL2SquaredDistance(p1,p2);

		ASSERT_TRUE(p1p2 >= 0);

		spPointDestroy(p2);
		spPointDestroy(p1);
	}
	return true;
}


bool pointTestDistanceSymetric()
{
	int dim , i;
	double p1p2,p2p1;
	SPPoint p1,p2;

	for (i=0 ; i < 100 ; i++){
		dim = 1 + (int)(rand() % 15);
		p1 = getRandomPoint(dim);
		p2 = getRandomPoint(dim);

		p1p2 = spPointL2SquaredDistance(p1,p2);
		p2p1 = spPointL2SquaredDistance(p2,p1);

		ASSERT_TRUE(p1p2 - p2p1 <= epsilon && p1p2 - p2p1 >= -epsilon);

		spPointDestroy(p2);
		spPointDestroy(p1);
	}
	return true;
}

bool pointTestTriangleInequality()
{
	int dim , i;
	double p1p2,p1p3,p3p2;
	SPPoint p1,p2,p3;

	for (i=0 ; i < 100 ; i++){
		dim = 1 + (int)(rand() % 15);
		p1 = getRandomPoint(dim);
		p2 = getRandomPoint(dim);
		p3 = getRandomPoint(dim);

		p1p2 = sqrt(spPointL2SquaredDistance(p1,p2));
		p1p3 = sqrt(spPointL2SquaredDistance(p1,p3));
		p3p2 = sqrt(spPointL2SquaredDistance(p3,p2));

		ASSERT_TRUE(p1p2 <= p1p3 + p3p2 + epsilon);
		ASSERT_TRUE(p1p3 <= p1p2 + p3p2 + epsilon);
		ASSERT_TRUE(p3p2 <= p1p2 + p1p3 + epsilon);

		spPointDestroy(p2);
		spPointDestroy(p1);
		spPointDestroy(p3);
	}
	return true;
}

bool pointDestroyInvalidArgumentsTest()
{
	spPointDestroy(NULL);
	return true;
}
/*
int main() {
	RUN_TEST(pointBasicCopyTest);
	RUN_TEST(pointBasicL2Distance);

	RUN_TEST(pointCopyTest);
	RUN_TEST(pointBasicL2Distance2);
	RUN_TEST(pointCreateInvalidArgumentsTest);
	RUN_TEST(pointDestroyInvalidArgumentsTest);

	RUN_TEST(pointTestTriangleInequality);
	RUN_TEST(pointTestDistanceSymetric);
	RUN_TEST(pointTestDistanceNotNegative);

	return 0;
}
*/
