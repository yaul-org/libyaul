#include <gamemath/fix16/fix16_vec3.h>

bool fix16_vec3_t::is_near_zero(fix16_t epsilon) const {
    // TODO: Implement this in C and use it here
    return x.is_near_zero(epsilon) && y.is_near_zero(epsilon) && z.is_near_zero(epsilon);
}

void fix16_vec3_t::end_normalization() {
    const fix16_t scale = fix16_t{cpu_divu_quotient_get()};

    x = fix16_mul(scale, x);
    y = fix16_mul(scale, y);
    z = fix16_mul(scale, z);
}

fix16_vec3_t fix16_vec3_t::reflect(const fix16_vec3_t& v, const fix16_vec3_t& normal) {
    // TODO: Move this to a C function
    const fix16_t factor = dot_product(v, normal) << 1;
    const fix16_vec3_t proj = normal * factor;

    return (v - proj);
}
