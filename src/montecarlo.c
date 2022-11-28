// Sample function on infinite interval using inputs in range (-1,1)
inline void sample_inf_integrand(integrand_func *f, real *output, const real *input, const u64 size) {
    const real in_scale = 1.0/(1.0 - x*x);
    const real out_scale = in_scale * (2.0*in_scale - 1.0);
    real transformed_input[size];

    for (u32 i = 0; i < size; ++i)
        transformed_input[i] = input[i]*in_scale;

    f(output, transformed_input, size, NULL);

    for (u32 i = 0; i < size; ++i)
        output[i] *= out_scale;
}

// Reservioir sampling

struct reservior_sampling {
    struct random_series_xors series;
    f32 sample;
    f32 weight;
    f32 total_weight;
    u32 samples_seen;
};

void reservoir_init(struct reservoir_sampling *r) {
    *r = (struct reservoir_sampling) {0};
    random_default_seed(&r->series);
}

void reservoir_update(struct reservoir_sampling *r, f32 sample, f32 weight) {
    r->total_weight += weight;
    ++r->samples_seen;

    // Flip coin weighted weight and previous total weight
    if (random_f32(r->series, 0.0f, r->total_weight) <= weight) {
        r->sample = sample;
        r->weight = weight;
    }
}

// RIS

#define RIS_SAMPLES 1024
struct resampled_importance_sampler {
    struct random_series_xors series;
    f32 bad_samples[RIS_SAMPLES];
    f32 bad_weights[RIS_SAMPLES];
    f32 good_samples[RIS_SAMPLES];
    f32 good_weights[RIS_SAMPLES];
};

void ris_init(struct resampled_importance_sampler *ris) {
    *ris = (struct resampled_importance_sampler) {0};
    random_default_seed(&ris->series);
}

void ris_init(struct resamples_importance_sampler *ris, integrand_func *f) {
    const real epsilon = 1e-7;
    // Draw all bad samples
    for (u32 i = 0; i < RIS_SAMPLES; ++i) {
        ris->bad_samples[i] = random_f32(&series, -1.0f + epsilon, 1.0f - epsilon);
    }

    // w = complexpdf(x)/simplepdf(x)
    sample_inf_integrand(f, ris->bad_weights, ris->bad_samples, RIS_SAMPLES);

    // Draw y from bad_samples with weight w

    // Calculate average of bad samples
    f32 bad_average = 0.0f;
    for (u32 i = 0; i < RIS_SAMPLES; ++i) {
        bad_average += ris->bad_samples[i]/(f32) RIS_SAMPLES;
    }

    // Weight of new sample: average(bad_samples)/complexpdf(y)
}

struct montecarlo_result {
    real integral;
    real error;
    u32 performed_iters;
    bool converged;
};

u32 montecarlo_infinite_interval(integrand_func *f, const real abs_tol, const real rel_tol, struct montecarlo_result *res) {
    const u64 num_iterations_sqrt = 1.0/abs_tol;
    const u64 num_iterations = 100000000000; //num_iterations_sqrt*num_iterations_sqrt;

    struct random_series_xors series = {0};
    random_default_seed(&series);

    real sum = 0.0;
    const real interval_size = 2.0;

    res->performed_iters = 0;
    while (res->performed_iters < num_iterations) {
        const real sample = random_f32(&series, -1.0f + epsilon, 1.0f - epsilon);

        f32 x = sample;
        f32 y;
        {
            const real in_scale = 1.0/(1.0 - x*x);
            const real transformed_sample = sample * in_scale;

            f(&y, &transformed_sample, 1, NULL);

            const real out_scale = in_scale * (2.0*in_scale - 1.0);
            y *= out_scale;
        }

        sum += interval_size*output/(f32)num_iterations;

        ++res->performed_iters;
    }


    res->integral = sum;
    res->error = abs_tol;
    res->converged = true;

    return 0;
}
