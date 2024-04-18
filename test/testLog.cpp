#include "../log/logger.h"

int main()
{
    Logger logger("log.log");
    logger.launch();

    while (true)
    {
        logger.log("hello world\n");
        usleep(100 * 1000);
    }
}