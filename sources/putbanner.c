/*
+-----------------------------------------------------------------------+
|              PUTBANNER- R.Cougnenc 1992 - Domaine public              |
|                                                                       |
|   PutBanner reprend tous les fichiers [a-z].dic pointes par la        |
|   variable d'environnement "DICO" ou par defaut /usr/local/lib/dico,  |
|   et rajoute l'en-tete pour la version de distribution.               |
|                                                                       |
|   Sous DOS il passe aussi les fichiers au format texte Unix afin de   |
|   gagner un octet par ligne ( CR/LF -> LF )                           |
|                                                                       |
+-----------------------------------------------------------------------+
*/

#ifndef LINUX
#ifndef MSDOS
#error "Vous devez definir le systeme d'exploitation LINUX ou MSDOS."
#endif
#endif

#ifdef LINUX
#define UNIX
#define MYOS "LINUX"                    /* Pour affichage version only    */
#else
#define MYOS "MSDOS"
#endif


#define DICT_PATH "/usr/local/lib/dico"  /* path par defaut */
#define DOS_PATH  "lexique"              /* path par defaut Dos uniquement  */
#define DICT_EXT ".dic"                  /* extension des fichiers dicos    */
#define VERSION_FILE "version.dic"       /* fichier texte version dico      */

#define ERROR 1
#define OK    0
#define MYVBUF 10240                     /* pour setvbuf() eventuel         */
#define COMMENT_CHAR  35                 /* ligne de commentaire: '#'       */

#ifndef LINUX
#define MUST_HAVE_BUF                  /* Linux va moins vite avec setvbuf! */
#endif

#ifdef UNIX 
#define TMPPATH "/tmp"
#define READ_BINARY  "r"
#define WRITE_BINARY "w"
#define READ_TEXT    "r"
#else
#define TMPPATH ""
#define READ_BINARY  "rb"
#define WRITE_BINARY "wb"
#define READ_TEXT    "rt"
#endif

typedef unsigned char byte;

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
            char *__Version__ ="1.0" ;
/*                                                                        */
/*------------------------------------------------------------------------*/

char dict_path[127] ;




