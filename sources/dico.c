#define LINUX

/*
    +-----------------------------------------------------------------------+
    |                                                                       |
    |           DICO.C R.Cougnenc 1992 - Domaine Public                     |
    |           ---------------------------------------                     |
    |                                                                       |
    |                                                                       |
    |       Le dictionnaire est organise en 26 fichiers, un                 |
    |       pour chaque lettre. Ceci constitue un petit arbre               |
    |       largement suffisant.                                            |
    |                                                                       |
    |       Si le mot commence par une lettre, on recherche                 |
    |       dans le  fichier correspondant, a la maniere de                 |
    |       grep un peu.                                                    |
    |                                                                       |
    |       S'il commence  par une expression reguliere, on                 |
    |       recherche dans  les  26  fichiers  en totalite.                 |
    |       C'est toutefois d'une vitesse supportable.                      |
    |                                                                       |
    +-----------------------------------------------------------------------+
*/
#ifndef LINUX
#ifndef sony
#ifndef MSDOS
   #error "Vous devez definir le systeme d'exploitation LINUX,sony ou MSDOS."
#endif
#endif
#endif

#ifdef sony                             /* Sony News WorkStations        */
#define UNIX
#define ISO_CHARS
#define MYOS "SONY"                     /* Pour affichage version only.  */
char *getenv();
#endif

#ifdef LINUX                            /* Linux Operating System        */ 
#define UNIX
#define ISO_CHARS
#define MYOS "LINUX" 
#endif

#ifdef MSDOS                            /* Systeme d'operation de disque */
#define MYOS "MSDOS"                    /* Microsoft :-)                 */
#endif

#define IBM_TERM    1                   /* Pour translation affichage      */
#define ASCII_TERM  2
#define ISO_TERM    3
#define COMMENT_CHAR 35                 /* '#' ligne de commentaire        */

#define DICT_PATH "/usr/local/lib/dico"  /* path par defaut unix/dos        */
#define DOS_PATH  "lexique"              /* path par defaut Dos uniquement  */
#define DICT_EXT ".dic"                  /* extension des fichiers dicos    */
#define VERSION_FILE "version.dic"       /* fichier texte version dico      */

#define ERROR 1
#define OK    0
#define MYVBUF 10240                     /* pour setvbuf() eventuel         */

#ifndef LINUX
#ifndef sony
#define MUST_HAVE_BUF                  /* Linux va moins vite avec setvbuf! */
#endif
#endif

typedef unsigned char byte;

#include <stdio.h>
#include <string.h>
#ifndef sony
#include <stdlib.h>
#endif

                                  /* protos fonctions speciales accents... */
byte ToLower( byte c );
byte ToUpper( byte c );
byte *StrUpr ( byte *str);
byte *StrLwr ( byte *str);
byte *StrAscii( byte *str);
byte equival( byte c  );
byte *beautify( byte  *str);
void lookfor();
void lookall();
#ifdef MSDOS
void GetDosPath(char *path);
void TakePath  (char *path);
#endif

#ifdef MSDOS
    char *sep = "\\";
#else
    char *sep = "/" ;
#endif

/*------------------------------------------------------------------------*/
/*             Ici seul endroit pour modifier le numero de version:       */
/*                                                                        */
            char *__Version__ ="1.1" ;
/*                                                                        */
/*------------------------------------------------------------------------*/

char dict_path[127] ;
int myterm = 0     ;    /* translation terminal pour accents */
long count = 0 ;




/*------------------------------------------------------------------------*/
/*
 *           Code...
 */

