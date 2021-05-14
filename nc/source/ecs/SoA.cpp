#include "SoA.h"

namespace nc::ecs
{
    SoAIndex::SoAIndex(size_t begin, size_t end, GapIterator gapBegin, GapIterator gapEnd)
        : m_current{begin},
          m_end{end},
          m_gapCurrent{gapBegin},
          m_gapEnd{gapEnd}
    {
    }

    SoAIndex::operator size_t() const
    {
        return m_current;
    }

    bool SoAIndex::Valid() const
    {
        return m_current < m_end;
    }

    SoAIndex& SoAIndex::operator++()
    {
        while(++m_current < m_end)
        {
            if(m_gapCurrent != m_gapEnd && m_current == *m_gapCurrent)
            {
                ++m_gapCurrent; continue;
            }

            break;
        }

        return *this;
    }
} // namespace nc::ecs