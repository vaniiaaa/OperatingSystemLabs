#include <gtest/gtest.h>
#include "../MultiThreads.hpp"

TEST(IntegrationTests, TestOneMarker)
{
    MarkerRunner runner(10, 1);
    runner.StartMarkers();
    runner.WaitForAllMarkers();
    std::srand(1);
    int i = runner.GetNumOfMarked();
    while (i > 0)
    {
        int index = rand() % 10;
        EXPECT_EQ(runner.GetArrayValue(index), 1) << "Element [" << index << "] not marked by marker 1";
        i--;
    }
}

TEST(IntegrationTests, TestOneMarkerClear)
{
    MarkerRunner runner(10, 1);
    runner.StartMarkers();
    runner.WaitForAllMarkers();
    runner.TerminateMarker(1);
    for (int i = 0; i < runner.GetSize(); i++)
    {
        EXPECT_EQ(runner.GetArrayValue(i), 0) << "Element [" << i << "] not cleared";
    }
}

TEST(ConcurrencyTests, Test10Markers)
{
    MarkerRunner runner(20, 10);
    runner.StartMarkers();
    runner.WaitForAllMarkers();
    //EXPECT_EQ(runner.GetNumOfMarked(), 20) << "Not all elements were marked " << runner.GetNumOfMarked();
    for (int i = 0; i < 20; i++) {
        int value = runner.GetArrayValue(i);
        EXPECT_GE(value, 0) << "Element [" << i << "] = " << value << " less 0";
        EXPECT_LE(value, 10) << "Element [" << i << "] = " << value << " greater 10";
    }
}

TEST(ConcurrencyTests, Test10MarkersTerminated)
{
    MarkerRunner runner(30, 5);
    int i = 1;
    while (i <= 5)
    {
        runner.StartMarkers();
        runner.WaitForAllMarkers();
        runner.TerminateMarker(i);
        EXPECT_TRUE(runner.IsMarkerTerminated(i)) << "Marker " << i << " not terminated";
        for (int j = 0; j < 30; j++)
        {
            int value = runner.GetArrayValue(j);
            EXPECT_NE(value, i) << "Element [" << j << "] = " << value << " equal to terminated marker " << i;
        }
        i++;
        runner.ContinueMarkers();
    }
    
}