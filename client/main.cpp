#include "photo_booth.hpp"

#include <memory>

using namespace photo_booth;
using namespace std;

int main() {
    auto const photo_booth = make_unique<PhotoBooth>();

    photo_booth->run();

    return 0;
}

