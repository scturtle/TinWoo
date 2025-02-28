#pragma once

#include <string>
#include <tuple>
#include <vector>

#include "nx/content_meta.hpp"

namespace tin::util {
nx::ncm::ContentMeta GetContentMetaFromNCA(std::string ncaPath);
} // namespace tin::util
