#ifndef STEPS_H
#define STEPS_H

#include <list>

class Steps
{
    const static size_t MEMORY_SIZE = 10;
    std::list<Step> m_steps;
    size_t numSteps = 0;

public:
    void addStep(Step step)
    {
        m_steps.push_front(step);
        if(m_steps.size() > MEMORY_SIZE)
            m_steps.pop_back();
        numSteps++;
    }

    size_t lastTimeBetweenSteps()
    {
        if(m_steps.size() < 2)
            return 0;
        std::list<Step>::iterator it = m_steps.begin();
        Step s2 = *(it++);
        Step s1 = *(it++);
        return s2.time - s1.time;
    }

    size_t getNumSteps()
    {
        return numSteps;
    }

    Step getLastStep(size_t nth)
    {
        if(m_steps.size() < nth+1)
            return Step();
        std::list<Step>::iterator it = m_steps.begin();
        for(size_t i=0; i<nth; i++) it++;
        return *it;
    }

    std::list<Step> getSteps()
    {
        return m_steps;
    }
};

#endif // STEPS_H
