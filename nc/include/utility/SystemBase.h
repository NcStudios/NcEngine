#pragma once

namespace nc
{
    /** Enforces pointer stability for derived types. */
    class SystemBase
    {
        public:
            SystemBase() = default;
            SystemBase(const SystemBase&) = delete;
            SystemBase(SystemBase&&) = delete;
            SystemBase& operator=(const SystemBase&) = delete;
            SystemBase& operator=(SystemBase&&) = delete;

        protected:
            ~SystemBase() = default;
    };
}