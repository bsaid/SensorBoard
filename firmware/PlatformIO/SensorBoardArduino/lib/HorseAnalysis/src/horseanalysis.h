#ifndef HORSEANALYSIS_H
#define HORSEANALYSIS_H

#include <stdio.h>
#include <string>

#include "step.h"
#include "steps.h"
#include "vector3D.h"

#define Q_UNUSED(x) (void)x;

class HorseAnalysis
{
    //debug only
    FILE *output = fopen("out.csv", "w");

    const static size_t MEMORY_TIME = 5; // 5 seconds
    size_t m_sampleFreq;

    size_t m_time = 0;
    Vector3D m_accOffsets;
    std::list<Vector3D> m_last;
    Vector3D m_lastMax;
    size_t m_lastZeroTime = 0;
    size_t m_lastPeakTime = 0;
    Step m_lastStep;
    Step m_actualStep;
    Steps m_steps;
    int m_actualMovement = 0;
    std::list<int> m_lastMovements;
    size_t m_timeUnderZero = 0;

    void updateTime()
    {
        m_time += (1000/m_sampleFreq);
    }

    double actualValue(Vector3D acc)
    {
        return (acc-m_accOffsets).axisValue(m_lastMax);
    }

    double actualValue()
    {
        Vector3D acc = *(m_last.begin());
        return (acc-m_accOffsets).axisValue(m_lastMax);
    }

    double actualValue(int historyIndex)
    {
        std::list<Vector3D>::iterator it = m_last.begin();
        for(int i=0; i<-historyIndex; i++) it++;
        return actualValue(*it);
    }

    bool isRisingEdge()
    {
        std::list<Vector3D>::iterator it = m_last.begin();
        double s2 = actualValue(*(it++));
        double s1 = actualValue(*(it++));
        return s1 < s2;
    }

    void updateLastData(Vector3D acc, int actualMovement)
    {
        m_last.push_front(acc);
        if(m_last.size() > m_sampleFreq*MEMORY_TIME)
            m_last.pop_back();
        m_lastMovements.push_front(actualMovement);
        if(m_lastMovements.size() > m_sampleFreq*2)  // *2 seconds
            m_lastMovements.pop_back();
    }

    void updateZeroOffsets()
    {
        Vector3D accSum;
        for(Vector3D i : m_last)
            accSum += i;
        m_accOffsets = accSum / (m_last.size());
    }

    void updateTimeUnderZero()
    {
        static int latency = 0;
        if(actualValue() > 0)
        {
            latency++;
            if(latency>1)
                m_timeUnderZero=0;
            else
                m_timeUnderZero+=(1000/m_sampleFreq);
        }
        else
        {
            m_timeUnderZero+=(1000/m_sampleFreq);
            latency=0;
        }
    }

    void updateStatistics()
    {
        m_lastMax = 0;
        std::list<Vector3D>::iterator it=m_last.begin();
        for(size_t i=0; i<m_sampleFreq && it!=m_last.end(); i++)
        {
            if(m_lastMax.size() < it->size())
                m_lastMax = *it;
            it++;
        }
        updateTimeUnderZero();
    }

    bool isZeroAcc()
    {
        std::list<Vector3D>::iterator it = m_last.begin();
        double s2 = actualValue(*(it++));
        double s1 = actualValue(*(it++));
        return (s1 >= 0 && s2 <= 0) || (s1 <= 0 && s2 >= 0);
    }

    bool isPeak()
    {
        std::list<Vector3D>::iterator it = m_last.begin();
        double s3 = actualValue(*(it++));
        double s2 = actualValue(*(it++));
        double s1 = actualValue(*(it++));
        return (s1 < s2 && s2 >= s3); // detect start time of the peak
    }

    void detectStep()
    {
        const size_t MIN_STEP_TIME = 200;      // ms
        const size_t MIN_FLY_TIME = 100;       // ms
        const double MIN_STEP_AMPLITUDE = 0.3; // G

        if(
            m_actualStep.started() &&
            isZeroAcc() &&
            m_timeUnderZero > MIN_FLY_TIME &&
            elapsedTime()-m_actualStep.peaks.begin()->first > MIN_STEP_TIME
        )
        {
            double highestAmplitude = 0;
            size_t time = 0;
            for(std::pair<size_t, double> peak : m_actualStep.peaks)
            {
                if(peak.second > highestAmplitude)
                {
                    highestAmplitude = peak.second;
                    time = peak.first;
                }
            }
            m_actualStep.time = time;
            m_actualStep.amplitude = highestAmplitude;
            if(
                m_actualStep.peaks.empty() ||
                m_actualStep.time - m_lastStep.time < MIN_STEP_TIME ||
                m_actualStep.amplitude < MIN_STEP_AMPLITUDE
            )
            {
                m_actualStep.reset();
                return;
            }

            m_steps.addStep(m_actualStep);
            m_lastStep = m_actualStep;
            m_actualStep.reset();
            return;
        }

        if(
            isMoving() &&
            elapsedTime() - m_lastStep.time > MIN_STEP_TIME &&
            m_lastZeroTime > m_lastStep.time &&
            isPeak()
        )
        {
            m_actualStep.addPeak(m_time, actualValue(-1));
        }
    }

