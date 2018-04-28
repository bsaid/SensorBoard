#ifndef STEP_H
#define STEP_H

#include <list>

class Step
{
public:
    size_t time = 0;
    double amplitude = 0;
    std::list< std::pair<size_t, double> > peaks;

    Step(size_t time = 0, double amplitude = 0)
        : time(time), amplitude(amplitude) {}

    bool started()
    {
        return !peaks.empty();
    }

    void reset()
    {
        time = 0;
        amplitude = 0;
        peaks.clear();
    }

    void addPeak(size_t time, double value)
    {
        peaks.push_back( std::pair<size_t, double>(time, value) );
        if(amplitude < value)
            amplitude = value;
    }
};

#endif // STEP_H
