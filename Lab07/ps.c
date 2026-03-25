#include "types.h"
#include "stat.h"
#include "user.h"

int main(void)
{
    if(getpinfo() < 0){
        printf(1, "Error: getpinfo failed\n");
        exit();
    }

    exit();
}

