#include "unit_test_util.h" //SUPPORTING MACROS ASSERT_TRUE/ASSERT_FALSE etc..
#include "../SPLogger.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


// This is a helper function which checks if two files are identical
static bool identicalFiles(const char* fname1, const char* fname2) {
	FILE *fp1, *fp2;
	fp1 = fopen(fname1, "r");
	fp2 = fopen(fname2, "r");
	char ch1 = EOF, ch2 = EOF;

	if (fp1 == NULL) {
		return false;
	} else if (fp2 == NULL) {
		fclose(fp1);
		return false;
	} else {
		ch1 = getc(fp1);
		ch2 = getc(fp2);

		while ((ch1 != EOF) && (ch2 != EOF) && (ch1 == ch2)) {
			ch1 = getc(fp1);
			ch2 = getc(fp2);
		}
		fclose(fp1);
		fclose(fp2);
	}
	if (ch1 == ch2) {
		return true;
	} else {
		return false;
	}
}

//Logger is not defined
static bool basicLoggerTest() {
	ASSERT_TRUE(spLoggerPrintError("A", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);
	spLoggerDestroy();
	return true;
}

//Only Errors should be printed
static bool basicLoggerErrorTest() {
	const char* expectedFile = "basicLoggerErrorTestExp.log";
	const char* testFile = "basicLoggerErrorTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}

//All messages should be printed in debug level
static bool basicLoggerDebugTest() {
	const char* expectedFile = "basicLoggerDebugTestExp.log";
	const char* testFile = "basicLoggerDebugTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}

//Only Errors and Warnings should be printed
static bool basicLoggerWarningTest() {
	const char* expectedFile = "basicLoggerWarningTestExp.log";
	const char* testFile = "basicLoggerWarningTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}

//All messages but Debug messages should be printed in Info level
static bool basicLoggerInfoTest() {
	const char* expectedFile = "basicLoggerInfoTestExp.log";
	const char* testFile = "basicLoggerInfoTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}

//Check logger-undefined error in all of the print functions
static bool basicLoggerUndefinedTest() {
	ASSERT_TRUE(spLoggerPrintError("MSGA", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_UNDIFINED);
	ASSERT_TRUE(spLoggerPrintMsg("MSGE") == SP_LOGGER_UNDIFINED);
	return true;
}

//Check logger-invalid-argument error in all of the print functions
static bool basicLoggerInvalidArgumentsTest() {
	const char* testFile = "basicLoggerTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA", NULL, __func__, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c", NULL, __LINE__) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintInfo(NULL) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c", __func__, -1) == SP_LOGGER_INVAlID_ARGUMENT);
	ASSERT_TRUE(spLoggerPrintMsg(NULL) == SP_LOGGER_INVAlID_ARGUMENT);
	spLoggerDestroy();
	return true;
}

//Check spLoggerPrintMsg function
static bool basicLoggerPrintMsgTest() {
	const char* expectedFile = "basicLoggerPrintMsgTestExp.log";
	const char* testFile = "basicLoggerPrintMsgTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("PrintMsg1") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintWarning("MSGB", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintInfo("MSGC") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("MSGA", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("PrintMsg2") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("PrintMsg3") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("PrintMsg4") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("MSGD", "sp_logger_unit_test.c", __func__, __LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("PrintMsg5") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintDebug("","sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}

//Check how we deal with long lines and new lines
static bool basicLoggerLinesTest() {
	const char* expectedFile = "basicLoggerLinesTestExp.log";
	const char* testFile = "basicLoggerLinesTest.log";
	ASSERT_TRUE(spLoggerCreate(testFile, SP_LOGGER_ERROR_LEVEL) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("This is a pretty long sentence that we wish to add to the log in order "
			"to see how the logger handles it.") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("This is an error message that contains new line in the end\n",
			"sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintMsg("This sentence contains a new line in the end and we wish to check how the "
			"logger handles this kind of messages.\n") == SP_LOGGER_SUCCESS);
	ASSERT_TRUE(spLoggerPrintError("Yet another error message that contains a new line in the end\n",
				"sp_logger_unit_test.c",__func__,__LINE__) == SP_LOGGER_SUCCESS);
	spLoggerDestroy();
	ASSERT_TRUE(identicalFiles(testFile,expectedFile));
	return true;
}
/*
int main() {
	RUN_TEST(basicLoggerTest);
	RUN_TEST(basicLoggerErrorTest);
	RUN_TEST(basicLoggerDebugTest);
	RUN_TEST(basicLoggerWarningTest);
	RUN_TEST(basicLoggerInfoTest);
	RUN_TEST(basicLoggerUndefinedTest);
	RUN_TEST(basicLoggerInvalidArgumentsTest);
	RUN_TEST(basicLoggerPrintMsgTest);
	RUN_TEST(basicLoggerLinesTest);
	return 0;
}

*/
