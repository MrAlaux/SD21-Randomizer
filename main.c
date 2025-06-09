//
// Copyright (C) 2023-2025 Alaux (MIT License)
//

#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

bool Get_String(char *const getter, const size_t size)
{
  memset(getter, '\0', size);

  fgets(getter, size, stdin);

  if (getter[0] == '\0')
  {
    // 'ETX' received
    putchar('\n');
  }
  else { getter[strcspn(getter, "\n")] = '\0'; }

  if (getter[size - 2] != '\0')
  {
    // Excess characters received; discard said excess
    for (char c; (c = getchar()) != '\n' && c != EOF;);
  }

  return !strlen(getter);
}

bool Get_Int(int *const getter)
{
  const size_t MAX_LENGTH = 12; // "-2147483648" has 11 characters; add one for the null terminator
  char input[MAX_LENGTH];

  if (Get_String(input, MAX_LENGTH)) { return true; }

  const size_t input_size = strlen(input);

  bool num   = false,
       minus = false;

  for (int i = 0;  i < input_size;  i++)
  {
    const char c = input[i];

    if ('0' <= c && c <= '9')
    {
      num = true;
    }
    else if (c == '-' && !minus && !i)
    {
      minus = true;
    }
    else { return true; }
  }

  if (!num) { return true; }

  *getter = atoi(input);
  return false;
}

bool Get_Float(float *const getter)
{
  const size_t MAX_LENGTH = 33; // Not exact, but reasonable: 32 chars plus null terminator
  char input[MAX_LENGTH];

  if (Get_String(input, MAX_LENGTH)) { return true; }

  const size_t input_size = strlen(input);

  bool num   = false,
       dot   = false,
       minus = false;

  for (int i = 0;  i < input_size;  i++)
  {
    const char c = input[i];

    if ('0' <= c && c <= '9')
    {
      num = true;
    }
    else if (c == '.' && !dot)
    {
      dot = true;
    }
    else if (c == '-' && !minus && !i)
    {
      minus = true;
    }
    else { return true; }
  }

  if (!num) { return true; }

  *getter = atof(input);
  return false;
}


typedef enum type_e {
  T_INT,
  T_FLOAT,
  T_STRING,

  NUM_TYPES
} type_t;

#define MAX_PARMS 8

typedef struct {
  const char    name[32];
  const char    description[80];
  const char    prototype[40];
  const int     num_parms;
  const type_t  parm_type[MAX_PARMS];
} func_t;

const func_t *func;

typedef enum funcs_e {
  FUNC_SPAWNOBJECT,
  FUNC_MONSTERPROJECTILE,

  NUM_FUNCS
} funcs_t;

const func_t funcs[NUM_FUNCS] = {
  {
    "A_SpawnObject",
    "type, angle, x_ofs, y_ofs, z_ofs, x_vel, y_vel, z_vel",
    "(%s, %s, %s, %s, %s, %s, %s, %s)",
    8, { T_STRING, T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT  }
  },
  {
    "A_MonsterProjectile",
    "type, angle, pitch, hoffset, voffset",
    "(%s, %s, %s, %s, %s)%s%s%s",
    5, { T_STRING, T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT  }
  },
};


float min[MAX_PARMS], max[MAX_PARMS];

float Random_Float(int index)
{
  return min[index] + (((double) rand() / RAND_MAX) * (max[index] - min[index]));
}

char *Random(int index)
{
  #define PARAMETER_SIZE 32
  static char parameters[MAX_PARMS][PARAMETER_SIZE];

  char *const parameter = parameters[index];

  if (func->num_parms <= index)
  {
    snprintf(parameter, PARAMETER_SIZE, "");
  }
  else switch (func->parm_type[index])
  {
    case T_INT:
      snprintf(parameter, PARAMETER_SIZE, "%i", (int) round(Random_Float(index)));
      break;

    case T_FLOAT:
      snprintf(parameter, PARAMETER_SIZE, "%0.1f", Random_Float(index));
      break;

    case T_STRING:
      snprintf(parameter, PARAMETER_SIZE, "PARM%i", index + 1);
      break;
  }

  return parameter;
}


int main()
{
#if 0
  #define TEST
#endif

  #define PATH "./sd21-randomizer.txt"

  FILE *const file = fopen(PATH, "w");

  if (!file) {
    puts("ERROR opening \"" PATH "\".");
    return 1;
  }

  srand(time(NULL));

  memset(min, 0, sizeof(*min) * MAX_PARMS);
  memset(max, 0, sizeof(*max) * MAX_PARMS);

  int num_states;

  while (true)
  {
    printf("Enter number of states [int]: ");

  #ifndef TEST
    if (!Get_Int(&num_states) && num_states > 0)
    { break; }
  #else
    printf("%i\n", num_states = 16);
    break;
  #endif
  }

  puts("");

  printf("Available functions:\n");

  for (int i = 0;  i < NUM_FUNCS;  i++)
  { printf("[%i] %s(%s)\n", i, funcs[i].name, funcs[i].description); }

  puts("");

  int func_index;

  while (true)
  {
    printf("Select function [int]: ");

  #ifndef TEST
    if (!Get_Int(&func_index) && (0 <= func_index && func_index < NUM_FUNCS))
    { break; }
  #else
    printf("%i\n", func_index = 0);
    break;
  #endif
  }

  puts("");

  func = &funcs[func_index];

  for (int i = 0;  i < func->num_parms;  i++)
  {
    if (func->parm_type[i] == T_STRING) { continue; }

    while (true)
    {
      printf("Enter min%i [float]: ", i + 1);

    #ifndef TEST
      if (!Get_Float(&min[i]))
      { break; }
    #else
      printf("%f\n", min[i] = i + 1.0);
      break;
    #endif
    }

    while (true)
    {
      printf("Enter max%i [float]: ", i + 1);

    #ifndef TEST
      if (!Get_Float(&max[i]))
      { break; }
    #else
      printf("%f\n", max[i] = i + 2.0);
      break;
    #endif
    }

    puts("");
  }

  #define FORMAT_SIZE 256
  char format[FORMAT_SIZE];

  snprintf(
    format, FORMAT_SIZE,
    "    Spawn%%i:\n"
    "      TNT1 A 1 %s%s\n"
    "      stop\n",
    func->name, func->prototype
  );

  for (int i = 1;  i <= num_states;  i++)
  {
    #define BUFFER_SIZE 512
    char buffer[BUFFER_SIZE];

    snprintf(
      buffer, BUFFER_SIZE,
      format,
      i, Random(0), Random(1), Random(2), Random(3), Random(4), Random(5), Random(6), Random(7)
    );

    printf("%s", buffer);
    fprintf(file, "%s", buffer);
  }

  fclose(file);

  printf(
    "\n"
    "Operation successful: \"" PATH "\" written.\n"
    "Press Enter to exit."
  );

  while (getchar() != '\n');

  return 0;
}
