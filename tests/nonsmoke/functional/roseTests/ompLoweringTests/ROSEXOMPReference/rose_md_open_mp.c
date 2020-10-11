# include <stdlib.h>
# include <stdio.h>
# include <time.h>
# include <math.h>
# include <omp.h>
#include "libxomp.h" 
void compute(int np,int nd,double pos[],double vel[],double mass,double f[],double *pot,double *kin);
double dist(int nd,double r1[],double r2[],double dr[]);
void initialize(int np,int nd,double box[],int *seed,double pos[],double vel[],double acc[]);
double r8_uniform_01(int *seed);
void timestamp();
void update(int np,int nd,double pos[],double vel[],double f[],double acc[],double mass,double dt);
/******************************************************************************/

int main(int argc,char *argv[])
/******************************************************************************/
/*
  Purpose:
    MAIN is the main program for MD_OPEN_MP.
  Discussion:
    MD implements a simple molecular dynamics simulation.
    The program uses Open MP directives to allow parallel computation.
    The velocity Verlet time integration scheme is used. 
    The particles interact with a central pair potential.
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    30 July 2009
  Author:
    Original FORTRAN77 version by Bill Magro.
    C version by John Burkardt.
  Parameters:
    None
*/
{
  int status = 0;
  XOMP_init(argc,argv);
  double *acc;
  double *box;
  double dt = 0.0001;
  double e0;
  double *force;
  int i;
//int id;
  double kinetic;
  double mass = 1.0;
  int nd = 3;
  int np = 1000;
  double *pos;
  double potential;
  int proc_num;
  int seed = 123456789;
  int step;
  int step_num = 400;
  int step_print;
  int step_print_index;
  int step_print_num;
  double *vel;
  double wtime;
  timestamp();
  proc_num = omp_get_num_procs();
  acc = ((double *)(malloc((nd * np) * sizeof(double ))));
  box = ((double *)(malloc(nd * sizeof(double ))));
  force = ((double *)(malloc((nd * np) * sizeof(double ))));
  pos = ((double *)(malloc((nd * np) * sizeof(double ))));
  vel = ((double *)(malloc((nd * np) * sizeof(double ))));
  printf("\n");
  printf("MD_OPEN_MP\n");
  printf("  C/OpenMP version\n");
  printf("\n");
  printf("  A molecular dynamics program.\n");
  printf("\n");
  printf("  NP, the number of particles in the simulation is %d\n",np);
  printf("  STEP_NUM, the number of time steps, is %d\n",step_num);
  printf("  DT, the size of each time step, is %f\n",dt);
  printf("\n");
  printf("  Number of processors available = %d\n",(omp_get_num_procs()));
  printf("  Number of threads =              %d\n",(omp_get_max_threads()));
/*
  Set the dimensions of the box.
*/
  for (i = 0; i < nd; i++) {
    box[i] = 10.0;
  }
  printf("\n");
  printf("  Initializing positions, velocities, and accelerations.\n");
/*
  Set initial positions, velocities, and accelerations.
*/
  initialize(np,nd,box,&seed,pos,vel,acc);
/*
  Compute the forces and energies.
*/
  printf("\n");
  printf("  Computing initial forces and energies.\n");
  compute(np,nd,pos,vel,mass,force,&potential,&kinetic);
  e0 = potential + kinetic;
/*
  This is the main time stepping loop:
    Compute forces and energies,
    Update positions, velocities, accelerations.
*/
  printf("\n");
  printf("  At each step, we report the potential and kinetic energies.\n");
  printf("  The sum of these energies should be a constant.\n");
  printf("  As an accuracy check, we also print the relative error\n");
  printf("  in the total energy.\n");
  printf("\n");
  printf("      Step      Potential       Kinetic        (P+K-E0)/E0\n");
  printf("                Energy P        Energy K       Relative Energy Error\n");
  printf("\n");
  step_print = 0;
  step_print_index = 0;
  step_print_num = 10;
  step = 0;
  printf("  %8d  %14f  %14f  %14e\n",step,potential,kinetic,(potential + kinetic - e0) / e0);
  step_print_index = step_print_index + 1;
  step_print = step_print_index * step_num / step_print_num;
  wtime = omp_get_wtime();
  for (step = 1; step <= step_num; step++) {
    compute(np,nd,pos,vel,mass,force,&potential,&kinetic);
    if (step == step_print) {
      printf("  %8d  %14f  %14f  %14e\n",step,potential,kinetic,(potential + kinetic - e0) / e0);
      step_print_index = step_print_index + 1;
      step_print = step_print_index * step_num / step_print_num;
    }
    update(np,nd,pos,vel,force,acc,mass,dt);
  }
  wtime = omp_get_wtime() - wtime;
  printf("\n");
  printf("  Elapsed time for main computation:\n");
  printf("  %f seconds.\n",wtime);
  free(acc);
  free(box);
  free(force);
  free(pos);
  free(vel);
  printf("\n");
  printf("MD_OPEN_MP\n");
  printf("  Normal end of execution.\n");
  printf("\n");
  timestamp();
  XOMP_terminate(status);
  return 0;
}
/******************************************************************************/

