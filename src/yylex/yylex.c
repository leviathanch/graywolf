#include <string.h>
#include <unistd.h>
#include <yalecad/debug.h>
#include <yalecad/cleanup.h>
#include <yalecad/file.h>
#include <general.h>
#include <string.h>
#include <yalecad/string.h>

#define DEBUG_MEM_VIOLATION 1

char yysbuf[YYLMAX];
yysvf *yyestate;
YYSTYPE yylval;
yysvf *yylstate[YYLMAX];
yysvf  *yybgin;
yywork *yytop;
int yyleng;
int yymorfg;
int yylineno;
int yyfnd;
int yyprevious;

rw_table __attribute__((visibility("default"))) *rwtable;
yywork __attribute__((visibility("default"))) *yycrank;
yysvf  __attribute__((visibility("default"))) *yysvec;
unsigned long __attribute__((visibility("default"))) line_countS = 1;
char __attribute__((visibility("default"))) *yyextra;
char __attribute__((visibility("default"))) yytext[YYLMAX];

char input( FILE* yyin );
int output( int out ) ;
int screen();
int yyback(int *p, int m);
int check_line_count(char *s) ;
int unput(char c);
int yylook(FILE *yyin);

yysvf *yystate;
yysvf **lsp;
unsigned long sizeof_yybgin;
unsigned long sizeof_yycrank;
unsigned long sizeof_yysvec;
unsigned long sizeof_rwtable;

int yylook(FILE *yyin)
{
	yywork *yyt;
	yywork *yyr;
	yysvf *yyz;
	int yych;
	char *yylastch;

	/* start off machines */
	if (!yymorfg) {
		yylastch = yytext;
	} else {
		yymorfg=0;
		yylastch = yytext+yyleng;
	}
#if DEBUG_MEM_VIOLATION
	printf("yylastch: %p\n",yylastch);
	printf("yytext: %p\n",yytext);
#endif

	while(1) {
		memcpy(yystate, yybgin, sizeof_yybgin);
		yyestate = yystate;
#if DEBUG_MEM_VIOLATION
		printf("yystate: %p\n",yystate);
		printf("yyestate: %p\n",yyestate);
#endif
		lsp = yylstate;

#if DEBUG_MEM_VIOLATION
		printf("yyprevious %c\n",yyprevious);
#endif
		if (yyprevious==YYNEWLINE) {
#if DEBUG_MEM_VIOLATION
			printf("Newline\n");
			printf("yystate before: %p\n",yystate);
#endif
			yystate++;
#if DEBUG_MEM_VIOLATION
			printf("yystate after: %p\n",yystate);
			printf("yystate->yystoff after: %p\n",yystate->yystoff);
			printf("yycrank: %p\n",yycrank);
#endif
		}

		while(1) {
			yyt = yystate->yystoff;
			*yylastch++ = yych = input(yyin);
#if DEBUG_MEM_VIOLATION
			printf("yylastch: %p\n",yylastch);
			printf("yytext: %p\n",yytext);
			printf("yytext: %s\n",yytext);
			printf("yych: %c\n",yych);
#endif
			yyr = yyt;
			yyt = yyr + yych;
#if DEBUG_MEM_VIOLATION
			printf("yyt %p yytop %p yyt->verify %d yyt->verify+yysvec %p yystate %p\n",yyt,yytop,yyt->verify,yyt->verify+yysvec,yystate);
#endif
			if((int)yych==32) {
				break;
			} else {
				if(yyt->advance+yysvec == YYLERR) {	/* error transitions */
// 					unput(*--yylastch);
					break;
				}
				*(lsp++) = yystate;
				yystate = yyt->advance+yysvec;
				goto contin;
			}
contin:
			;
		}

#if DEBUG_MEM_VIOLATION
		printf("lsp %lu, yylstate %lu\n",lsp,yylstate);
#endif
		while (lsp-- > yylstate) {
			printf("Miau1\n");
			*(yylastch--) = 0;
			yyfnd = *((*lsp)->yystops);
			if (lsp != 0 && yyfnd > 0){
				if(yyextra[yyfnd]){		/* must backup */
					while(yyback((*lsp)->yystops,-yyfnd) != 1 && lsp > yylstate){
						lsp--;
// 						unput(*yylastch--);
					}
				}
				yyprevious = *yylastch;
				yyleng = yylastch-yytext+1;
				yytext[yyleng] = 0;
				printf("yytext: %s\n",yytext);
				printf("action %d\n",yyfnd);
				return(yyfnd++);
			}
			//unput(*yylastch);
		}
		if ( yytext[0] == 0) {
			printf("Miau2\n");
			return 0;
		}
		if (feof(yyin)) {
			printf("Miau3\n");
			return 0;
		}
		yyprevious = yytext[0];
// 		yytext[0] = input(yyin);
		if (yyprevious>0)
			output(yyprevious);
		yylastch = yytext;
	}
	return 0;
}

