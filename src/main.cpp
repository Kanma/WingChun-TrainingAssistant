#include "Application.h"


int main(int argc, char** argv)
{
    Application application;

    if (!application.init(argc, argv))
        return 1;

    application.run();

    return 0;
}
