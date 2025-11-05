#ifndef RUNNER_HPP
#define RUNNER_HPP

#include "MultiThreads.hpp"

class InvalidBoundInput : public std::exception
{
};
class InvalidSecondInput : public std::exception
{
};

void InputController(int &input, int lower, int upper, HANDLE *MainSignals)
{
    while (true)
    {
        try
        {
            std::cin >> input;
            if (lower >= input || input > upper)
                throw InvalidBoundInput();
            if (WaitForSingleObject(MainSignals[input - 1], 0) == WAIT_OBJECT_0)
                throw InvalidSecondInput();
            break;
        }
        catch (const InvalidBoundInput &e)
        {
            std::cout << "Value have to be between " << lower + 1 << " and " << upper << '\n';
        }
        catch (const InvalidSecondInput &e)
        {
            std::cout << "Marker " << input << " already terminated" << '\n';
        }
    }
}

void Runner(int size, int nummark)
{
    MarkerRunner runner(size, nummark);
    while(!runner.AllMarkersTerminated())
    {
        runner.StartMarkers();
        runner.WaitForAllMarkers();
        int markerToTerminate;
        runner.PrintArray();
        std::cout << "Enter marker number to terminate: ";
        InputController(markerToTerminate, 0, nummark, runner.GetMainSignals());
        runner.TerminateMarker(markerToTerminate);
        runner.PrintArray();
        runner.ContinueMarkers();
    }
}
#endif RUNNER_HPP