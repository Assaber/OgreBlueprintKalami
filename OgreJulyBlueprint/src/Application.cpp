#include "Application.h"

int Application::join(std::function<void(void)>&& loop)
{
    while (mbEnable)
    {
        processEvents();
        loop();
    }
        
    return 0;
}

bool Application::event(QEvent* e)
{
    static bool ret = false;
    ret = super::event(e);

    if (e->type() == QEvent::Quit) {
        mbEnable = false;
    }

    return ret;
}