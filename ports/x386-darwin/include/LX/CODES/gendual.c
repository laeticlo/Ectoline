#include <stdio.h>


                                /* -------------------- */
                                /* char encodings       */
                                /* for Sequences        */
                                /* -------------------- */

                                /* default              */

static long sDftCode[]  =  {

#include "dft_code.h"

};

#define GENDUAL

                                /* IUPAC Proteins       */
static long sSeqPCode[]  =  {

#include "prot_code.h"

};
                                /* IUPAC Dna/Rna        */
static long sSeqDCode[]  =  {

#include "dna_code.h"

};

static int Lalpha = sizeof(sDftCode)/sizeof(long);


/* --------- */
static long DualizeSymbol(pval, code)
        long pval, *code;
{
        int  i;
        long dval;

        dval = pval;

        for (i = 0 ; i < Lalpha ; i++)
            if (pval & code[i])
                dval |= sDftCode[i];
}

/* --------- */
static void PrintSymbols(pval)
        long pval;
{
        int  i;

        for (i = 0 ; i < Lalpha ; i++)
            if (pval & sDftCode[i])
                printf("%c", 'A'+i);
}

/* --------- */

static void PrintAsciiCode(code)
        long *code;
{

        int  i;

        printf("initial code\n");

        for (i = 0 ; i < Lalpha ; i++) {
            printf("%c=", 'A'+i);
            if (code[i])
                PrintSymbols(code[i]);
            else
                printf(" * not recognized *");
            printf("\n");
        }


        printf("dual code\n");

        for (i = 0 ; i < Lalpha ; i++) {
            printf("%c=", 'A'+i);
            if (code[i])
                PrintSymbols(DualizeSymbol(code[i] | sDftCode[i], code));
            else
                printf(" * not recognized *");
            printf("\n");
        }
}

/* --------- */

static void PrintTableCode(code)
        long *code;
{

        int  i;

        printf("\t");
        for (i = 0 ; i < Lalpha ; i++) {

            printf("0x%8.8x",
                        code[i] 
                        ? DualizeSymbol(code[i] | sDftCode[i], code)
                        : 0);

            printf(" /* %c */, ", 'A' + i);
            if ((i%3) == 2) printf("\n\t");
        }
}

/* --------- */

main()
{

        printf("Dna\n");
        PrintAsciiCode(sSeqDCode);
        PrintTableCode(sSeqDCode);

        printf("\n\nProtein\n");
        PrintAsciiCode(sSeqPCode);
        PrintTableCode(sSeqPCode);



}
