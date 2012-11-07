/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  07/30/2000 12:00pm
       filename :  flags.cpp
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Object used to toggle and test bits in a flag value
*********************************************************************/

#include "flags.h"

using namespace ssobjects;

Flags::Flags(unsigned32 nInitiallySet)
        : m_nFlags(nInitiallySet)
{
}

Flags::~Flags()
{
    return;
}

unsigned32 Flags::toggle(unsigned32 nFlagsSetting,unsigned32 nFlagsResetting)
{
    m_nFlags&=~nFlagsResetting;
    m_nFlags|=nFlagsSetting;
    return m_nFlags;
}

bool Flags::isSet(unsigned32 nCheckFlags)
{
    return m_nFlags&nCheckFlags ? true:false;
}

