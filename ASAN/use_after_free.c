#include <stdio.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
    int *nums = (int *)malloc(10 * sizeof(int));

    free(nums);

    nums[1] = 100;

    return 0;
}
