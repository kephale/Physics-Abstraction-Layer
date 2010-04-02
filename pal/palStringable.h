#include <iosfwd>
#include <string>

class palStringable {
public:
    virtual std::string toString() const = 0;
};

extern std::ostream& operator<<(std::ostream& os, const palStringable& stringable);
