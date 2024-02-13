#include <math.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

typedef struct STUDENT {
  int student_number;
  int arrival_time;
  int start_help_time;
  int helped_time;
  int help_duration;
} STUDENT;

typedef struct DATA {
  int total_students;
  int overtime;
  STUDENT students[100];
} DATA;

void printArray(int *arr, int size) {
  int i;
  for (i = 0; i < size; i++)
    printf("%d ", arr[i]);

  printf("\n");
}

void printStudents(STUDENT *arr, int t_s) {
  int i;
  printf("Current students: %d\n", t_s);
  for (i = 0; i < t_s; i++)
    printf(
        "arrived: %d, start help time: %d, help duration: %d, helped time: %d, student number: %d\n",
        arr[i].arrival_time, arr[i].start_help_time, arr[i].help_duration, arr[i].helped_time,
        arr[i].student_number);

  printf("\n");
}

int serviceRate() {
  int lowerLimit = 2;
  int upperLimit = 12;
  int num = (rand() % (upperLimit - lowerLimit + 1)) + lowerLimit;
  return num;
}

void new_minute(int *queue, int *queue_len, int *student_number,
                int current_time, STUDENT *students) {
  // average number of people that arrive each hour
  float arrival_rate = 8.0 / 100.0;
  int lowerLimit = 1;
  int upperLimit = 100;
  int max_arrival = lowerLimit + round(arrival_rate * 100);
  int num = (rand() % (upperLimit - lowerLimit + 1)) + lowerLimit;
  // printf("%d %d", max_arrival, num);
  if (num < max_arrival) {
    // a new student has arrived, will be added to queue
    // printf("A new student %d has arrived!\n", *student_number);
    queue[*queue_len] = *student_number;
    students[*student_number].student_number = *student_number;
    students[*student_number].arrival_time = current_time;
    students[*student_number].help_duration = serviceRate();
    students[*student_number].helped_time = 0;
    (*queue_len)++;
    (*student_number)++;
  }
}

void currentHelpedStudent(int *queue, int *queue_len, STUDENT *results, int current) {
  int student;
  int t;
  int b;
  if (*queue_len > 0) {
    // someone is waiting, go help them
    student = queue[0];
    if (*queue_len == 1 && results[student].helped_time == 0) {
      results[student].start_help_time = current;
    }
    
    if (results[student].helped_time == results[student].help_duration) {
      // student has been helped, remove from queue
      // printf("Student %d has been helped, next student\n", student);
      for (b = 0; b < (*queue_len - 1); b++) {
        queue[b] = queue[b + 1];
      }
      (*queue_len)--;
      // get new student to help this minute
      if (*queue_len > 0) {
        student = queue[0];
        results[student].start_help_time = current;
        // printf("will help student %d\n", student);
      } else {
        return;
      }
    }
    // help student
    t = results[student].helped_time;
    results[student].helped_time = t + 1;
  }
}

void helpRemaining(int *queue, int *queue_len, STUDENT *results,
                   int *overtime) {
  while (*queue_len > 0) {
    currentHelpedStudent(queue, queue_len, results, 60 + (*overtime));
    if (*queue_len > 0) {
      (*overtime)++;
    }
  }
}

DATA runOnce() {
  int student_number = 0;
  DATA data;
  STUDENT students[100];
  int queue[100];
  int queue_len = 0;
  int overtime = 0;

  for (int i = 0; i < 60; i++) { // 60 minutes in one hour
    new_minute(queue, &queue_len, &student_number, i, students);
    currentHelpedStudent(queue, &queue_len, students, i);
  }
  // after the hour, there might be students that have not been helped yet
  helpRemaining(queue, &queue_len, students, &overtime);

  data.overtime = overtime;
  data.total_students = student_number;
  for (int j = 0; j < student_number; j++) {
    data.students[j] = students[j];
  }
  return data;
}

void createStats() {
  /* Answer these questions for 100 loops:
  -The average time a student spends waiting during an office hour visit
  - The average time a student spends with the professor during an office hour
  visit
  - The average time a Professor spends at an office hour beyond the 1 hour he
  intended to
  */
  int simulations = 100;
  int student_count = 0;
  int total_student_wait_times = 0;
  int total_student_help_times= 0;
  int total_overtime = 0;
  DATA data;
  
  for (int i = 0; i < simulations; i++) {
    data = runOnce();
    printf("SIMULATION %d\n", i +1);
    printf("The professor had to remain an extra %d minutes\n", data.overtime);
    printStudents(data.students, data.total_students);
    for (int j = 0; j < data.total_students; j++) {
      int wait = data.students[j].start_help_time - data.students[j].arrival_time;
      total_student_wait_times += wait;
      total_student_help_times += data.students[j].help_duration;
    }
    total_overtime += data.overtime;
    student_count += data.total_students;
  }

  printf("Total student wait time: %d\n", total_student_wait_times);
  printf("Total student help time: %d\n", total_student_help_times);
  printf("Total overtime %d\n", total_overtime);
  printf("Total students %d\n", student_count);

  double average_student_wait_time = (double)total_student_wait_times / (double)student_count;
  double average_student_help_time = (double)total_student_help_times / (double)student_count;
  double average_overtime = (double)total_overtime / (double)simulations;

  printf("RESULTS for \n");
  printf("average student wait time: %f\n", average_student_wait_time);
  printf("average student help time: %f\n", average_student_help_time);
  printf("average overtime: %f\n", average_overtime);
}

int main(void) {
  srand(time(NULL)); // Initialization
  DATA data;

  // data = runOnce();
  // printf("The professor had to remain an extra %d minutes\n", data.overtime);
  // printStudents(data.students, data.total_students);

  createStats();
}