main( int argc, char **argv )
{
 char *dic,*term ;

    if( argc < 2 )
        return usage();

#ifdef MSDOS
    GetDosPath( argv[0] );
#else
    if( (dic = getenv("DICO")) == NULL )       /* ou est le lexique ? */
        strcpy(dict_path, DICT_PATH ) ;        /* default path        */
    else
        strcpy(dict_path, dic );               /* user env path       */
#endif

    if( (term = getenv("DICOTERM")) != NULL)   /* Faut-il traduire ?  */
    {
        if( !strcmp(term, "IBM") )
            myterm = IBM_TERM ;
        else 
        if( !strcmp(term, "ASCII") )
            myterm = ASCII_TERM ;
        else
        if( !strcmp(term, "ISO") )
            myterm = ISO_TERM ;
        else
        printf("DICO: getenv %s non valide. Terminal inchange\n",term );
    }

    if(argv[1][0] =='-')                      /* option ligne commande?*/
        return   options( argv[1][1]  );

    mybanner();                               /* on dit bonjour...     */

    while( --argc)                            /* Allez, roulez !       */
    {
        if( isjoker (argv[argc][0] ) )
            lookall( argv[argc] );
        else
            lookfor( argv[argc] );
    }

    fprintf(stderr,"\nTrouve %ld mot%s\n",count, count > 1 ? "s" :"" );

       return OK ;
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 * file_usage:   message d'erreur et explications sur la recherche 
 *               du path du dictionnaire.           
 *
 *
 */

file_usage()
{
#ifndef MSDOS        /* Cas normal */

    fprintf(stderr,"\nLe dictionnaire doit etre installe par defaut " );
    fprintf(stderr,"dans le repertoire:\n\n");
    fprintf(stderr,"    %s\n\n",DICT_PATH);
    fprintf(stderr,"S'il se trouve ailleurs (par exemple sous MS-DOS...,");
    fprintf(stderr," vous devez\n");
    fprintf(stderr,"definir une variable d'environnement nommee DICO "); 
    fprintf(stderr,"decrivant le path\n");
    fprintf(stderr,"complet d'acces aux fichiers.\n\n");
    fprintf(stderr,"Exemple unix : export DICO=/usr/rene/dico\n");
    fprintf(stderr,"Exemle ms-dos: set DICO=c:\\utils\\dico\n\n");

#else               /* Cas de ms-dos */

    fprintf(stderr,"\nLe dictionnaire doit etre installe par defaut" );
    fprintf(stderr," dans l'un des\nrepertoires suivants:\n\n");

    fprintf(stderr,"    - Dans un repertoire precise par la variable\n");
    fprintf(stderr,"      d'environnement nommee DICO,\n\n");

    fprintf(stderr,"    - Un sous-repertoire  'lexique' a l'endroit\n");
    fprintf(stderr,"      ou se trouve le programme 'dico.exe',\n\n");

    fprintf(stderr,"    - Dans le repertoire /usr/local/lib/dico par\n");
    fprintf(stderr,"      compatibilite avec la version normale UN*X,\n\n");

    fprintf(stderr,"    - Dans le repertoire courant.\n\n");
#endif
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 *  Usage: Message d'aide generale.
 *
 *
 */
usage()
{
    fprintf(stderr,"\n\nDico - R.Cougnenc 1992 - Recherche dans lexique ");
    fprintf(stderr,"Francais - V %s (%s)\n",__Version__, MYOS );
    fprintf(stderr,"----------------------------------------------------" );
    fprintf(stderr,"--------------------\n\n");

    fprintf(stderr,
        "Usage: dico pattern1 pattern2 ... patternN\n\n");
    fprintf(stderr,
        "Pattern peuvent etre soit des mots entiers, soit des expressions\n");
    fprintf(stderr,
        "contenant les caracteres de remplacement suivants:\n\n");
    fprintf(stderr,
                "        *    remplace un nombre quelconque de caracteres\n");
    fprintf(stderr,
                "        ?    remplace un caractere a sa position\n");
    fprintf(stderr,
                "       []    remplace un ensemble de caracteres\n");
    fprintf(stderr,
                "      [^]    exclut un ensemble de caracteres\n\n");

    fprintf(stderr, 
        "Exemple: dico encu[^l]*\n\n") ;
    fprintf(stderr,"Attention: Sous Un*x vous devez mettre les expressions " );
    fprintf(stderr,"entre quotes\n");
    fprintf(stderr,"afin qu'elles ne soient pas expansees par le shell !!!\n\n");
    fprintf(stderr,"Options: -v affiche la  version du lexique\n");
    fprintf(stderr,"         -c compte les mots qu'il contient\n\n");
    return ERROR;
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 *  options():  Traite les options speciales de la ligne de commande.
 *              Pour l'instant, il n'y en que deux :-)
 *
 */
options( char *c )
{
 char fname[127] ;
 char buf[80];
 FILE *fp ;

    switch ( (int) c )
    {
        case 'v': /* Affiche la date mise a jour dico, fichier version.dic */

            sprintf(fname, "%s%s%s", dict_path, sep, VERSION_FILE ) ;

            if(( fp = fopen(fname,"r")) == NULL )
            {
                fprintf(stderr,"Impossible de lire fichier %s\n", fname );
                file_usage();   
                return ERROR;
            }

            mybanner();

            while( fgets(buf,70,fp ))
                printf("%s", buf );
          return OK;


         case 'c':
            return countwords();

        default:
            fprintf(stderr, "option -%c non valide\n",c);
            return ERROR ;
    }
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 *  countwords(): compte le nombre de mots dans le dico actuel.
 *                Lent mais on s'en tape...
 *
 *
 */
 countwords()
 {
    char fname[127];
    char buf[90];
    byte letter ;
    FILE *fp ;
    long partial_town, total_town ;
    long partial_word, total_word ;

    total_town = partial_town = total_word = partial_word = 0L ;

    for( letter = 'a' ; letter <= 'z' ; letter++ )
    {
        sprintf(fname, "%s%s%c%s",dict_path, sep, letter, DICT_EXT );
        if(( fp = fopen(fname,"r")) == NULL )
        {
            fprintf(stderr,"Impossible de lire fichier %s\n",fname );
            file_usage();
            return ERROR;
        }

#ifdef MUST_HAVE_BUF
        setvbuf( fp, NULL, _IOFBF, MYVBUF );
#endif

        fprintf(stderr,"Compte la lettre %c...", letter );
	fflush( stderr );
        while( fgets(buf,89,fp) )
        {
            if((byte) *buf == COMMENT_CHAR )  /* Ligne de commentaire     */
                continue ;
	    if( strchr( buf, '\t') )
		partial_town ++ ;
	    else
            	partial_word ++ ;
        }

        fclose( fp );
	fprintf(stderr, "\r" );
	fflush( stderr);
        printf("%8ld mots et %4ld communes dans la lettre %c.\n", 
				partial_word, partial_town,letter) ;
	fflush( stdout );
        total_word += partial_word ;
	total_town += partial_town  ;
        partial_word = partial_town = 0L ;
    }

    printf("\n%8ld mots et %ld communes au total.\n", total_word, total_town);
    return OK ;
 }
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/* 
 *  lookfor: Recherche dans le fichier de l'initiale correspondante
 *           de l'expression pattern.
 */
void
lookfor ( char *pattern )
{
 FILE *fp ;
 char fname[127];
 char buf[80];
 char sbuf[80];
 byte car ;
    
    StrLwr( pattern );  /* passe en minuscules comme le dico */
    car = equival (pattern[0]);
    fprintf(stderr, "Recherche %s dans la lettre %c... \n\n", pattern, car) ;

    sprintf(fname, "%s%s%c%s", dict_path, sep, pattern[0], DICT_EXT ) ;

    if(( fp = fopen(fname,"r")) == NULL )
    {
        fprintf(stderr,"Impossible de lire fichier %s\n", fname );
        file_usage();   
        return ;
    }

#ifdef MUST_HAVE_BUF
    setvbuf( fp, NULL, _IOFBF, MYVBUF );
#endif

    StrAscii( pattern);          /* Passe en non accent-sensitive */
    while( fgets(buf,79,fp) )
    {
        if( *buf == '#' )        /* Ligne de commentaire          */
            continue ;
        nolf( buf );             /* supprime le line-feed */
        strcpy( sbuf, buf );     /* No accent sensitive   */
        StrAscii( sbuf);
        if( fmatch(sbuf,pattern) )
        {
            translate( buf ); /* gere terminaux....   */
            printf("\t%s\n", beautify(buf) );
            count ++ ;
        }
    }

    fclose( fp) ;
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 *  lookall:  Recherche dans TOUS les 26 fichiers de l'expression pattern.
 *            Sûrement 26 fois plus lent que l'autre pour la lettre z :-)
 *
 */
void
lookall ( char *pattern )
{
 FILE *fp ;
 char fname[127];
 char buf[80];
 char sbuf[80];
 byte letter;
 

    StrLwr( pattern ); /* passe en minuscules comme le dico */

    fprintf(stderr,"Recherche %s dans TOUT le dictionnaire...\n\n",pattern);

    StrAscii( pattern);          /* Passe en non accent-sensitive */

    for( letter = 'a' ; letter <= 'z' ; letter++ )
    {
        sprintf(fname, "%s%s%c%s",dict_path, sep, letter, DICT_EXT );
        if(( fp = fopen(fname,"r")) == NULL )
        {
            fprintf(stderr,"Impossible de lire fichier %s\n",fname );
            file_usage();
            return ;
        }

#ifdef MUST_HAVE_BUF
        setvbuf( fp, NULL, _IOFBF, MYVBUF );
#endif

        while( fgets(buf,79,fp) )
        {
            if( *buf == '#' )        /* Ligne de commentaire          */
                continue ;
            nolf( buf );              /* supprime le line-feed */
            strcpy( sbuf, buf);       /* no accent sensitive  */
            StrAscii( sbuf);
            if( fmatch(sbuf,pattern) )
            { 
                translate( buf ) ;
                buf[0] = ToUpper( buf[0] ) ;  
                printf("\t%s\n", beautify (buf) );
                count ++ ;
            } 
        }

        fclose( fp );
    }
    
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 * mybanner(): Affiche le nom et numero de version du programme.
 *
 *
 */

mybanner()
{
    printf("\nDICO V %s (%s) - R.Cougnenc 1992\n\n", __Version__, MYOS);
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 * translate( buf ): Regarde s'il faut afficher en ISO, IBM ou ASCII pur
 *                   et modifie buf en consequence.
 *
 */
translate( char * buf)
{
    switch ( myterm )
    {
        case IBM_TERM :
            StrIBM ( buf );
        break ;
        
        case ASCII_TERM :
            StrAscii( buf );
        break ;

        case ISO_TERM :
            StrISO ( buf );
        break ;
    }
} 
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 *  beautify():         Modifie une chaine pour la rendre plus lisible:
 *                      La premiere lettre est passee en majuscule.
 *                      Si c'est un code postal, on met des majuscules a
 *                      chaque mot ( c'est mieux que rien finalement ),
 *                      et on remplace la tabulation par un formattage
 *                      correct.
 *                      La fonction renvoie un pointeur sur une chaine
 *                      STATIQUE, ecrasee a chaque appel, contenant le
 *                      joli resultat :-)
 */
byte *
beautify ( byte * str )
{
   static byte result[ 80 ];
   byte ville[50], code[6];
   byte *r = ville;

   *str = ToUpper ( *str );                 /* Premiere lettre...     */

    if( ! strchr( str, '\t') )               /* Cas du nom commun      */
    {
 	strcpy( result, str );
 	return result ;
    }
                                           /* Cas du nom propre      */
    
    while( *str )
    {
	if( *str == '\t' )                /* separation ville/code   */    
	{
		*r = '\0';
		strcpy( code, str +1 );
		break ;
	}
	*r++ = *str++ ;
	if( *(str -1 ) == ' '|| *(str -1 ) == 39 ) /* apostrophe */
		*str = ToUpper ( *str );
    }
	
    sprintf(result, "%-30s %s", ville, code );
    return result ;
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 *   is joker: Retourne 0 si le caractere n'est pas un truc pour
 *            un debut d'expressions regulieres.
 *
 */
isjoker( int c ) 
{
    switch( c )
    {
        case '[':
        case '?':
        case '*': 
            return 1 ;
    }

    return 0  ;
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 * nolf: Supprime un line feed a la fin d'une chaine.
 *
 */
nolf( char *str) 
{
    while( *str )
    {
      if( *str == '\n')
        *str = '\0' ;
      str++;
    }
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
    Fonction : int fmatch(char *string,char *pattern);

    Gestion simplifiee des expressions regulieres, afin d'etre simple
    et compatible MS-DOS ou autres systemes.
*/
                                                /* d'apres freeware qnx */


#define MATCH   1
int match(char *_fname, char *_pat);

int fmatch(char *_fname, char *_pat)
{
    register char *pat, *fname;

    pat = _pat;
    fname = _fname;

    if(*pat++ == '*') {
        if(*pat == '\0')
            return(MATCH);

        while(*fname)
            if(match(fname++, pat))
                return(MATCH);

        return(0);
    }
    else
        return(match(fname, pat + -1));
}


match(char *_fname, char *_pat)
{
    register char *pat, *fname;
    char not_flag, found_flag;

    pat = _pat;
    fname = _fname;

    while(*pat && *fname) {
        switch(*pat) {

        case '?':
            break;

        case '[':
            not_flag = (*++pat == '^')||(*pat == '!') ;
            found_flag = 0;

            while(*pat  &&  *pat++ != ']')
            {
                if(*(pat-1) == *fname)
                    found_flag = 1;
/* SD : traitement des [a-z] */
                 else if (*pat =='-'){
                    char ch, chorg ;
                    int incr ;
                    chorg = *(pat-1) ;
                    pat++ ;
                    if (*pat){
                      incr = (*pat > chorg) ? 1 : -1 ;
                      for (ch=chorg;ch!= *pat + incr ;ch+= incr ){
                         if (ch==*fname) {
                           found_flag = 1 ; break ;
                         }
                       } /* fin for ch */
                       pat++ ;
                    } /* fin if *pat */
                 } /* fin if */

            }

            if(found_flag == not_flag)
                return(0);

            --pat;
            break;

        case '*':
            return(fmatch(fname, pat));

        default:
            if(*pat != *fname)
                return(0);
        }

        ++pat;
        ++fname;
    }

 return((*pat == 0 || (*pat == '*' && *++pat == 0))
         &&  *fname == 0 ? MATCH : 0);
}
/*----------------------( fin de fmatch() )--------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 *    ToLower: special tolower 8bits, don't touch 8 bit chars.
 *
 *
 */
byte ToLower (byte c )
{
    return ( c >='A' && c <= 'Z'     ?   c + 32  :   c );
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
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
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/*
 *    ToUpper: special toupper 8bits, don't touch 8 bit chars.
 *
 *
 */
byte ToUpper (byte c )
{
    return ( c >='a' && c <= 'z'     ?   c - 32  :   c );
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
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

/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 * strIBM:  Translates an ISO accentuated string to an MS-DOS one.
 *
 *
 */
StrIBM ( byte *str )
{
    while ( *str )
    {
        iso2ibm( str++ );
    } 
}
/*-----------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 * strISO:  Translates an MS-DOS accentuated string to a ISO one.
 *
 *
 */
StrISO ( byte *str )
{
    while ( *str )
    {
        ibm2iso( str++ );
    } 
}
/*-----------------------------------------------------------------------*/


/*-----------------------------------------------------------------------*/
/*
 *   ibm2iso ( c):  Translates ibm accentuated chars by ISO char.
 *   iso2ibm ( c )  Translates iso accentuated chars to IBM ones.
 *
 *     The character is modified !!!!  (Pointer )
 *
 * (c) Rene Cougnenc 1990
 */

#define NONE    0xfa    /* Car IBM pour remplacer les inexistants */

static unsigned char IsoTable[256]=
{
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
  0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,NONE,
  0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,
  0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
  0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
  0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,
  0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
  0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
  0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,
  0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
  0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,
  0x79,0x7a,0x7b,0x7c,0x7d,0x7e,NONE,NONE,NONE,NONE,NONE,
  NONE,NONE,NONE,NONE,NONE,NONE,NONE,NONE,NONE,NONE,NONE,
  NONE,NONE,0x60,0x27,NONE,NONE,NONE,NONE,NONE,NONE,NONE,
  NONE,NONE,NONE,NONE,NONE,NONE,0x20,0xad,0x9b,0x9c,0xfe,
  0x9d,0xb3,0x15,NONE,NONE,0xa6,0xae,0xaa,0x2d,NONE,0xc4,
  0xf8,0xf1,0xfd,NONE,0x27,0xe6,0xe3,NONE,0x2c,NONE,0xb1,
  0xaf,0xac,0xab,NONE,0xa8,0x41,0x41,0x41,0x41,0x8e,0x8f,
  0x92,0x80,0x45,0x90,0x45,0x45,0x49,0x49,0x49,0x49,0x44,
  0xa5,0x4f,0x4f,0x4f,0x4f,0x4f,0x78,0xed,0x55,0x55,0x55,
  0x55,0x59,NONE,0xe1,0x85,0xa0,0x83,0x61,0x84,0x86,0x91,
  0x87,0x8a,0x82,0x88,0x89,0x8d,0xa1,0x8c,0x8b,NONE,0xa4,
  0x95,0xa2,0x93,0x6f,0x94,0xf6,0xed,0x97,0xa3,0x96,0x81,
  0x79,NONE,0x98,
};

#define NOPE    0xb7    /* Car iso pour remplacer les inexistants */

static unsigned char TableIso[256]=
{
  0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
  0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0xa7,
  0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,
  0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
  0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
  0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,
  0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
  0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
  0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,
  0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
  0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,
  0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x20,0xc7,0xfc,0xe9,0xe2,
  0xe4,0xe0,0xe5,0xe7,0xea,0xeb,0xe8,0xef,0xee,0xec,0xc4,
  0xc5,0xc9,0xe7,0xc6,0xf4,0xf6,0xf2,0xfb,0xf9,0xff,0xd6,
  0xdc,0xa2,0xa3,0xa5,NOPE,NOPE,0xe1,0xed,0xf3,0xfa,0xf1,
  0xd1,0xaa,0xba,0xbf,0xaf,0xac,0xbd,0xbc,0xa1,0xab,0xbb,
  NOPE,NOPE,NOPE,0xa6,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,
  NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,
  NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,
  NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,
  NOPE,NOPE,NOPE,NOPE,NOPE,0xdf,NOPE,0xb6,NOPE,NOPE,0xb5,
  NOPE,NOPE,NOPE,NOPE,NOPE,NOPE,0xf8,NOPE,NOPE,NOPE,0xb1,
  NOPE,NOPE,NOPE,NOPE,0xf7,NOPE,0xb0,0xb7,0xb7,NOPE,NOPE,
  0xb2,0xa4,0x20
};

iso2ibm( unsigned char *c )
{
    *c = IsoTable[ *c ] ;
}

ibm2iso( unsigned char *c )
{
    *c = TableIso[ *c ] ;
}

/*----------------------------------------------------------------------*/

/****************************************************************************/
/*             Fonctions dependantes du jeu de cars utilise                 */
/****************************************************************************/

#ifdef MSDOS

static byte EquivalTable[256] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,
0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,
0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,
0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,
0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,
0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x43,0x75,0x65,0x61,
0x61,0x61,0x61,0x63,0x65,0x65,0x65,0x69,0x69,0x69,0x41,
0x41,0x45,0x91,0x92,0x6f,0x6f,0x6f,0x75,0x75,0x79,0x4f,
0x55,0x9b,0x9c,0x9d,0x9e,0x9f,0x61,0x69,0x6f,0x75,0x6e,
0x4e,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,
0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0xc4,0xc5,
0xc6,0xc7,0xc8,0xc9,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,
0xd1,0xd2,0xd3,0xd4,0xd5,0xd6,0xd7,0xd8,0xd9,0xda,0xdb,
0xdc,0xdd,0xde,0xdf,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,
0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,0xee,0xef,0xf0,0xf1,
0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,
0xfd,0xfe,0xff};
#endif           /* MSDOS chars */


#ifdef ISO_CHARS /* Jeu de caracteres iso 8859 - 1 sous UNIX */

static byte EquivalTable[256] = {
0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,
0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,0x14,0x15,
0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,0x1e,0x1f,0x20,
0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,
0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,0x35,0x36,
0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,
0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,0x4c,
0x4d,0x4e,0x4f,0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,
0x58,0x59,0x5a,0x5b,0x5c,0x5d,0x5e,0x5f,0x60,0x61,0x62,
0x63,0x64,0x65,0x66,0x67,0x68,0x69,0x6a,0x6b,0x6c,0x6d,
0x6e,0x6f,0x70,0x71,0x72,0x73,0x74,0x75,0x76,0x77,0x78,
0x79,0x7a,0x7b,0x7c,0x7d,0x7e,0x7f,0x80,0x81,0x82,0x83,
0x84,0x85,0x86,0x87,0x88,0x89,0x8a,0x8b,0x8c,0x8d,0x8e,
0x8f,0x90,0x91,0x92,0x93,0x94,0x95,0x96,0x97,0x98,0x99,
0x9a,0x9b,0x9c,0x9d,0x9e,0x9f,0xa0,0xa1,0xa2,0xa3,0xa4,
0xa5,0xa6,0xa7,0xa8,0xa9,0xaa,0xab,0xac,0xad,0xae,0xaf,
0xb0,0xb1,0xb2,0xb3,0xb4,0xb5,0xb6,0xb7,0xb8,0xb9,0xba,
0xbb,0xbc,0xbd,0xbe,0xbf,0xc0,0xc1,0xc2,0xc3,0x41,0x41,
0xc6,0x43,0xc8,0x45,0xca,0xcb,0xcc,0xcd,0xce,0xcf,0xd0,
0x4e,0xd2,0xd3,0xd4,0xd5,0x4f,0xd7,0xd8,0xd9,0xda,0xdb,
0x55,0xdd,0xde,0xdf,0x61,0x61,0x61,0xe3,0x61,0x61,0xe6,
0x63,0x65,0x65,0x65,0x65,0x69,0x69,0x69,0x69,0xf0,0x6e,
0x6f,0x6f,0x6f,0xf5,0x6f,0xf7,0xf8,0x75,0x75,0x75,0x75,
0xfd,0xfe,0x79};

#endif /* ISO CHARS */


/*--------------------------------------------------------------------------*/
/*
 *  equival( ch ) :   Renvoie le caractere non accentue equivalent a une
 *            lettre accentuee, en Francais.  
 *
 */

byte equival( byte ch )
{
	return EquivalTable[ ch ] ;
}
/*--------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------*/
/*
 * StrAscii:     Supprime les accents Francais dans une chaine, la rendant
 *               pur 7 bits.
 *
 *               Retourne un pointeur sur la chaine modifiee.
 */
byte*
StrAscii ( byte *str)
{
  byte *p = str ;
 
    while (*str)
    {
               *str = EquivalTable[ *str++ ] ;
    }

 return p ;
}
/*--------------------------------------------------------------------------*/


#ifdef MSDOS

char ProgPath[ 127 ];
/*--------------------------------------------------------------------------*/
/*
 * MSDOS SPECIFIC:     Recherche du lexique specifique a MS-DOS en raison
 *                     de l'abondant courrier recu :-)
 *
 */
void
GetDosPath(char *path)
{
    char buf[127];
    char *dic ;

    TakePath( path );

                        /* Premiere recherche normale, dans la variable  */
                        /* d'environnement. On ne recherche pas plus si  */
                        /* elle est definie.                             */

    if( (dic = getenv("DICO")) != NULL )
    {
        strcpy(dict_path, dic );
        return ;
    }

                        /* Seconde  recherche dans un sous-repertoire du */
                        /* repertoire de lancement de dico.exe:          */

    sprintf(buf, "%s\\%s/a%s", ProgPath, DOS_PATH, DICT_EXT );
    if(! access( buf, 0 ) )
    {
        sprintf( dict_path, "%s\\%s", ProgPath, DOS_PATH );
        return ;
    }

                        /* Troisieme recherche dans le repertoire normal */
                        /* prevu sous Unix                               */

    sprintf(buf, "%s/a%s", DICT_PATH, DICT_EXT );

    if(! access( buf, 0 ) )
    {
        strcpy( dict_path, DICT_PATH );
        return ;
    }

                        /* Dernier espoir, le repertoire courant.        */

        *dict_path = '\0' ;
        *sep       = '\0' ;

}


void
TakePath(char *path)
{
        int i;

        strcpy(ProgPath,path);
        i = strlen(ProgPath);
        while(i)
        {
                if(ProgPath[i] == '\\' )
                {
                   ProgPath[i] = 0 ;
                   break ;
                }
                ProgPath[ i-- ] = 0;
        }
}

/*----------------------------------------------------------------------*/
#endif    /* MSDOS */