int __attribute__((visibility("default"))) yylex(FILE *yyin, FILE *yyout)
{
#if DEBUG_MEM_VIOLATION
	printf("Runing lexer\n");
#endif
	int nstr = 0;
	while((nstr = yylook(yyin)) >= 0)
		printf("nstr: %d\n",nstr);
		yyfussy:
			switch(nstr){
				case 0:
						return 0;
						break;
				case 1:
						/* C-style comments over multiple lines */
						check_line_count(yytext) ;
						break;
				case 2:
						/* convert to an integer */
						yylval.ival = atoi( yytext ) ;
						return (INTEGER); 
						break;
				case 3:
					/* convert to an integer */
					yylval.fval = atof( yytext ) ;
					return (FLOAT); 
					break;
				case 4:
						/* convert to an integer */
						yylval.fval = atof( yytext ) ;
						return (FLOAT); 
						break;
				case 5:
						printf("It's a colon!\n");
						return( COLON ) ;
						break;
				case 6:
						printf("It's a 6!\n");
						return( screen() ) ;
						break;
				case 7:
						line_countS++;
						break;
				case 8:
						break;
				case 9:
						return( token(yytext[0]) ) ;
						break;

				case -1:
						break;
				default:
						printf("bad switch yylook %d",nstr);
						break;
	}
	return 0;
}

/* end of yylex */

int yyback(int *p, int m) {
	if (p==0)
		return 0;
	while (p)
	{
		if (*p++ == m)
			return 1;
	}
	return 0;
}

int check_line_count( char *s )
{
	if( s ){
		if( strlen(s) >= YYLMAX ){
			printf("comment beginning at line %d ", line_countS );
			printf("exceeds maximum allowed length:%d chars.\n", YYLMAX );
		}
		for( ;*s;s++ ){
			if( *s == '\n'){
				line_countS++;
			}
		}
	}
} /* end check_line_count */

int screen() 
{
	INT c ;
	rw_table  *low = rwtable,        /* ptr to beginning */
			 *mid ,  
			 *high = END(rwtable) ;   /* ptr to end */

	/* binary search to look thru table to find pattern match */
	while( low <= high){
		mid = low + (high-low) / 2 ;
		if( (c = strcmp(mid->rw_name, yytext) ) == STRINGEQ){
			return( mid->rw_yylex ) ; /* return token number */
		} else if( c < 0 ){
			low = mid + 1 ;
		} else {
			high = mid - 1 ;
		}
	}
	/* at this point we haven't found a match so we have a string */
	/* save the string by making copy */
	yylval.string = (char *) Ystrclone( yytext ) ;
	return (STRING); 
} /* end screen function */

int output( int out )
{
	//printf("output: %c \n",out);
}

char input( FILE * yyin )
{
	char ret;
// 	char ret = (((yytchar=yysptr>yysbuf?U(*--yysptr):fgetc(yyin))==10?(yylineno++,yytchar):yytchar)==EOF?0:yytchar);
	ret=fgetc(yyin);
	printf("char %d\n",ret);
	return ret;
}

int unput(char c)
 {
	if(c=='\n')
		 yylineno--;
}

void __attribute__((visibility("default"))) reset_yylex(unsigned long sizeof_rwtableS, unsigned long sizeof_yycrankS, unsigned long sizeof_yysvecS) {

	yybgin = yysvec+1;
	sizeof_yysvec = sizeof_yysvecS;
	sizeof_yybgin = sizeof_yysvecS-1;
	sizeof_rwtable = sizeof_rwtableS;
	sizeof_yycrank = sizeof_yycrankS;
	printf("sizeof(rwtable) %lu,sizeof(yycrank) %lu,sizeof(yysvec) %lu\n",sizeof_rwtable,sizeof_yycrank,sizeof_yysvec);
	yytop = yycrank+sizeof_yycrank;
	yyleng = 0;
	yymorfg = 0;
	yylineno = 1;
	yyfnd = 0;
	yyprevious = 0;
	yystate = malloc(sizeof_yybgin);
	lsp = malloc(sizeof(yysvf *)*YYLMAX);
#if DEBUG_MEM_VIOLATION
	printf("Allocated sizeof_yybgin which is %lu\n",sizeof_yybgin);
#endif
}
