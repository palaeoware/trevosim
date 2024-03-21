#ifndef TESTSUITE_H
#define TESTSUITE_H

#include <gtest/gtest.h>

class testsuite: public ::testing::Test
{
protected:
    testsuite();
};

#endif // TESTSUITE_H
