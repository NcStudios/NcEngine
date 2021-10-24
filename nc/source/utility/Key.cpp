
#include "utility/Key.h"

namespace nc
{

    Key::Key(const std::string input) noexcept
    {
        text = (char*)input.c_str();
        value = hash_input(text);
    }

    Key::Key(const Key& input) noexcept
    {
        text = input.text;
        value = input.value;
    }

}