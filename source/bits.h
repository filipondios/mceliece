#pragma once

#define MSB 0x80
#define GET(m, i, j)   ((m[i]) &   (MSB >> j))
#define SET(m, i, j)   ((m[i]) |=  (MSB >> j))
#define UNSET(m, i, j) ((m[i]) &= ~(MSB >> j))