struct OUT__2__9996___data 
{
  void *np_p;
  void *nd_p;
  double **pos_p;
  double **vel_p;
  double **f_p;
  void *ke_p;
  void *pe_p;
  void *PI2_p;
}
;
static void OUT__2__9996__(void *__out_argv);

void compute(int np,int nd,double pos[],double vel[],double mass,double f[],double *pot,double *kin)
/******************************************************************************/
/*
  Purpose:
    COMPUTE computes the forces and energies.
  Discussion:
    The computation of forces and energies is fully parallel.
    The potential function V(X) is a harmonic well which smoothly
    saturates to a maximum value at PI/2:
      v(x) = ( sin ( min ( x, PI2 ) ) )**2
    The derivative of the potential is:
      dv(x) = 2.0 * sin ( min ( x, PI2 ) ) * cos ( min ( x, PI2 ) )
            = sin ( 2.0 * min ( x, PI2 ) )
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    21 November 2007
  Author:
    Original FORTRAN77 version by Bill Magro.
    C version by John Burkardt.
  Parameters:
    Input, int NP, the number of particles.
    Input, int ND, the number of spatial dimensions.
    Input, double POS[ND*NP], the position of each particle.
    Input, double VEL[ND*NP], the velocity of each particle.
    Input, double MASS, the mass of each particle.
    Output, double F[ND*NP], the forces.
    Output, double *POT, the total potential energy.
    Output, double *KIN, the total kinetic energy.
*/
{
  double d;
  double d2;
  int i;
  int j;
  int k;
  double ke;
  double pe;
  double PI2 = 3.141592653589793 / 2.0;
  double rij[3];
  pe = 0.0;
  ke = 0.0;
  struct OUT__2__9996___data __out_argv2__9996__;
  __out_argv2__9996__ . PI2_p = ((void *)(&PI2));
  __out_argv2__9996__ . pe_p = ((void *)(&pe));
  __out_argv2__9996__ . ke_p = ((void *)(&ke));
  __out_argv2__9996__ . f_p = &f;
  __out_argv2__9996__ . vel_p = &vel;
  __out_argv2__9996__ . pos_p = &pos;
  __out_argv2__9996__ . nd_p = ((void *)(&nd));
  __out_argv2__9996__ . np_p = ((void *)(&np));
  XOMP_parallel_start(OUT__2__9996__,&__out_argv2__9996__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/md_open_mp.c",256);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/md_open_mp.c",304);
  ke = ke * 0.5 * mass;
   *pot = pe;
   *kin = ke;
  return ;
}
/******************************************************************************/

double dist(int nd,double r1[],double r2[],double dr[])
/******************************************************************************/
/*
  Purpose:
    DIST computes the displacement (and its norm) between two particles.
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    21 November 2007
  Author:
    Original FORTRAN77 version by Bill Magro.
    C version by John Burkardt.
  Parameters:
    Input, int ND, the number of spatial dimensions.
    Input, double R1[ND], R2[ND], the positions of the particles.
    Output, double DR[ND], the displacement vector.
    Output, double D, the Euclidean norm of the displacement.
*/
{
  double d;
  int i;
  d = 0.0;
  for (i = 0; i < nd; i++) {
    dr[i] = r1[i] - r2[i];
    d = d + dr[i] * dr[i];
  }
  d = sqrt(d);
  return d;
}
/******************************************************************************/

void initialize(int np,int nd,double box[],int *seed,double pos[],double vel[],double acc[])
/******************************************************************************/
/*
  Purpose:
    INITIALIZE initializes the positions, velocities, and accelerations.
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    21 November 2007
  Author:
    Original FORTRAN77 version by Bill Magro.
    C version by John Burkardt.
  Parameters:
    Input, int NP, the number of particles.
    Input, int ND, the number of spatial dimensions.
    Input, double BOX[ND], specifies the maximum position
    of particles in each dimension.
    Input, int *SEED, a seed for the random number generator.
    Output, double POS[ND*NP], the position of each particle.
    Output, double VEL[ND*NP], the velocity of each particle.
    Output, double ACC[ND*NP], the acceleration of each particle.
*/
{
  int i;
  int j;
/*
  Give the particles random positions within the box.
*/
  for (i = 0; i < nd; i++) {
    for (j = 0; j < np; j++) {
      pos[i + j * nd] = box[i] * r8_uniform_01(seed);
    }
  }
  for (j = 0; j < np; j++) {
    for (i = 0; i < nd; i++) {
      vel[i + j * nd] = 0.0;
    }
  }
  for (j = 0; j < np; j++) {
    for (i = 0; i < nd; i++) {
      acc[i + j * nd] = 0.0;
    }
  }
  return ;
}
/******************************************************************************/

