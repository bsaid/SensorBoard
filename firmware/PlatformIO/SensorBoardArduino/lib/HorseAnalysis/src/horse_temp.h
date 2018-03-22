#include "step.h"
#include "steps.h"
#include "vector3d.h"
#include "horseanalysis.h"

int deprecated_main()
{
    //char aa;scanf("%c", &aa); // wait for synchronous start
    FILE* f;
    f = fopen("1acc.csv", "r");

    size_t frequency = 100;
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

    return 0;
}
