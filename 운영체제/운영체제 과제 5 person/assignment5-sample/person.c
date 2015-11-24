/*
 * OS Assignment #5
 *
 * @file person.c
 */

#include "person.h"
#include <stdio.h>
#include <unistd.h>

static void
print_usage (const char *prog)
{
  fprintf (stderr, "usage: %s [-f file] [-w] [-s value] attr_name\n", prog);
  fprintf (stderr, "  -f file  : person file, default is './person.dat'\n");
  fprintf (stderr, "  -w       : watch mode\n");
  fprintf (stderr, "  -s value : set the value for the given 'attr_name'\n");
}

int
main (int    argc,
      char **argv)
{
  const char *file_name;
  const char *set_value;
  const char *attr_name;
  int         watch_mode;

  /* Parse command line arguments. */
  file_name  = "./person.dat";
  set_value  = NULL;
  watch_mode = 0;
  while (1)
    {
      int opt;

      opt = getopt (argc, argv, "ws:f:");
      if (opt < 0)
	break;

      switch (opt)
	{
	case 'w':
	  watch_mode = 1;
	  break;
	case 's':
	  set_value = optarg;
	  break;
	case 'f':
	  file_name = optarg;
	  break;
	default:
	  print_usage (argv[0]);
	  return -1;
	  break;
	}
    }
  if (!watch_mode && optind >= argc)
    {
      print_usage (argv[0]);
      return -1;
    }
  attr_name = argv[optind];

  /* not yet implemented */

  return 0;
}