double r8_uniform_01(int *seed)
/******************************************************************************/
/*
  Purpose:
    R8_UNIFORM_01 is a unit pseudorandom R8.
  Discussion:
    This routine implements the recursion
      seed = 16807 * seed mod ( 2**31 - 1 )
      unif = seed / ( 2**31 - 1 )
    The integer arithmetic never requires more than 32 bits,
    including a sign bit.
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    11 August 2004
  Author:
    John Burkardt
  Reference:
    Paul Bratley, Bennett Fox, Linus Schrage,
    A Guide to Simulation,
    Springer Verlag, pages 201-202, 1983.
    Bennett Fox,
    Algorithm 647:
    Implementation and Relative Efficiency of Quasirandom
    Sequence Generators,
    ACM Transactions on Mathematical Software,
    Volume 12, Number 4, pages 362-376, 1986.
  Parameters:
    Input/output, int *SEED, a seed for the random number generator.
    Output, double R8_UNIFORM_01, a new pseudorandom variate, strictly between
    0 and 1.
*/
{
  int k;
  double r;
  k =  *seed / 127773;
   *seed = 16807 * ( *seed - k * 127773) - k * 2836;
  if ( *seed < 0) {
     *seed =  *seed + 2147483647;
  }
  r = ((double )( *seed)) * 4.656612875E-10;
  return r;
}
/******************************************************************************/

void timestamp()
/******************************************************************************/
/*
  Purpose:
    TIMESTAMP prints the current YMDHMS date as a time stamp.
  Example:
    31 May 2001 09:45:54 AM
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    24 September 2003
  Author:
    John Burkardt
  Parameters:
    None
*/
{
# define TIME_SIZE 40
  static char time_buffer[40];
  const struct tm *tm;
  size_t len;
  time_t now;
  now = time(((void *)0));
  tm = (localtime((&now)));
  len = strftime(time_buffer,40,"%d %B %Y %I:%M:%S %p",tm);
  printf("%s\n",time_buffer);
  return ;
# undef TIME_SIZE
}
/******************************************************************************/

struct OUT__1__9996___data 
{
  void *np_p;
  void *nd_p;
  double **pos_p;
  double **vel_p;
  double **f_p;
  double **acc_p;
  void *dt_p;
  void *rmass_p;
}
;
static void OUT__1__9996__(void *__out_argv);

void update(int np,int nd,double pos[],double vel[],double f[],double acc[],double mass,double dt)
/******************************************************************************/
/*
  Purpose:
    UPDATE updates positions, velocities and accelerations.
  Discussion:
    The time integration is fully parallel.
    A velocity Verlet algorithm is used for the updating.
    x(t+dt) = x(t) + v(t) * dt + 0.5 * a(t) * dt * dt
    v(t+dt) = v(t) + 0.5 * ( a(t) + a(t+dt) ) * dt
    a(t+dt) = f(t) / m
  Licensing:
    This code is distributed under the GNU LGPL license. 
  Modified:
    17 April 2009
  Author:
    Original FORTRAN77 version by Bill Magro.
    C version by John Burkardt.
  Parameters:
    Input, int NP, the number of particles.
    Input, int ND, the number of spatial dimensions.
    Input/output, double POS[ND*NP], the position of each particle.
    Input/output, double VEL[ND*NP], the velocity of each particle.
    Input, double F[ND*NP], the force on each particle.
    Input/output, double ACC[ND*NP], the acceleration of each particle.
    Input, double MASS, the mass of each particle.
    Input, double DT, the time step.
*/
{
  int i;
  int j;
  double rmass;
  rmass = 1.0 / mass;
  struct OUT__1__9996___data __out_argv1__9996__;
  __out_argv1__9996__ . rmass_p = ((void *)(&rmass));
  __out_argv1__9996__ . dt_p = ((void *)(&dt));
  __out_argv1__9996__ . acc_p = &acc;
  __out_argv1__9996__ . f_p = &f;
  __out_argv1__9996__ . vel_p = &vel;
  __out_argv1__9996__ . pos_p = &pos;
  __out_argv1__9996__ . nd_p = ((void *)(&nd));
  __out_argv1__9996__ . np_p = ((void *)(&np));
  XOMP_parallel_start(OUT__1__9996__,&__out_argv1__9996__,1,0,"/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/md_open_mp.c",607);
  XOMP_parallel_end("/home/awang15/Projects/rexdev/rex_src/tests/nonsmoke/functional/CompileTests/OpenMP_tests/md_open_mp.c",620);
  return ;
}

