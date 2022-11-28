#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <assert.h>

typedef int8_t   i8;
typedef uint8_t  u8;
typedef int16_t  i16;
typedef uint16_t u16;
typedef int32_t  i32;
typedef uint32_t u32;
typedef int64_t  i64;
typedef uint64_t u64;
typedef float    f32;
typedef double   f64;

typedef f64 real;

void log_error(const char *fmt, ...) {
    fprintf(stderr, "\033[31;1m[error]\033[0m ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    fputc('\n', stderr);
}

void log_warning(const char *fmt, ...) {
    fprintf(stdout, "\033[33;1m[warning]\033[0m ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    fputc('\n', stdout);
}

void log_info(const char *fmt, ...) {
    fprintf(stdout, "\033[36;1m[info]\033[0m ");

    va_list args;
    va_start(args, fmt);
    vfprintf(stdout, fmt, args);
    va_end(args);

    fputc('\n', stdout);
}

#include "prioqueue.c"
#include "quadgk.c"

#include "random.h"
#include "montecarlo.c"

#include <stdlib.h>

void f(real * restrict output, const real * restrict input, const u32 n, void *args) {
    (void) args;
    for (u32 i = 0; i < n; ++i) {
        output[i] = exp(-input[i] * input[i]);
    }
}

int main() {
    const struct quadgk_settings settings = {
        .gk = gk15,
        .abs_error_tol = 1e-10,
        .rel_error_tol = 1e-10,
        .max_iters = 10000,
    };

    const u64 size = quadgk_required_memory(&settings);
    void *memory = malloc(size);


    log_info("Should converge to sqrt(pi) =");
    log_info("%27.10lf", sqrt(M_PI));

    {
        struct quadgk_result result = {0};
        quadgk_infinite_interval(f, &settings, memory, &result);
        log_info("%5s %5u %15.10lf (err %e)", (result.converged) ? "true" : "false", result.performed_iters, result.integral, result.error);
    }

    {
        struct montecarlo_result result = {0};
        montecarlo_infinite_interval(f, 1e-10, 1e-10, &result);
        log_info("%5s %5u %15.10lf (err %e)", (result.converged) ? "true" : "false", result.performed_iters, result.integral, result.error);
    }

    free(memory);

    return 0;
}
