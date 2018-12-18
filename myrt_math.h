#pragma once

#include "myrt.h"

struct v2 {
    float X;
    float Y;
};

internal inline v2
operator+(const v2 &lhs, const v2 &rhs)
{
    return { lhs.X + rhs.X,
        lhs.Y + rhs.Y
    };
}

internal inline v2
operator-(const v2 &lhs, const v2 &rhs)
{
    return { lhs.X - rhs.X,
        lhs.Y - rhs.Y
    };
}

internal inline v2
operator*(const v2 &lhs, float s)
{
    return { lhs.X * s,
        lhs.Y * s
    };
}

internal inline v2
operator/(const v2 &lhs, float s)
{
    return { lhs.X / s,
        lhs.Y / s
    };
}

struct v3 {
    float X;
    float Y;
    float Z;
};

internal inline v3
operator+(const v3 &lhs, const v3 &rhs)
{
    return { lhs.X + rhs.X,
        lhs.Y + rhs.Y,
        lhs.Z + rhs.Z
    };
}

internal inline v3
operator-(const v3 &lhs, const v3 &rhs)
{
    return { lhs.X - rhs.X,
        lhs.Y - rhs.Y,
        lhs.Z - rhs.Z
    };
}

internal inline v3
operator*(const v3 &lhs, float s)
{
    return { lhs.X * s,
        lhs.Y * s,
        lhs.Z * s
    };
}

internal inline v3
operator*(float s, const v3 &rhs)
{
    return { rhs.X * s,
        rhs.Y * s,
        rhs.Z * s
    };
}

internal inline v3
operator/(const v3 &lhs, float s)
{
    return { lhs.X / s,
        lhs.Y / s,
        lhs.Z / s
    };
}
