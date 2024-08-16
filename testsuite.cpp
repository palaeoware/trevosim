#include "testsuite.h"
#include "testinternal.h"

// This is the main function called by the TREvoSimTest application
int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    int rtr = RUN_ALL_TESTS();

    qInfo() << "Testing complete. Press any key to exit...";

    // Qt Solution for waiting for a key input
    QTextStream in(stdin);
    forever
    {
        QString line = in.readLine();
        if (!line.isNull())
        {
            break;
        }
    }

    return rtr;
}

// Class testsuite
testsuite::testsuite()
{
}

TEST_F(testsuite, TREvoSimTestZero) {
    QString output;
    testinternal internalTestClass(nullptr);
    bool result = internalTestClass.callTest(0, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestOne) {
    QString output;
    testinternal internalTestClass(nullptr);
    bool result = internalTestClass.callTest(1, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestTwo) {
    QString output;
    testinternal internalTestClass(nullptr);
    bool result = internalTestClass.callTest(2, output);
    ASSERT_TRUE(result) << output.toStdString();

}

TEST_F(testsuite, TREvoSimTestThree) {
    QString output;
    testinternal internalTestClass(nullptr);
    bool result = internalTestClass.callTest(3, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestFour) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(4, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestFive) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(5, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestSix) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(6, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestSeven) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(7, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestEight) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(8, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestNine) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(9, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestTen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(10, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestEleven) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(11, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestTwelve) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(12, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestThirteen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(13, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestForteen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(14, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestFifteen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(15, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestSixteen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(16, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestSeventeen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(17, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestEighteen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(18, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestNineteen) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(19, output);
    ASSERT_TRUE(result) << output.toStdString();
}

TEST_F(testsuite, TREvoSimTestTwenty) {
    QString output;
    MainWindow *mainWindow = nullptr;
    testinternal internalTestClass(mainWindow);
    bool result = internalTestClass.callTest(20, output);
    ASSERT_TRUE(result) << output.toStdString();
}
