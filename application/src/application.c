/* Header Files */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stats_common.h"

/* Macros */
#define DBG_PRINT(fmt ...) {\
if (enableDebug)\
{\
printf(fmt);\
}\
}\

/* Global and static variables */
static bool enableDebug = false;


/*** Functions ***/

/*
 * PrintHelp() : To print help menu.
 */
void PrintHelp()
{
    printf("Help:\n");
    printf("-v : To enable verbose.\n");
    printf("--help : To print help menu.\n");
    printf("-H \"Header-name: Header-value\" : Can be used multiple times, each time specifying an extra HTTP header to add to your request.\n");
    printf("-n <integer> : Number of HTTP requests to make (i.e. the number of samples you will have to take the median of); Default is 1.\n");
}

/*
 * main()
 */
int main(int argc, char* argv[])
{

    int i;
    int numOfReqs = 1; // Set  default value as 1.
    int retValue = 0;
    STATS *stats;

    /* Read command line arguments */
    for (i = 1; i < argc; i++)
    {
        /* Add headers to the list using SetHeader() */
        retValue = strcmp("-H",argv[i]);
        if(retValue == 0)
        {
            DBG_PRINT("Header : %s\n", argv[i+1]);
            SetHeader(argv[i+1]);
        }

        /* Set number of requests */
        retValue = strcmp("-n", argv[i]);
        if(retValue == 0)
        {
            numOfReqs = atoi(argv[i+1]);
            DBG_PRINT("Number of requests to make : %d\n", numOfReqs);
        }

        /* Enable verbose */
        retValue = strcmp("-v", argv[i]);
        if(retValue == 0)
        {
            enableDebug = true;
            SetVerbose(true);
        }

        /* Print help menu */
        retValue = strcmp("--help", argv[i]);
        if(retValue == 0)
        {
            PrintHelp();
            return 0;
        }
    }

    /* Check if number of requests is zero */
    if (numOfReqs == 0)
    {
        printf("Invalid Arguments, Cant set number of requests to 0!\n");
        PrintHelp();
        return -1;
    }

    /* Allocate memory for the STATS structure to get http statistics */
    stats = (STATS*)malloc(sizeof(STATS));
    if(stats == NULL)
    {
        printf("ERROR! Failed to allocate memory\n");
        return -1;
    }

    /* Get HTTP statistics by calling the api GetHttpStats() and check for return value */
    retValue = GetHttpStats(numOfReqs, stats);
    if(retValue != STATS_SUCCESS)
    {
        printf("ERROR! Failed to retrieve http statistics\n");
        return -1;
    }
    else
    {
        /* Print statistics */
        DBG_PRINT("HTTP Statistics :\n");
        DBG_PRINT("Response Code = %ld\n", stats->responseCode);
        DBG_PRINT("IP Address = %s\n", stats->ipAddress);
        DBG_PRINT("Namelookup Time Median = %lf\n", stats->namelookupTimeMedian);
        DBG_PRINT("Connect Time Median = %lf\n", stats->connectTimeMedian);
        DBG_PRINT("Start Time Median = %lf\n", stats->startTimeMedian);
        DBG_PRINT("Total Time Median = %lf\n", stats->totalTimeMedian);

        printf("\nSKTEST;%s;%ld;%lf;%lf;%lf;%lf\n", stats->ipAddress, stats->responseCode, stats->namelookupTimeMedian, stats->connectTimeMedian, stats->startTimeMedian, stats->totalTimeMedian);
    }

    /* Cleanup */
    free(stats);
    stats = NULL;

    return 0;
}