main( int argc, char **argv )
{
 char *dic;

#ifdef MSDOS
    GetDosPath( argv[0] );
#else
    if( (dic = getenv("DICO")) == NULL )       /* ou est le lexique ? */
        strcpy(dict_path, DICT_PATH ) ;        /* default path        */
    else
        strcpy(dict_path, dic );               /* user env path       */
#endif


    mybanner();                               /* on dit bonjour...     */

    if( do_not() )
    {
        printf("\nAucune modification faite.\n");
        return ERROR ;
    }

    return cleandic();
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/* 
 * do_not: Ecran d'acceuil, demande de confirmation.
 *
 */
do_not()
{
    char rep[80];

    printf("PUTBANNER va modifier tous les fichiers [a-z].dic presents\n");
    printf("dans le repertoire %s.\n\n",dict_path);

    printf("Il va rajouter dans chaque fichier l'entete destine a realiser\n");
    printf("la version de distribution.\n\n");
    printf("Tout autre commentaire sera supprime, et sous DOS les fichiers\n");
    printf("seront passes au format texte Unix.(LF au lieu de CR-LF).\n\n");

    while( 1 )
    {
        printf("\tVoulez-vous continuer (O/N) ? :");
        fgets(rep,78, stdin );
        switch ( rep[0]) 
        {
            case 'n':
            case 'N':
                return  ERROR ;
 
            case 'o':
            case 'O':
            case 'y':
            case 'Y':
            case 'j':
            case 'J':
                return OK ;
        }
    }
}
/*--------------------------------------------------------------------------*/
 

putheader( int letter , FILE *fp)
{
int i ;
fprintf(fp,"#");
for( i = 0 ; i < 72 ; i++ )
	fprintf(fp,"%c", letter -32 );
fprintf(fp, "\n");

fprintf(fp,"#                           +----------+\n");
fprintf(fp,"#                           |  Le Dico |\n");
fprintf(fp,"#                           +----------+\n#\n");
fprintf(fp,"#                           La Lettre  %c\n#\n", letter - 32 );
fprintf(fp,"#\n");
fprintf(fp,"#  Ce lexique a ete realise par un collectif d'utilisateurs de BBS et\n");
fprintf(fp,"#  est mis dans le DOMAINE PUBLIC. La premiere mouture a ete finalisee\n");
fprintf(fp,"#  en Octobre 1992 par Rene Cougnenc, distribuee avec son programme d'\n");
fprintf(fp,"#  exploitation \"DICO\", compilable sous Un*x ou Ms-Dos entre autres.\n");
fprintf(fp,"#\n");
fprintf(fp,"#  Vous pouvez supprimer cet en-tete pour votre utilisation personnelle\n");
fprintf(fp,"#  sur votre machine afin de gagner place et vitesse.\n");
fprintf(fp,"#\n");
fprintf(fp,"#  En Aucun cas vous ne devez distribuer ce lexique sans les en-tetes,\n");
fprintf(fp,"#  ou sans fournir l'integralite des fichiers *.dic, de A a Z, ainsi\n");
fprintf(fp,"#  que le fichier version.dic indiquant la mise a jour et le type de\n");
fprintf(fp,"#  format texte accentue qui a ete utilise.  Merci de laisser egalement\n");
fprintf(fp,"#  la documentation contenant le nom des createurs, pour la posterite :-)\n");
fprintf(fp,"#\n#\n#");
for( i = 0 ; i < 72 ; i++ )
	fprintf(fp,"%c", letter -32 );
fprintf(fp, "\n");
}



putfooter( int letter , FILE *fp)
{
  int i ;

  fprintf(fp,"#\n");
  fprintf(fp,"#             Le Dico - Fin de la lettre %c -\n#", letter - 32 );
  for( i = 0 ; i < 72 ; i++ )
	fprintf(fp,"%c", letter -32 );
  fprintf(fp, "\n");
}
/*--------------------------------------------------------------------------*/
/*
 *  cleandic(): Passe en revue toutes les lignes du dictionnaire, et
 *              elimine les lignes de commentaires ainsi que les retour-                  
 *              charriot eventuels provenant de DOS, pour gagner de la
 *              place entre autres sous ce systeme.
 */
 cleandic()
 {
    char fname[127];
    char tmpfile[127];
    char buf[90];
    char ch, letter ;
    FILE *fp,*ftmp ;


    sprintf(tmpfile,"%s/cleantmp.%d", TMPPATH, getpid() );

    for( letter = 'a' ; letter <= 'z' ; letter++ )
    {
        sprintf(fname, "%s%s%c%s",dict_path, sep, letter, DICT_EXT );
        if(( fp = fopen(fname,READ_TEXT)) == NULL )
        {
            fprintf(stderr,"Impossible de lire fichier %s\n",fname );
            file_usage();
            return ERROR;
        }

#ifdef MUST_HAVE_BUF
        setvbuf( fp, NULL, _IOFBF, MYVBUF );
#endif

        if(( ftmp = fopen(tmpfile,WRITE_BINARY)) == NULL ) /* BINARY for DOS!*/
        {
            fprintf(stderr,"Fichier temporaire %s impossible !\n",fname );
            return ERROR;
        }

#ifdef MUST_HAVE_BUF
        setvbuf( ftmp, NULL, _IOFBF, MYVBUF );
#endif

        printf("Traite la lettre %c...\n", letter );
	putheader( letter, ftmp);
        while( fgets(buf,88,fp) )
        {
            nocrlf( buf );
            if((byte) *buf == COMMENT_CHAR || *buf == '\0'  ) 
                   continue;
            fprintf(ftmp,"%s\n",buf);
        }
	putfooter( letter, ftmp);
        fclose( fp );
        fclose( ftmp);
                              /* ensuite, on recopie. C'est tres lent mais */
                              /* ca marche a coup sur sous DOS avec des    */
                              /* drives differents, ce serait mieux avec   */
                              /* un link ou un rename... Pas me casser :-) */

        if(( ftmp = fopen(tmpfile,READ_BINARY)) == NULL ) /* BINARY for DOS!*/
        {
            fprintf(stderr,"Fichier temporaire %s inacessible !\n",fname );
            return ERROR;
        }

#ifdef MUST_HAVE_BUF
        setvbuf( ftmp, NULL, _IOFBF, MYVBUF );
#endif
 

        if(( fp = fopen(fname,WRITE_BINARY)) == NULL )
        {
            fprintf(stderr,"Impossible de reecrire fichier %s\n",fname );
            return ERROR;
        }

#ifdef MUST_HAVE_BUF
        setvbuf( fp, NULL, _IOFBF, MYVBUF );
#endif

        while( fread( &ch, 1, 1, ftmp ) ) 
               fwrite( &ch, 1,1, fp ) ; 
 
        fclose( fp );
        fclose( ftmp);
    }

    unlink( tmpfile );
    printf("Hop, j'ai fini.\n");
    return OK ;
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
    printf("\nPUTBANNER V %s (%s) - R.Cougnenc 1992\n\n", __Version__, MYOS);
}
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*
 * nocrlf: Supprime un line feed et/ou un retour charriot
 *         a la fin d'une chaine.
 *
 */
nocrlf( char *str) 
{
    while( *str )
    {
      if((unsigned char) *str == '\n' || (unsigned char) *str == '\r')
        *str = '\0' ;
      str++;
    }
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
    fprintf(stderr,"      ou se trouve le programme 'putbanner.exe',\n\n");

    fprintf(stderr,"    - Dans le repertoire /usr/local/lib/dico par\n");
    fprintf(stderr,"      compatibilite avec la version normale UN*X,\n\n");

    fprintf(stderr,"    - Dans le repertoire courant.\n\n");
#endif
}
/*--------------------------------------------------------------------------*/


#ifdef MSDOS
getpid()
{
	return 1 ;	/* :-)   */
}

#endif
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
