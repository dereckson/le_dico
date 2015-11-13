/*
			GoodEnd: Nettoie les fins de chaines.
			Vite fait sur le gaz.
*/

#include <stdio.h>
#include <string.h>


main( int argc, char **argv) 
{
	FILE *fp,*ftmp;
	char buf[127]; 

	while( --argc )
        {
		if( (fp = fopen (argv[argc],"r")) == NULL )
		{
			printf("Unable to open file %s\n", argv[argc] );
			return 1 ;
		}

		if(( ftmp = fopen ("goodend.tmp", "w")) == NULL )
		{
			printf("Unable to open tempfile !\n") ;
			return 1 ;
		}

		while( fgets( buf, 126, fp ) ) 
		{
			buf[strlen (buf) -1] = '\0'; /* no line-feed */
			if( nogood( buf))
			{
				printf("Rejete: %s\n", buf);
				continue ;
			}
			fprintf(ftmp, "%s\n", buf);
		}
		
		fclose( fp );
		fclose( ftmp);

		unlink( argc[argv] );
		rename( "goodend.tmp", argc[argv] );
		 
	}
}

/*
		Rejette les chaines terminees par '-' et autres.
*/
nogood( char *str)
{
	char c;

	c = str[strlen(str) -1];

	switch ( c )
	{
		case '-':
		case ',':
		case '.':
		case '?':
		case '!':
			return 1 ;
		break ;

		default :
			return 0 ;
	} 
}
