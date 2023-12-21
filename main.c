#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef enum { false, true } boolean;


#define MAXPARMS 8


typedef struct {
  char *description;
  char *prototype;
  int numparms;
  enum { T_INT, T_FLOAT, T_STRING } const parmtype[MAXPARMS];
} func_t;

func_t *func;

typedef enum {
  FUNC_SPAWNOBJECT,
  FUNC_MONSTERPROJECTILE,

  NUMFUNCS
} funcs_t;

func_t funcs[NUMFUNCS] = {
  {
    "A_SpawnObject(type, angle, x_ofs, y_ofs, z_ofs, x_vel, y_vel, z_vel)",
    "A_SpawnObject(%s, %s, %s, %s, %s, %s, %s, %s)",
    8, { T_STRING, T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT  }
  },

  {
    "A_MonsterProjectile(type, angle, pitch, hoffset, voffset)",
    "A_MonsterProjectile(%s, %s, %s, %s, %s)%s%s%s",
    5, { T_STRING, T_FLOAT,  T_FLOAT,  T_FLOAT,  T_FLOAT  }
  },
};


float min[MAXPARMS], max[MAXPARMS];

// Taken from StackOverflow
float frandom(int index)
{
  return min[index] + ((rand() / (float) RAND_MAX) * (max[index] - min[index]));
}

char *parameters[MAXPARMS][BUFSIZ];

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
      snprintf(parameter, BUFSIZ, "PARM%i", index);
      break;
  }

  return parameter;
}


int main()
{
  int i;
  int numstates, func_index;
  FILE *fptr;

  for (i = 0;  i < MAXPARMS;  i++) { min[i] = max[i] = 0.0f; }

  srand(time(NULL));

  printf("Enter number of states [int]: ");
  scanf("%i", &numstates);
  puts("");

  printf("Available functions:\n");
  for (i = 0;  i < NUMFUNCS;  i++)
  { printf("[%i] %s\n", i, funcs[i].description); }
  puts("");

  printf("Select function [int]: ");
  scanf("%i", &func_index);
  puts("");

  func = &funcs[func_index];

  for (i = 0;  i < func->numparms;  i++)
  {
    if (func->parmtype[i] == T_STRING) { continue; }

    printf("Enter min%i [float]: ", i + 1);
    scanf("%f", &min[i]);

    printf("Enter max%i [float]: ", i + 1);
    scanf("%f", &max[i]);

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
      "      TNT1 A 1 %s\n"
      "      Stop\n",
      func->prototype
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

  while (getchar() != '\n');
  while (getchar() != '\n');

  return 0;
}