    int getIndexOfMaxValue(int * arr, int size)
    {
        int maxCount = 0;
        int output = 0;
        for(int i=0; i<size; i++)
        {
            if(arr[i] > maxCount)
            {
                maxCount = arr[i];
                output = i;
            }
        }
        return output;
    }

public:
    HorseAnalysis(size_t sampleFreq) : m_sampleFreq(sampleFreq){}
    ~HorseAnalysis()
    {
        //debug only
        fclose(output);
    }

    void addData(double ax, double ay, double az, double gx, double gy, double gz)
    {
        Q_UNUSED(gx);
        Q_UNUSED(gy);
        Q_UNUSED(gz);

        updateTime();
        updateLastData(Vector3D(ax, ay, az), m_actualMovement);
        updateZeroOffsets();
        updateStatistics();
        if(isZeroAcc())
            m_lastZeroTime = m_time;

        detectStep();
        m_actualMovement = detectMovement();

        //debug only
        fprintf(output, "%f %f %d %f %f %f %d %f %f %f %f\n",
                actualValue(),
                m_last.begin()->size(),
                isMoving(),
                m_steps.lastTimeBetweenSteps()/100.0,
                m_steps.getNumSteps()/30.0,
                m_lastStep.amplitude,
                m_actualMovement,
                -((double)m_timeUnderZero/100.0),
                ax, ay, az
        );
    }

    bool isMoving()
    {
        // 0.3 - minimal acc amplitude needed to move
        // 100 - startup time in milliseconds, no detection possible
        return (m_time > 100) && (m_lastMax-m_accOffsets).axisValue(m_lastMax) > 0.3;
    }

    size_t elapsedTime() /* milliseconds */
    {
        return m_time;
    }

    size_t numSteps()
    {
        return m_steps.getNumSteps();
    }

    int detectMovement()
    {
        if(!isMoving())
            return 0;

        int prob[5];
        for(int i=0; i<5; i++)
            prob[i] = 0;

        ///<begin> definitions

        prob[m_actualMovement]++;

        // 400 - magic number of milliseconds when
        //       the horse cannot run and make steps with frequency lower than this number
        if(m_time - m_lastStep.time > 400)
            prob[1]+=1;

        if(m_lastStep.amplitude < 2.5)
            prob[1]+=2;

        if(m_time - m_lastStep.time < 500)
            prob[2]+=1;

        if(m_lastStep.amplitude > 2.0)
            prob[2]+=1;

        if(m_lastStep.amplitude > 5)
        {
            prob[2]+=2;
            prob[3]+=3;
        }

        if(m_lastStep.amplitude > 10)
            prob[3]+=2;

        if(abs(m_steps.getLastStep(0).amplitude - m_steps.getLastStep(1).amplitude) > 5)
            prob[3]+=5;

        ///<end> definitions

        return getIndexOfMaxValue(prob, 5);
    }

    int getLastSecondMovement()
    {
        int prob[5];
        for(int i=0; i<5; i++)
            prob[i] = 0;

        // computed from records from the last 2 seconds
        // if the new value is present longer than 1 second, the modus value has majority
        for(int m : m_lastMovements)
        {
            switch(m)
            {
                case 0: prob[0]++; break;
                case 1: prob[1]++; break;
                case 2: prob[2]++; break;
                case 3: prob[3]++; break;
                default: prob[4]++; break;
            }
        }

        //printf("%i %i %i %i %i >> ", prob[0], prob[1], prob[2], prob[3], prob[4]);
        return getIndexOfMaxValue(prob, 5);
    }

    const std::string detectAndNameMovement()
    {
        switch(getLastSecondMovement())
        {
        case 0:
            return "stani"; break;
        case 1:
            return "krok "; break;
        case 2:
            return "klus "; break;
        case 3:
            return "cval "; break;
        default:
            return "nevim"; break;
        }
    }
};

#endif // HORSEANALYSIS_H
