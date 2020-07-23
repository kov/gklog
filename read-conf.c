#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "read-conf.h"

extern int errno;
extern int restrict;
extern FILE* output;

	extern char conf[];
	FILE *file; // rule file to be opened

int ProcessLine (char *pattern, char* subpattern, char* cor, char*Line)
{
	int found;
	int i = 0 ,
		pa = 0,
		sa = 0,
		ca = 0;
	
	/* This sequence of code is to help allowing the comments on the config
	file... it allows you to add blank lines to that file */
	while (pattern[i] != '\0')
	{
		if (pattern[i] != ' ')
			pa = 1;
		i++;
	}
	i = 0;
	while (subpattern[i] != '\0')
	{
		if (subpattern[i] != ' ')
			sa = 1;
		i++;
	}
	i = 0;
	while (cor[i] != '\0')
	{
		if (cor[i] != ' ')
			ca = 1;
		i++;
	}

	if (pa == 0 && sa == 0 && ca == 0)
		return 0;
	else if (ca == 0)
		return 0;
	else if (pa == 0 && sa == 0)
		return 0;
	
	if ( strstr (Line, pattern)  && strstr (Line, subpattern) )
	{
		/* WOW!! Here we go to the longest IF list I've ever
		seen!! */
		printf ("\033[");
		
		if (!strcasecmp (cor, "green"))
			printf ("0;32");
		else if (!strcasecmp (cor, "blue"))
			printf ("0;34");
		else if (!strcasecmp (cor, "cyan"))
			printf ("0;36");
		else if (!strcasecmp (cor, "red"))
			printf ("0;31");
		else if (!strcasecmp (cor, "purple"))
			printf ("0;35");
		else if (!strcasecmp (cor, "brown"))
			printf ("0;33");
		else if (!strcasecmp (cor, "gray2"))
			printf ("0;37");
		else if (!strcasecmp (cor, "gray1"))
			printf ("0;30");
		else if (!strcasecmp (cor, "bblue"))
			printf ("1;34");
		else if (!strcasecmp (cor, "bgreen"))
			printf ("1;32");
		else if (!strcasecmp (cor, "bcyan"))
			printf ("1;36");
		else if (!strcasecmp (cor, "bred"))
			printf ("1;31");
		else if (!strcasecmp (cor, "bpurple"))
			printf ("1;35");
		else if (!strcasecmp (cor, "yellow"))
			printf ("1;33");
		else if (!strcasecmp (cor, "white"))
			printf ("1;37");
		else
			printf ("0");
		
		printf ("m");
	
		found = 1;
	}
         else
         		found = 0;
					
	return found;
}
			
int ProcessRules (char *Line)
{
	file = fopen (conf, "r");
	if (file == NULL)
	{
		perror ("Opening rules file");
		return 1;
	}
	
	fclose (file);
	
{ // starting block
	int 	i, a, b, c; //fors... so many?!
	char s[1024];
	/* don't know if this is the rightest thing to do... */
	char rule[3][1024];	// where we're going to store rules
	file = fopen (conf, "r");
	
	bzero (s, 1024);
		
	for (i = 0 ; !feof(file) ; i++)
	{
		s[i] = fgetc (file);
		if (s[i] == '&' || s[i] == '=')
				s[i] = ',';
		
		if (s[i] == '#')
		{
			if (s[i-1] == ' ') // if there're spaces before... eliminate it
			{
				while (s[i-1] == ' ')
					i--; // keep going back 'till all spaces have
						// been removed (found another char)
			}
			// If nothing has been read (rules) yet,
			// do not record a '\n' on it...
			if ( ! strstr (s, "|") && ! strstr (s, ",") )
			{
				i = -1;
			}
			else
			{
				if (s[i-1] == '|')
				{
					while ( (s[i] = fgetc(file)) != '\n' )
					{
						s[i] = '\0';
					}
					i--;
				}
				else if ( s[i-1] != '\n')
				{
					while ( (s[i] = fgetc(file)) != '\n' );
//					s[i] = '\n';
				}
			}
			// this gets next char untill it is a new line or a
			// end-of-file
			if ( s[i] != '|')
			{
				while ( s[i] != '\n' && !feof(file) && s[i] != '|')
					s[i] = fgetc (file);
			}
		}
		
		if (s[i] == '\n' || feof(file))
		{
			s[i] = '|';
		}
	}
	for ( ; i < 1024 ; i++)
		s[i]='\0';

	fclose (file);
	
	i = 0;
	while (s[i] != '\0')
	{	
		fflush (stdout);
		for (a = 0 ; a < 3 ; a++)
			bzero (rule[a], 1024);
		
		a = 0;
		b = 0;
                   				
		for ( ; s[i] != '|' ; i++)
		{
			if (s[i] != ',' && s[i] != '|' && s[i] != 'Ã')
			{
				rule[a][b] = s[i];
				b++;
			}
			else
			{
				rule[a][b] = '\0';
				b = 0;
				a++;
			}
		}
#ifdef DEBUG		
		printf("\033[1;37mDEBUG: begining - 1%s1\t2%s2\t3%s3 - end\n\033[0m", rule[0], rule[1], rule[2]);
		fflush(stdout);
#endif
		if ( ProcessLine (rule[0], rule[1], rule[2], Line) == 1)
			return 1;
					
		/* if reached '|' it could be end of file or could be
		 the begining of a new rule, so we increment it  so that
		 for will not break before the time we need it to*/
		if (s[i] == '|')
			i++;
	}
} // end of block
	return 0;
	
}
