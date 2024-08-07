#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } boolean;

boolean Get_String(char **getter)
{
  const size_t MAX_CHARS = 64;
  char input[MAX_CHARS];

  fgets(input, MAX_CHARS, stdin);

  if (!(strlen(input) - 1)) { return true; } // Don't count the newline

  *getter = strdup(input);
  return false;
}

boolean Get_Int(int *getter)
{
  char *input;

  if (Get_String(&input)) { goto error1; }

  const size_t input_size = strlen(input) - 1; // Don't count the newline

  boolean num   = false,
          minus = false;

  for (int i = 0;  i < input_size;  i++)
  {
    const char c = input[i];

    if ('0' <= c && c <= '9')
    { num = true; }
    else if (c == '-' && !minus && !i)
    { minus = true; }
    else
    { goto error2; }
  }

  if (!num) { goto error2; }

  *getter = atoi(input);
  free(input);
  return false;

  error2: free(input);
  error1: return true;
}

boolean Get_Float(float *getter)
{
  char *input;

  if (Get_String(&input)) { goto error1; }

  const size_t input_size = strlen(input) - 1; // Don't count the newline

  boolean num   = false,
          dot   = false,
          minus = false;

  for (int i = 0;  i < input_size;  i++)
  {
    const char c = input[i];

    if ('0' <= c && c <= '9')
    { num = true; }
    else if (c == '.' && !dot)
    { dot = true; }
    else if (c == '-' && !minus && !i)
    { minus = true; }
    else
    { goto error2; }
  }

  if (!num) { goto error2; }

  *getter = atof(input);
  free(input);
  return false;

  error2: free(input);
  error1: return true;
}


#define MAX_PARMS 8


typedef struct {
  const char name[32];
  const char description[80];
  const char prototype[40];
  const int  num_parms;
  const enum { T_INT, T_FLOAT, T_STRING } parm_type[MAX_PARMS];
} func_t;

const func_t *func;

typedef enum {
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

char parameters[MAX_PARMS][32];

char *Random(int index)
{
  char *const parameter = parameters[index];

  if (func->num_parms <= index)
  {
    snprintf(parameter, 32, "");
  }
  else switch (func->parm_type[index])
  {
    case T_INT:
      snprintf(parameter, 32, "%i", (int) round(Random_Float(index)));
      break;

    case T_FLOAT:
      snprintf(parameter, 32, "%0.1f", Random_Float(index));
      break;

    case T_STRING:
      snprintf(parameter, 32, "PARM%i", index + 1);
      break;
  }

  return parameter;
}


int main()
{
  #if 0
  #define TEST
  #endif

  FILE *fptr = fopen("./sd21-randomizer.txt", "w");

  if (!fptr) {
    puts("ERROR opening output file.");
    exit(1);
  }

  srand(time(NULL));

  int i;

  for (i = 0;  i < MAX_PARMS;  i++) { min[i] = max[i] = 0.0f; }

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

  for (i = 0;  i < NUM_FUNCS;  i++)
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

  for (i = 0;  i < func->num_parms;  i++)
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

  char format[256];

  snprintf(
    format, 256,
    "    Spawn%%i:\n"
    "      TNT1 A 1 %s%s\n"
    "      stop\n",
    func->name, func->prototype
  );

  for (i = 1;  i <= num_states;  i++)
  {
    char buffer[512];

    snprintf(
      buffer, 512,
      format,
      i, Random(0), Random(1), Random(2), Random(3), Random(4), Random(5), Random(6), Random(7)
    );

    printf("%s", buffer);
    fprintf(fptr, "%s", buffer);
  }

  fclose(fptr);

  printf(
    "\n"
    "Operation successful. Text file written.\n"
    "Please hit Enter to exit."
  );

  while (getchar() != '\n');

  return 0;
}
