#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } boolean;


#define MAXPARMS 8


typedef struct {
  const char name[32];
  const char description[80];
  const char prototype[40];
  const int  numparms;
  const enum { T_INT, T_FLOAT, T_STRING } parmtype[MAXPARMS];
} func_t;

const func_t *func;

typedef enum {
  FUNC_SPAWNOBJECT,
  FUNC_MONSTERPROJECTILE,

  NUMFUNCS
} funcs_t;

const func_t funcs[NUMFUNCS] = {
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


float min[MAXPARMS], max[MAXPARMS];

// Taken from StackOverflow
float frandom(int index)
{
  return min[index] + ((rand() / (float) RAND_MAX) * (max[index] - min[index]));
}

char parameters[MAXPARMS][BUFSIZ];

char *random(int index)
{
  char *const parameter = parameters[index];

  if (func->numparms <= index) {
    snprintf(parameter, BUFSIZ, "");
  }
  else switch (func->parmtype[index])
  {
    case T_INT:
      snprintf(parameter, BUFSIZ, "%i", (int) round(frandom(index)));
      break;

    case T_FLOAT:
      snprintf(parameter, BUFSIZ, "%0.1f", frandom(index));
      break;

    case T_STRING:
      snprintf(parameter, BUFSIZ, "PARM%i", index + 1);
      break;
  }

  return parameter;
}


int main()
{
  int i;
  int numstates, func_index;
  FILE *fptr;

  #if 1
  #define TEST
  #endif

  for (i = 0;  i < MAXPARMS;  i++) { min[i] = max[i] = 0.0f; }

  srand(time(NULL));

  printf("Enter number of states [int]: ");
  #ifndef TEST
  scanf("%i", &numstates);
  #else
  printf("%i\n", numstates = 16);
  #endif
  puts("");

  printf("Available functions:\n");
  for (i = 0;  i < NUMFUNCS;  i++)
  { printf("[%i] %s(%s)\n", i, funcs[i].name, funcs[i].description); }
  puts("");

  printf("Select function [int]: ");
  #ifndef TEST
  scanf("%i", &func_index);
  #else
  printf("%i\n", func_index = 0);
  #endif
  puts("");

  func = &funcs[func_index];

  for (i = 0;  i < func->numparms;  i++)
  {
    if (func->parmtype[i] == T_STRING) { continue; }

    printf("Enter min%i [float]: ", i + 1);
    #ifndef TEST
    scanf("%f", &min[i]);
    #else
    printf("%f\n", min[i] = i + 1.0);
    #endif

    printf("Enter max%i [float]: ", i + 1);
    #ifndef TEST
    scanf("%f", &max[i]);
    #else
    printf("%f\n", max[i] = i + 2.0);
    #endif

    puts("");
  }

  fptr = fopen("./sd21-randomizer.txt", "w");

  if (!fptr) {
    puts("ERROR opening file.");   
    exit(1);             
  }
  else {
    char format[BUFSIZ];

    snprintf(
      format, BUFSIZ,
      "    Spawn%%i:\n"
      "      TNT1 A 1 %s%s\n"
      "      Stop\n",
      func->name, func->prototype
    );

    for (i = 1;  i <= numstates;  i++)
    {
      char buffer[BUFSIZ];

      snprintf(
        buffer, BUFSIZ,
        format,
        i, random(0), random(1), random(2), random(3), random(4), random(5), random(6), random(7)
      );

      printf("%s", buffer);
      fprintf(fptr, "%s", buffer);
    }
  }

  fclose(fptr);

  printf(
    "\n"
    "Operation successful. Text file written.\n"
    "Please hit Enter to exit."
  );

  #ifndef TEST
  while (getchar() != '\n');
  #endif
  while (getchar() != '\n');

  return 0;
}
