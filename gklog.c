#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "read-conf.h"

extern int errno;

// Program's name
#define PrgName "GKLog"
// Program's version
#define PrgVersion "0.7"

// Will the program generate 'header' output?
#define QUIET_DEFAULT 0

// default file to log's output
#define OUTPUT "/var/log/gklog"
// will we show the resulting output to stdout?
#define DEFAULT_SHOW 1

int Show = DEFAULT_SHOW;

/* will the log be shown entirely or will show only mathcing 
lines?*/
int restrict = 0;

/* The location of the config file will be handled in main() but 
this defines where is located the global configuration file */
#define GLOBALCONF "/etc/gklog.rules"
char conf[256];  // Default conffile's name

int ReadLog (char *file, char* Output)
{
    FILE* output; // output file
    FILE* log;
    char Line[1024];
    char c;
    int i;

    if (!strcmp(file, "-"))
    {
    	log = stdin;
    } // check if we're working with stdin
    else
    {
    	// original file
	    log = fopen (file, "r");
     }

    output = fopen (Output, "w");    // file to be created

    if (output == NULL)
    {
		perror("fopen(): output");
		return 1;
    }

    if (log == NULL)
    {
		perror("fopen(): log");
		return 1;
    }

    while (!feof(log))
    {
	bzero (Line, 1024); // fills line with \0's
	c = '\0'; // \0s the char so that there won't be
	// problens in case it is '\n'
	for (i = 0 ; c != '\n' ; i++)
	{
	    c = fgetc(log);
	    if (c == EOF) // if the file is over
		goto theend; // get out
	    Line[i] = c;
	}
	fflush(log);
	Line[i] = '\0';
    fflush(output);

	    if (Show == 1) // show to stdout
	    {
		if ( ProcessRules (Line) == 1 )
		{
			fprintf (stdout, "%s", Line);
    		fprintf (output, "%s", Line);
			fprintf(stdout, "\033[0m");
			fflush (output);
			fflush (stdout);
		}
		else
		{
			if (!restrict)
				fprintf(stdout, "%s", Line);
		}
	    }
    }



theend:
       if (strcmp (file, "-"))
       {
	    if ( (fclose (log)) == EOF )
	    {
		perror("fclose()");
	    }
	}
    if ( (fclose (output)) == EOF )
    {
	perror("fclose()");
    }

    return 0;
}

void Disclaimer (int quiet_run)
{
	if ( quiet_run == 0 )
	{
		printf ("%s version %s\n"
					"This program is Free Software, licensed under the GPL. You probably\n"
					"received a copy of it with this program, if not you may find it on\n"
					"http://www.fsf.org. This program has no warranties! Use it at your\n"
					"own risk!\n\n", PrgName, PrgVersion);
	}
}

void Help (const char *cmd)
{
	Disclaimer ( 0 );
    printf("Usage: %s <log_file> [options]\n\n", cmd);
    printf(
    	"Options:\n"
		"  -h || --help		 this message\n"
		"  -c || --config <file>	 use <file> instead of /etc/gklog.rules\n"
		"  -r || --restrict     	 restricts the output of non-matching lines\n"
		"  -q || --quiet          do not output program headers (disclaimer)\n"
		"  -o || --output <file>  write to <file> instead of /var/log/gklog\n\n"
	   );
    printf("There'll be the following defaults:\n");
    printf("      log:     /var/log/syslog\n");
    printf("      output: /var/log/gklog\n\n");
}

int main(int argc, char *argv[])
{
    int i;
    int quiet_run = QUIET_DEFAULT;
    char Output[1024];
    char logsname[255];
	char *UserConf = calloc (100, sizeof (char));

    if (argc <2)
    {
		Disclaimer (0);
		printf("Usage: %s <log_file> [options]\n\n", argv[0]);
        printf("There'll be the following defaults:\n");
        printf("      log:     /var/log/syslog\n");
        printf("      output: /var/log/gklog\n\n");
    }

    strcpy (logsname, "/var/log/syslog");
    strcpy (Output, OUTPUT);

	for (i = 0 ; i < argc ; i++)
	{
		if (!strcmp(argv[i], "-h") ||
			!strcmp (argv[i], "--help"))
		{
			Help (argv[0]);
			return 0;
		}
		if (!strcmp(argv[i], "-c") ||
			!strcmp (argv[i], "--config"))
		{
			i++;
			bzero (conf, 256);
			strcpy (conf, argv[i]);
		}
		if (!strcmp(argv[i], "-o") ||
			!strcmp (argv[i], "--output"))
		{
			i++;
			strcpy (Output, argv[i]);
		}
		if (!strcmp(argv[i], "-r") ||
			!strcmp (argv[i], "--restrict"))
		{
			restrict = 1;
		}
		if (!strcmp(argv[i], "-q") ||
			!strcmp (argv[i], "--quiet"))
		{
			quiet_run = 1;
		}

	}

	Disclaimer (quiet_run);

	if ( !strcmp (conf, "") )
	{
		UserConf = getenv ("HOME");
		strcat (UserConf, "/.gklog.rules");
	
		if ( fopen (UserConf, "r") != NULL )
		{
			strcpy (conf, UserConf);
			if (quiet_run == 0)
				fprintf (stderr, "Using user configuration: %s...\n", conf);
		}
		else
		{
			strcpy (conf, GLOBALCONF);
			if (quiet_run == 0)
				fprintf (stderr, "User configuration ($HOME/.gklog.rules) not found...\n"
							"Using global configuration: %s...\n", conf);
		}
	}
	else
	{
		if (quiet_run == 0)
			fprintf (stderr, "Using user-defined configuration: %s...\n", conf);	
	}

	if (quiet_run == 0)
		printf ("Starting analysis:\n"); // newline to separate header from output

    if (argc >= 2)
    {
		strcpy (logsname, argv[1]);
	}

	if(!strcmp (logsname, "-"))
	{	
		while (1)
		{
			if ( (ReadLog (logsname, Output)) == 1)
	    		{        	
				printf("Error reading log\n\n");
				return 1;
	  		}
	  	}
	  }
	  else
	  {
		if ( (ReadLog (logsname, Output)) == 1)
		{
		printf("Error reading log\n\n");
		return 1;
		}
	}

    return 0;

}
