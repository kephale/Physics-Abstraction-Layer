#include "palStringable.h"
#include <ostream>

std::ostream& operator<<(std::ostream& os, const palStringable& stringable) {
    return os << stringable.toString();
};
