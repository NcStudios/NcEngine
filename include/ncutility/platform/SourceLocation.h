#pragma once

#if __has_include(<source_location>)
    #include <source_location>
    #define NC_SOURCE_LOCATION std::source_location
#elif __has_include(<experimental/source_location>)
    #include <experimental/source_location>
    #define NC_SOURCE_LOCATION std::experimental::source_location
#else
    namespace nc::detail
    {
    struct PlaceholderSourceLocation
    {
        static constexpr auto msg = "source_location not implemented on this compiler";
        static constexpr auto current() noexcept { return PlaceholderSourceLocation{}; }
        constexpr auto file_name() const noexcept { return msg; }
        constexpr auto function_name() const noexcept { return msg; }
        constexpr auto line() const noexcept { return 0; }
        constexpr auto column() const noexcept { return 0; }
    };
    } // namespace nc::detail

    #define NC_SOURCE_LOCATION nc::detail::PlaceholderSourceLocation
#endif

#define NC_SOURCE_LOCATION_CURRENT NC_SOURCE_LOCATION::current()
#define NC_SOURCE_FILE NC_SOURCE_LOCATION_CURRENT.file_name()
#define NC_SOURCE_FUNCTION NC_SOURCE_LOCATION_CURRENT.function_name()
#define NC_SOURCE_LINE NC_SOURCE_LOCATION_CURRENT.line()
#define NC_SOURCE_COLUMN NC_SOURCE_LOCATION_CURRENT.column()
