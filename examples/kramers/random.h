# ifndef _RANDOM_H_
# define _RANDOM_H_

# ifndef ONLY_PREPROCESSOR
# include <stdint.h>
# else
//!# include <stdint.h>
# endif

# ifndef __bool_type__
# define __bool_type__
typedef enum {false, true} bool; /* Boolean type */
# endif

/* Pseudorandom number generation */
uint64_t s[] = {12679825035178159220ull, 15438657923749336752ull}; /* PRNG seed */

# ifdef PRNG_XORSHIFT
# define PRNG() xorshift128plus()
# define RANDOM_MAX 18446744073709551615ull
/* 64-bit (pseudo)random integer */
uint64_t xorshift128plus(void)
{
  uint64_t x = s[0];
  uint64_t const y = s[1];
  s[0] = y;
  x ^= x << 23; // a
  x ^= x >> 17; // b
  x ^= y ^ (y >> 26); // c
  s[1] = x;
  return x + y;
}
# else
/* Minimal PCG32 code adapted from M.E. O'Neill: pcg-random.org (2014) */
# define PRNG() pcg32_random_r()
# define RANDOM_MAX 4294967295ul

uint32_t pcg32_random_r(void)
{
    uint64_t oldstate = s[0];
    // Advance internal state
    s[0] = oldstate * 6364136223846793005ULL + (s[1]|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}
# endif

/* Random number from a uniform distribution */
double uniform(double min, double max)
{
  return min + (PRNG()/((double) RANDOM_MAX))*(max - min);
}

# ifndef NO_GAUSSIAN_PRNG
/* Random number from a Gaussian distribution */
float Gaussian(float mean, float stddev)
{
  double u1, u2, s0 = 2;
  static bool saved_value = false; /* Flag indicating stored number */
  static double Gnum; /* Stored Gaussian number */

  if(saved_value) {
    saved_value = false;
    return Gnum;
  }

  while(s0 >= 1)
  {
    u1 = uniform(-1, 1);
    u2 = uniform(-1, 1);
    s0 = u1*u1 + u2*u2;
  }

  Gnum = mean + stddev*u2*sqrt(-2*logf(s0)/s0);
  saved_value = true;

  return mean + stddev*u1*sqrt(-2*logf(s0)/s0);
}
# endif

/* Marsaglia's algorithm for random point on a unit sphere */
void random_axis(float * n)
{
  float x, y, s = 2;

  while(s > 1) {
    x = uniform(-1, 1);
    y = uniform(-1, 1);
    s = x*x + y*y;
  }

  n[0] = 2*x*sqrt(1 - s);
  n[1] = 2*y*sqrt(1 - s);
  n[2] = 1 - 2*s;

  return;
}

/* To change the PRNG seed you may use code like this:
// The PRNG state must be seeded so that it is not everywhere zero.
s[0] = 12679825035178159220u;
s[1] = 15438657923749336752u;
*/

# endif
