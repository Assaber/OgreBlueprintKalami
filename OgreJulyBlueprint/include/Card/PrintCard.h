#pragma once
#include "container/BKCard.h"

class PrintCard : public BKCard
{
    CREATE_CARD_FACTORY(PrintCard)
public:
    PrintCard();
};