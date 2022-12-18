#include "pendulum.h"
#include <pthread.h>
#include <stddef.h>
#include <unistd.h>
#include <math.h>

/* Watek sterujacy wahadlem */
void* controller(void* unused)
{
  double position = 0.0;
  double velocity = 0.0;
  double kinetic = 0.0;
  double potential = 0.0;

  int dir = 0;
  int done = 0;

  (void)unused;

  /* TODO: ustawic parametry poczatkowe symulacji i zasygnalizowaæ to innym
   *       watkom (zmienna warunkowa/condition variable) */
  pthread_mutex_lock(&pendulum_params.mutex);
  pendulum_params.position = 0.0;
  pendulum_params.ready = 1;
  pthread_cond_signal(&pendulum_params.condvar);
  pthread_mutex_unlock(&pendulum_params.mutex);

  while(!done)
    {
      /* TODO: wczytac stan wahadla do zmiennych lokalnych */
      	 pthread_mutex_lock(&pendulum_state.mutex);
         position = pendulum_state.position ;
         velocity = pendulum_state.velocity ;
         kinetic = pendulum_state.kinetic ;
         potential = pendulum_state.potential ;
         done = pendulum_state.done;
         pthread_mutex_unlock(&pendulum_state.mutex);


      /* Na samym szczycie (N) energia potencjalna wynosi ok 1.30 */
      if(sin(position) >= 0)
        {
          /******************************************************************
           * Wahadlo w prawej polplaszczyznie (wschod).
           *
           *                 ,,,,,
           *                 --+--
           *                    \
           *                     \
           *                      O
           *
           *  - albo przykladamy moment przeciwzegarowo (dir=1),
           *  - albo zdejmujemy (dir=0).
           *
           ******************************************************************/

          /* TODO: wyznaczyc sterowanie, dir = ... */
    	  	  if ((velocity<0)||(potential+kinetic<1.3))
    	  		  dir=1;
    	  	  else
    	  		  dir=0;
        }
      else
        {
          /******************************************************************
           * Wahadlo w lewej polplaszczyznie (zachod):
           *
           *                 ,,,,,
           *                 --+--
           *                  /
           *                 /
           *                O
           *
           *  - albo przykladamy moment wpolzegarowo (dir=-1),
           *  - albo zdejmujemy (dir=0).
           *
           ******************************************************************/

          /* TODO: wyznaczyc sterowanie, dir = ... */
    	  if ((velocity>0)||(potential+kinetic<1.3))
    	      dir=-1;
    	  else
    	      dir=0;
        }

      /* Zadajemy obliczone sterowanie */
      /* TODO: zweryfikowac, czy nic tu wiecej nie potrzeba */
      pthread_mutex_lock(&pendulum_control.mutex);
      pendulum_control.dir = dir;
      pthread_mutex_unlock(&pendulum_control.mutex);

      usleep(100000); /* 100 ms */
    }

  return NULL;
}

/* Tworzy watek sterujacy wahadlem i zwraca jego identyfikator */
pthread_t start_controller()
{
  pthread_t thread = (pthread_t)NULL;
  /* TODO: utworzyc watek */
  int rc;
  rc=pthread_create(&thread,NULL,&controller,NULL);
  if(rc)
  {
	  printf("Blad. Kod powrotu z funkcji ptherad_create(): %d\n", rc);
	  exit(1);
  }
  return thread;
}
