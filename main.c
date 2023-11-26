#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PARMCOUNT 6

float min[PARMCOUNT], max[PARMCOUNT];

float random(int index)
{
  return min[index] + (rand() / (float) RAND_MAX) * (max[index] - min[index]);
}

int main()
{
  FILE *fptr;
  int val, parms;
  int i;

  for (i = 0;  i < PARMCOUNT;  i++) { min[i] = max[i] = 0.0; }

  srand(time(NULL));

  printf("Enter val (int): ");
  scanf("%i", &val);
  printf("\n");

  printf("Parameters (0 = All, 1 = Ofs, 2 = Vel): ");
  scanf("%i", &parms);
  printf("\n");

  parms = (parms < 0 ? 0 :
           parms > 2 ? 2 : parms);

  if (parms == 2) { i = 3; }
  else            { i = 0; }

  for (;  i < PARMCOUNT;  i++)
  {
    if (i == 3 && parms == 1) { break; }

    printf("Enter min%i (float): ", i+1);
    scanf("%f", &min[i]);

    printf("Enter max%i (float): ", i+1);
    scanf("%f", &max[i]);

    printf("\n");
  }

  fptr = fopen("./sd21-randomizer.txt", "w");

  if (!fptr) {
    printf("ERROR opening file.\n");   
    exit(1);             
  }
  else for (i = 1;  i <= val;  i++)
  {
    char buffer[BUFSIZ];

    if (parms == 2)
    {
      snprintf(buffer, BUFSIZ,
               "    Spawn%i:\n"
               "      TNT1 A 1 A_SpawnObject(thing, 0.0, 0.0, 0.0, 0.0, %0.1f, %0.1f, %0.1f)\n"
               "      Stop\n",
               i, random(3), random(4), random(5)
              );
    }
    else if (parms == 1)
    {
      snprintf(buffer, BUFSIZ,
               "    Spawn%i:\n"
               "      TNT1 A 1 A_SpawnObject(thing, 0.0, %0.1f, %0.1f, %0.1f)\n"
               "      Stop\n",
               i, random(0), random(1), random(2)
              );
    }
    else
    {
      snprintf(buffer, BUFSIZ,
               "    Spawn%i:\n"
               "      TNT1 A 1 A_SpawnObject(thing, 0.0, %0.1f, %0.1f, %0.1f, %0.1f, %0.1f, %0.1f)\n"
               "      Stop\n",
               i, random(0), random(1), random(2), random(3), random(4), random(5)
              );
    }

    printf("%s", buffer);
    fprintf(fptr, "%s", buffer);
  }

  fclose(fptr);

  printf("\n"
         "Operation successful. Text file written.\n"
         "Please hit Enter to exit.");

  while (getchar() != '\n');
  while (getchar() != '\n');

  return 0;
}
