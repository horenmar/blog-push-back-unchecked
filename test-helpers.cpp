#include "test-helpers.hpp"

#include <ostream>

bool operator==(counter const& c1, counter const& c2) {
    return c1.default_constructors == c2.default_constructors &&
        c1.copy_constructors == c2.copy_constructors &&
        c1.copy_assignments == c2.copy_assignments &&
        c1.move_constructors == c2.move_constructors &&
        c1.move_assignments == c2.move_assignments &&
        c1.destructors == c2.destructors &&
        c1.other == c2.other;
}
counter operator-(counter c1, counter const& c2) {
    c1.default_constructors -= c2.default_constructors;
    c1.copy_constructors -= c2.copy_constructors;
    c1.copy_assignments -= c2.copy_assignments;
    c1.move_constructors -= c2.move_constructors;
    c1.move_assignments -= c2.move_assignments;
    c1.destructors -= c2.destructors;
    c1.other -= c2.other;
    return c1;
}

std::ostream & operator<<(std::ostream& out, counter const& cnt) {
    out << "| dc: " << cnt.default_constructors
        << " | cc: " << cnt.copy_constructors
        << " | ca: " << cnt.copy_assignments
        << " | mc: " << cnt.move_constructors
        << " | ma: " << cnt.move_assignments
        << " | ~d: " << cnt.destructors
        << " | o:  " << cnt.other
        << " |";
    return out;
}

tracker::tracker():value(0) {
    cnt.default_constructors++;
}

tracker::tracker(double val):value(val) {
    cnt.other++;
}

tracker::tracker(tracker const& rhs) noexcept:
    value(rhs.value) {
    cnt.copy_constructors++;
}
tracker& tracker::operator=(tracker const& rhs) noexcept {
    value = rhs.value;
    cnt.copy_assignments++;
    return *this;
}
tracker::tracker(tracker&& rhs) noexcept:
    value(rhs.value) {
    cnt.move_constructors++;
}
tracker& tracker::operator=(tracker&& rhs) noexcept {
    value = rhs.value;
    cnt.move_assignments++;
    return *this;
}
tracker::~tracker() {
    cnt.destructors++;
}

counter tracker::cnt = {};


bool operator==(tracker const& c1, tracker const& c2) {
    return c1.value == c2.value;
}

bool operator==(thrower const& t1, thrower const& t2) {
    return t1.value == t2.value && t1.should_throw == t2.should_throw;
}

thrower::thrower(int value, bool should_throw):value{ value }, should_throw { should_throw } {}

thrower::thrower(thrower const& rhs):value{ rhs.value }, should_throw{ rhs.should_throw } {
    rhs.check();
}

thrower& thrower::operator=(thrower const& rhs) {
    rhs.check();
    value = rhs.value;
    should_throw = rhs.should_throw;
    return *this;
}

thrower::thrower(thrower&& rhs) noexcept:
    value{ rhs.value },
    should_throw{ rhs.should_throw }
{}

thrower& thrower::operator=(thrower&& rhs) noexcept {
    value = rhs.value;
    should_throw = rhs.should_throw;
    return *this;
}

void thrower::check() const {
    if (should_throw) {
        throw 123;
    }
}
