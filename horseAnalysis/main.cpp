#include <windows.h> // only for real-time playback //todo: DELETE only debug

#include "step.h"
#include "steps.h"
#include "vector3d.h"
#include "horseanalysis.h"

int main(int argc, char* argv[])
{
    //char aa;scanf("%c", &aa); // wait for synchronous start
    FILE* f;
    if(argc > 1)
        f = fopen(argv[1], "r");
    else
        f = fopen("1acc.csv", "r");

    size_t frequency = 100;
    if(argc > 2)
        frequency = atoi(argv[2]);
    HorseAnalysis horse(frequency);

    while(getc(f) != '\n' && !feof(f));
    while(!feof(f))
    {
        for(int i=0; i<2 && !feof(f); i+=(getc(f)==','));
        float a, b, c = 0;
        fscanf(f, "%f,%f,%f", &a, &b, &c);
        horse.addData(a, b, c, 0, 0, 0);
        if(horse.elapsedTime() % 1000 == 0)
        {
            printf("Second %4d: moving %s, steps %4i, %s, ...\n",
                   horse.elapsedTime()/1000,
                   horse.isMoving() ? "yes": "no ",
                   horse.numSteps(),
                   horse.detectAndNameMovement().c_str()
            );
            //Sleep(1000); //todo: DELETE only debug with real-time video
        }
    }
    fclose(f);

    // show results in graph
    system("gnuplot -persistent -e \"\
        plot 'out.csv' using 0:1 with lines, \
        '' using 0:2 with lines, \
        '' using 0:3 with lines, \
        '' using 0:6 with lines, \
        '' using 0:7 with lines lt -1, \
        '' using 0:8 with lines, \
    \"");
    return 0;
}
