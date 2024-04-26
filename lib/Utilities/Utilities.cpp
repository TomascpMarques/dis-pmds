#include <Arduino.h>
#include "Utilities.h"

char *RandomCharSequenceLen32()
{
    static char id[32 + 1] = "";

    for (int i = 0; i < 33; ++i)
    {
        long choice = random(1, 5);

        if (choice <= 3)
            /* Generate random Letter from A-z */
            if (random(1, 5) <= 3)
                id[i] = random(0, 25) + 'A';
            else
                id[i] = random(0, 25) + 'a';
        else
            /* Generate random number from 0-9 */
            id[i] = random(0, 9) + '0';
    }

    id[32] = '\0';
    return id;
}

char *RandomCharSequenceLen12()
{
    static char id[12 + 1] = "";

    for (int i = 0; i < 13; ++i)
    {
        randomSeed(random(1340, 9999999));
        long choice = random(1, 5);

        if (choice <= 3)
            /* Generate random Letter from A-z */
            if (random(1, 5) <= 3)
                id[i] = random(0, 25) + 'A';
            else
                id[i] = random(0, 25) + 'a';
        else
            /* Generate random number from 0-9 */
            id[i] = random(0, 9) + '0';
    }

    id[12] = '\0';
    return id;
}

char *RandomCharSequenceLen8()
{
    static char id[8 + 1] = "";

    for (int i = 0; i < 8; ++i)
    {
        long choice = random(1, 5);

        if (choice <= 3)
            /* Generate random Letter from A-z */
            if (random(1, 5) <= 3)
                id[i] = random(0, 25) + 'A';
            else
                id[i] = random(0, 25) + 'a';
        else
            /* Generate random number from 0-9 */
            id[i] = random(0, 9) + '0';
    }

    id[8] = '\0';
    return id;
}