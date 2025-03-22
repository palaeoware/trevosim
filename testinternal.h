#ifndef TESTINTERNAL_H
#define TESTINTERNAL_H

#include "mainwindow.h"

class testinternal
{
public:
    testinternal(MainWindow *theMainWindowCon);
    bool callTest(int testNumber, QString &outString);
    QString testDescription(int testNumber);

private:
    bool testZero(QString &outString);
    bool testOne(QString &outString);
    bool testTwo(QString &outString);
    bool testThree(QString &outString);
    bool testFour(QString &outString);
    bool testFive(QString &outString);
    bool testSix(QString &outString);
    bool testSeven(QString &outString);
    bool testEight(QString &outString);
    bool testNine(QString &outString);
    bool testTen(QString &outString);
    bool testEleven(QString &outString);
    bool testTwelve(QString &outString);
    bool testThirteen(QString &outString);
    bool testFourteen(QString &outString);
    bool testFifteen(QString &outString);
    bool testSixteen(QString &outString);
    bool testSeventeen(QString &outString);
    bool testEighteen(QString &outString);
    bool testNineteen(QString &outString);
    bool testTwenty(QString &outString);
    bool testTwentyOne(QString &outString);
    bool testTwentyTwo(QString &outString);
    bool testTwentyThree(QString &outString);
    bool testTwentyFour(QString &outString);
    bool testTwentyFive(QString &outString);

    MainWindow *theMainWindow;
    bool error;
    QMap<int, QString> testList;
};

#endif // TESTINTERNAL_H
