#pragma once

#include <iosfwd>

struct counter {
    counter() = default;
    counter(int dc, int cc, int ca, int mc, int ma, int d, int o):default_constructors(dc),
        copy_constructors(cc), copy_assignments(ca), move_constructors(mc), move_assignments(ma),
        destructors(d), other(o) {}
    int default_constructors = 0;
    int copy_constructors = 0;
    int copy_assignments = 0;
    int move_constructors = 0;
    int move_assignments = 0;
    int destructors = 0;
    int other = 0;
};

bool operator==(counter const& c1, counter const& c2);
counter operator-(counter c1, counter const& c2);
std::ostream& operator<<(std::ostream& out, counter const& cnt);

struct tracker {
    tracker();
    tracker(double val);
    tracker(const tracker&) noexcept;
    tracker& operator=(const tracker&) noexcept;
    tracker(tracker&&) noexcept;
    tracker& operator=(tracker&&) noexcept;
    ~tracker();

    double value;
    static counter cnt;
};

bool operator==(const tracker& c1, const tracker& c2);

struct thrower {
    thrower(int, bool);
    thrower(thrower const&);
    thrower& operator=(thrower const&);
    thrower(thrower&&) noexcept;
    thrower& operator=(thrower&&) noexcept;

    void check() const;
    int value;
    bool should_throw;
};

bool operator==(thrower const& t1, thrower const& t2);
