# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include "random.h"

int main(int argc, char * argv[])
{
  /* Usage message */
  if(argc < 5) {
    fprintf(stderr, "Usage: %s <trap depth> "
                              "<trap radius> "
                              "<diffusion coefficient> "
                              "<thermal energy>\n",
            argv[0]);
    return 0;
  }

  /* Variable declarations */
  int nruns = 10000;    /* Number of runs */
  int run;              /* Current run */
  double q[2];          /* Particle position */
  double F[2], Fmod;    /* Force and modulus of force*/
  double r;             /* Distance to origin */
  long int tstep;       /* Current step */
  long int sumtime = 0; /* Sum of times */
  long int sumt2 = 0;   /* Sum of times squared */

  /* Simulation parameters */
  double E0 = atof(argv[1]); /* Trap depth */
  double R = atof(argv[2]);  /* Trap radius */
  double D = atof(argv[3]);  /* Diffusion coefficient */
  double kT = atof(argv[4]); /* Thermal energy */
  double M = D/kT;           /* Mobility */
  double dt = 0.0001;        /* Time step */

  /* nruns realisations of the stochastic process */
  for(run = 0; run < nruns; run++) {
    /* Reset position */
    q[0] = q[1] = 0.0;

    /* Run Brownian dynamics until particle escapes */
    for(tstep = 0; 1; tstep++) {
      /* Particle position */
      r = sqrt(q[0]*q[0] + q[1]*q[1]);

      /* Break when particle leaves the trap */
      if(r >= R) break;

      /* Force vector */
      Fmod = 6.0*E0*(r/R - 1)/(R*R*R);
      F[0] = Fmod*q[0];
      F[1] = Fmod*q[1];

      /* Euler-Maruyama scheme */
      q[0] += M*F[0]*dt + Gaussian(0,1)*sqrt(2*D*dt);
      q[1] += M*F[1]*dt + Gaussian(0,1)*sqrt(2*D*dt);
    }

    sumtime += tstep;
    sumt2 += tstep*tstep;
  }

  float meantime = (sumtime*dt)/((float) nruns);
  float mu = 1.0/meantime;

  printf("# E0 \t\t R \t\t D \t\t kT \t\t "
         "mu \t\t error(mu)\n");
  printf("%f\t%f\t%f\t%f\t%f\t%f\n", E0, R, D, kT, mu,
          mu*mu*sqrt((sumt2*dt*dt)/((float) nruns)
                     - meantime*meantime)/sqrt(nruns));

  return 0;
}