static void OUT__1__9996__(void *__out_argv)
{
  int *np = (int *)(((struct OUT__1__9996___data *)__out_argv) -> np_p);
  int *nd = (int *)(((struct OUT__1__9996___data *)__out_argv) -> nd_p);
  double **pos = (double **)(((struct OUT__1__9996___data *)__out_argv) -> pos_p);
  double **vel = (double **)(((struct OUT__1__9996___data *)__out_argv) -> vel_p);
  double **f = (double **)(((struct OUT__1__9996___data *)__out_argv) -> f_p);
  double **acc = (double **)(((struct OUT__1__9996___data *)__out_argv) -> acc_p);
  double *dt = (double *)(((struct OUT__1__9996___data *)__out_argv) -> dt_p);
  double *rmass = (double *)(((struct OUT__1__9996___data *)__out_argv) -> rmass_p);
  int _p_i;
  int _p_j;
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *np - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
    for (_p_i = 0; _p_i <  *nd; _p_i++) {
      ( *pos)[_p_i + p_index_ *  *nd] = ( *pos)[_p_i + p_index_ *  *nd] + ( *vel)[_p_i + p_index_ *  *nd] *  *dt + 0.5 * ( *acc)[_p_i + p_index_ *  *nd] *  *dt *  *dt;
      ( *vel)[_p_i + p_index_ *  *nd] = ( *vel)[_p_i + p_index_ *  *nd] + 0.5 *  *dt * (( *f)[_p_i + p_index_ *  *nd] *  *rmass + ( *acc)[_p_i + p_index_ *  *nd]);
      ( *acc)[_p_i + p_index_ *  *nd] = ( *f)[_p_i + p_index_ *  *nd] *  *rmass;
    }
  }
  XOMP_barrier();
}

static void OUT__2__9996__(void *__out_argv)
{
  int *np = (int *)(((struct OUT__2__9996___data *)__out_argv) -> np_p);
  int *nd = (int *)(((struct OUT__2__9996___data *)__out_argv) -> nd_p);
  double **pos = (double **)(((struct OUT__2__9996___data *)__out_argv) -> pos_p);
  double **vel = (double **)(((struct OUT__2__9996___data *)__out_argv) -> vel_p);
  double **f = (double **)(((struct OUT__2__9996___data *)__out_argv) -> f_p);
  double *ke = (double *)(((struct OUT__2__9996___data *)__out_argv) -> ke_p);
  double *pe = (double *)(((struct OUT__2__9996___data *)__out_argv) -> pe_p);
  double *PI2 = (double *)(((struct OUT__2__9996___data *)__out_argv) -> PI2_p);
  double _p_d;
  double _p_d2;
  int _p_i;
  int _p_j;
  int _p_k;
  double _p_ke;
  _p_ke = 0;
  double _p_pe;
  _p_pe = 0;
  double _p_rij[3];
  long p_index_;
  long p_lower_;
  long p_upper_;
  XOMP_loop_default(0, *np - 1,1,&p_lower_,&p_upper_);
  for (p_index_ = p_lower_; p_index_ <= p_upper_; p_index_ += 1) {
/*
  Compute the potential energy and forces.
*/
    for (_p_i = 0; _p_i <  *nd; _p_i++) {
      ( *f)[_p_i + p_index_ *  *nd] = 0.0;
    }
    for (_p_j = 0; _p_j <  *np; _p_j++) {
      if (p_index_ != _p_j) {
        _p_d = dist( *nd, *pos + p_index_ *  *nd, *pos + _p_j *  *nd,_p_rij);
/*
  Attribute half of the potential energy to particle J.
*/
        if (_p_d <  *PI2) {
          _p_d2 = _p_d;
        }
         else {
          _p_d2 =  *PI2;
        }
        _p_pe = _p_pe + 0.5 * pow((sin(_p_d2)),2);
        for (_p_i = 0; _p_i <  *nd; _p_i++) {
          ( *f)[_p_i + p_index_ *  *nd] = ( *f)[_p_i + p_index_ *  *nd] - _p_rij[_p_i] * sin(2.0 * _p_d2) / _p_d;
        }
      }
    }
/*
  Compute the kinetic energy.
*/
    for (_p_i = 0; _p_i <  *nd; _p_i++) {
      _p_ke = _p_ke + ( *vel)[_p_i + p_index_ *  *nd] * ( *vel)[_p_i + p_index_ *  *nd];
    }
  }
  XOMP_barrier();
  XOMP_atomic_start();
   *ke =  *ke + _p_ke;
  XOMP_atomic_end();
  XOMP_atomic_start();
   *pe =  *pe + _p_pe;
  XOMP_atomic_end();
}
