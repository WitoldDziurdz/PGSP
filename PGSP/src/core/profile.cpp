#include "profile.h"

#include <iostream>
#include <sstream>

TotalDuration::TotalDuration(const string& msg)
    : message(msg + ": ")
    , start(steady_clock::now())
{
}

TotalDuration::~TotalDuration() {
    ostringstream os;
    os << message
        << duration_cast<milliseconds>(steady_clock::now() - start).count()
        << " ms" << endl;
    cerr << os.str();
}
