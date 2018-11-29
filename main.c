#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <pthread.h>

#define PRNG_BUFFER_SIZE 32
#define DEF_CALC_PER_THREAD 10000

typedef struct {
  struct random_data random_states;
  int points_in_circle, total_points;
  long calculations_per_thread;
} thread_args;
 
typedef struct {
  double x, y;
} Point;

double compute_sqeuclidean_distance(const Point pt) {
  return (pt.x*pt.x - 0.25) + (pt.y*pt.y - 0.25);
}

void* thread_main(void* args) {
  // Get arguments from void*
  thread_args *Args = (thread_args*)args;
  int xCoord, yCoord;
  Point point;
  
  for (int calc = 0; calc < Args->calculations_per_thread; calc++) {
    // Generate random numbers to serve as the x and y coordinates
    random_r(&Args->random_states, &xCoord);
    random_r(&Args->random_states, &yCoord);
    point.x = (double)xCoord / RAND_MAX, point.y = (double)yCoord / RAND_MAX;
    // Compute square euclidean distance from (0.5,0.5)
    if (compute_sqeuclidean_distance(point) <= 0.5)
      Args->points_in_circle++;
    Args->total_points++;
  }
  // Data is returned in the args argument
}

int main(int argc, char** argv) {
  // Get number of logical processors on the system (to optimally choose the number of threads)
  int num_of_cpus = sysconf(_SC_NPROCESSORS_ONLN);

  thread_args *thread_data = (thread_args*)calloc(num_of_cpus, sizeof(thread_args));
  char* random_state_buffers = (char*)calloc(num_of_cpus, PRNG_BUFFER_SIZE);

  pthread_t *threads = (pthread_t*)malloc(num_of_cpus*sizeof(pthread_t));
  
  // Start threads, and pass along the random state for the random number generation
  for (int thread_index = 0u; thread_index < num_of_cpus; thread_index++) {
    initstate_r(random(), &random_state_buffers[thread_index], PRNG_BUFFER_SIZE, &thread_data[thread_index].random_states);
    if (argc == 2)
      thread_data[thread_index].calculations_per_thread = atoi(argv[1]);
    else 
      thread_data[thread_index].calculations_per_thread = DEF_CALC_PER_THREAD;
    pthread_create(&threads[thread_index], NULL, thread_main, &thread_data[thread_index]);
  }
  // Join threads and add up the results
  int total_points = 0, points_in_circle = 0;
  for (int thread_index = 0; thread_index < num_of_cpus; thread_index++) {
    pthread_join(threads[thread_index], NULL);
    total_points += thread_data[thread_index].total_points;
    points_in_circle += thread_data[thread_index].points_in_circle;
  }

  printf("PI was approximated to be %.5f\n", (double)points_in_circle/total_points*4);
  free(threads);
  free(random_state_buffers);
  free(thread_data);

  return 0;
}
