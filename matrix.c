#include <stdio.h>
#include <math.h>

struct position
{
    double x;
    double y;
};

struct angle
{
    double sin;
    double cos;
};

/**
 * init_angle - init
 * @a:
 * @origin:
 */
int init_angle(struct angle *a, struct position *origin)
{
    double x = origin->x;
    double y = origin->y;

    a->sin = y / sqrt(x * x + y * y);
    a->cos = x / sqrt(x * x + y * y);
    return 0;
}

/**
 * position_rectify - rectify position
 * @a:
 * @out:
 * @in:
 */
int position_rectify(const struct angle *a,
        struct position *in, struct position *out)
{
    double x = in->x;
    double y = in->y;
    double sin = a->sin;
    double cos = a->cos;

    out->x = x * cos + y * sin;
    out->y = y * cos - x * sin;
    return 0;
}

int main(void)
{
    struct position origin = {1, 1};
    struct position in = {15, 6};
    struct position out;
    struct angle a;

    init_angle(&a, &origin);

    position_rectify(&a, &in, &out);

    printf("x:%f, y:%f\n", out.x, out.y);
    return 0;
}
