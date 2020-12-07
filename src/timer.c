/*#include <time.h>
#include <stdio.h>

int timer(double x) {
    clock_t start_t, end_t, total_t;
    int i;

    start_t = clock();
    end_t = 0;
    for(i=0; i< 1000000000; i++) {
        end_t = clock();
        if(((end_t - start_t) / CLOCKS_PER_SEC) > x) break;
    }
    return(1);
}
*/