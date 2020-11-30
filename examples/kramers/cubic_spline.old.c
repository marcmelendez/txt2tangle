# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "random.h"

int main(int argc, char * argv[])
{
  /* Usage message */
  if(argc < 5) {
    fprintf(stderr, "Usage: %s <trap depth> <trap radius> <diffusion coefficient> <thermal energy>\n", argv[0]);
    return 0;
  }

  /* Variable declarations */
  int nruns = 10000; /* Number of runs */
  int run;
  double q[2]; /* Particle position */
  double F[2], Fmod; /* Force and modulus of force*/
  double r; /* Distance to origin */
  double D0 = atof(argv[3]); /* Diffusion coefficient */
  double kT = atof(argv[4]); /* Thermal energy */
  double trapdepth = atof(argv[1]);
  double R = atof(argv[2]);
  double mu = D0/kT; /* Mobility */
  double dt = 0.0001; /* Time step */
  long int tsteps;
  long int sumtime = 0;

  for(run = 0; run < nruns; run++) {
    /* Reset position */
    q[0] = q[1] = 0.0;

    /* Run Brownian dynamics until particle escapes */
    for(tsteps = 0; 1; tsteps++) {
      r = sqrt(q[0]*q[0] + q[1]*q[1]);
      if(r >= R) break;

      Fmod = 6.0*trapdepth*(r/R - 1)/(R*R*R);

      F[0] = Fmod*q[0];
      F[1] = Fmod*q[1];

      q[0] += mu*F[0]*dt + Gaussian(0,1)*sqrt(2*D0*dt);
      q[1] += mu*F[1]*dt + Gaussian(0,1)*sqrt(2*D0*dt);
    }

    sumtime += tsteps;
  }

  printf("# depth \t radius \t trapping rate\n");
  printf("%f\t%f\t%f\n", trapdepth, R, ((float) nruns)/(sumtime*dt));

  return 0;
}
