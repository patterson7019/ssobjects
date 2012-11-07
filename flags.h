/********************************************************************
       Copyright (c) 2006, Lee Patterson & Ant Works Software
       http://ssobjects.sourceforge.net

       created  :  07/30/2000 12:00pm
       filename :  flags.h
       author   :  Lee Patterson (workerant@users.sourceforge.net)

       purpose  :  Object used to toggle and test bits in a flag value
*********************************************************************/

#ifndef FLAGS_H
#define FLAGS_H

#include "ssobjects.h"

namespace ssobjects
{

class Flags
{
    public:
        enum
            {
            F_NONE=0,
            F_ALL=0xFFFFFFFF
            };

    protected:
        unsigned32 m_nFlags;

    public:
        Flags(unsigned32 nInitiallySet=0);
        virtual ~Flags();
        unsigned32 toggle(unsigned32 nFlagsSetting,unsigned32 nFlagsResetting=F_NONE);
        bool isSet(unsigned32 nCheckFlags);
        unsigned32 get() {return m_nFlags;}
};

};

#endif

