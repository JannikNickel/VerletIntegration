#pragma once

#include <optional>
#include <type_traits>

template<typename T>
using optional_ref = std::optional<std::reference_wrapper<const T>>;
