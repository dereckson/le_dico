/*
        		Eclate.C -  Rene Cougnenc 1992

	Eclatement d'un lexique  en 26 fichiers a...z      

	Les lettres accentuees sont conservees et traitees comme leur
	equivalent non accentue. ( en Francais )

	Les mots sont passes automatiquement en minuscules.
	Un mot commencant par autre chose qu'une lettre est ignore.
	
	Attention, ce fichier source contient des caracteres 8 bits,
	afin de le rendre portable entre les differents jeux de
	caracteres 8 bits accentues des differentes machines.

	Si votre compilateur ne les accepte pas, vous etes mal :-)) 
*/

#define UNIX    /* si strlwr n'est pas dans la librairie uniquement */

#include <stdio.h>
#include <string.h>

typedef unsigned char byte ; 

#define EXT_DIC  ".dic"  /* extension donnée aux fichiers a...z */

#define ERROR 1
#define OK    0

byte *StrLwr ( byte *str ); /* special accentuated routines */
byte *StrUpt ( byte *str );
byte equival ( byte ch );

main(int argc, char **argv)
{
FILE *fp ;
byte buf[80] ;

	if( argc < 2 )
		return usage() ;

	if( (fp = fopen (argv[1], "r")) == NULL )
	{
		printf("Impossible de lire le fichier %s\n", argv[1] );
		return 1 ;
	}

	while( fgets(buf, 70, fp)  )
	{

		if (addword( buf ) )
		   return ERROR ;

	}

fclose( fp );
return OK ;
}


/*--------------------------------------------------------------------------*/
usage()
{
	printf("ECLATE: R.Cougnenc 1992\n\n");
	printf("	Usage: eclate dico.file\n");
	printf("	Eclate le dico en 26 fichiers [a..z].%s\n",EXT_DIC);
	printf("	Les lettres accentuees sont conservees, mais sont considerees\n");
	printf("	comme leur equivalent non accentue ( Francais )\n\n");
	printf("	Programme surement tres lent car vite fait :-)\n");
	return ERROR ;
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 *  addword(): Rajoute un mot dans son fichier correspondant a son initiale.
 *        
 *
 */
addword( byte *str )
{ 
 char fname[127] ;
 FILE *fp ;
 byte car;

	StrLwr(str);                 /* passe en minuscules            */
	car =  equival( str[0] );     /* prend l'initiale non accentuee */
/*
	printf("Chaine: %s  str[0]: %c   car: %c\n", (char*)str, str[0], car );
	return OK;
*/

	sprintf(fname, "%c%s", car, EXT_DIC );

	if( car < 'a'  || car > 'z' ) /* Ligne ignoree.                 */
	{
		printf("Ignore: %s", str );
		return OK ; 
	}

	if( ( fp = fopen( fname, "a")) == NULL )
	{
	  	printf("Impossible ouvrir fichier %s\n", fname ); 
	  	return ERROR  ;
	}
	
	fprintf(fp,"%s", str );
	fclose (fp );

return OK;
}

/*--------------------------------------------------------------------------*/




/*--------------------------------------------------------------------------*/
/*
 *  equival( ch ) :   Renvoie le caractere non accentue equivalent a une
 *		      lettre accentuee minuscule, en Francais.	
 *
 *		      ATTENTION cette fonction contient dans le code des
 *		      caracteres 8 bits !!!!!!!!!!	
 */
byte equival( byte ch )
{
	switch ((char) ch )
	{
		case 'à':
		case 'â':
		case 'ä':
			return 'a' ;

		case 'è':
		case 'é':
		case 'ê':
		case 'ë':
			return 'e' ;

		case 'î':
		case 'ï':
			return 'i' ;

		case 'ô':
		case 'ö':
			return 'o' ;

		case 'ù':
		case 'û':
		case 'ü':
			return 'u' ;

		case 'ç':
			return 'c' ;

		default:
			return ch ;  /* Pas de changement */
	}
}
/*--------------------------------------------------------------------------*/


#include <stdio.h>
#include <string.h>
#include <ctype.h>
 

/*
 *    ToLower: special tolower 8bits, don't touch 8 bit chars.
 *
 *
 */
byte ToLower (byte c )
{
	return ( c >='A' && c <= 'Z'     ?   c + 32  :   c );
}
 
/*
 *  StrLwr:  Special strlwr routine, works whith 8 bit chars ISO/IBM,
 *           do not modify these characters.
 *
 */
byte *StrLwr( byte *str )
{
 byte *p = str ;

	while( *str)
 	{
		*str= ToLower(*str);
		str++;
	}
 return p ;
}


/*
 *    ToUpper: special toupper 8bits, don't touch 8 bit chars.
 *
 *
 */
byte ToUpper (byte c )
{
	return ( c >='a' && c <= 'z'     ?   c - 32  :   c );
}
 
/*
 *  StrUpr:  Special strupr routine, works whith 8 bit chars ISO/IBM,
 *           do not modify these characters.
 *
 */
byte *StrUpr( byte *str )
{
 byte *p = str ;

	while( *str)
 	{
		*str= ToUpper(*str);
		str++;
	}
 return p ;
}

#ifdef NONONONONNONONN 
stricmp(s1, s2)
char *s1, *s2;
{

  while (1)
  {
        if (toupper(*s1) != toupper(*s2))
            return(( toupper(*s1) - toupper(*s2) ));

        if (*s1 == 0)
            return(0);

        s1++;
        s2++;
  }

}



int strnicmp(s1, s2,n)
char *s1, *s2;
int n;
{
 int count = 0 ;

  while (count < n)
  {
        if (toupper(*s1) != toupper(*s2) )
            return(( toupper(*s1) - toupper(*s2) ));

        if (*s1 == 0)
            return(0);

        s1++;
        s2++;
        count ++ ;
  }
  return( 0 );
}

#endif 

