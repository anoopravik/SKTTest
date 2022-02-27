/* Header files */
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <curl/curl.h>
#include <string.h>

#include "stats_common.h"


/* Macros */
#define TEST_URL "http://www.google.com/"

/* Global and static variables */
static struct curl_slist *headerHandle = NULL;
static bool enableDebug = false;


/*** HELPER FUNCTIONS ***/

/*
 * FindMedian() : To find median of elements in an array.
 *              Arguments : a -> array of type double whose median has to be found.
 *                          numOfElements(int) -> count of elements in the array.
 *              Return : returns median value(double).
 */
double FindMedian (double* a, int numOfElements)
{
    int i,j;
    double temp;
    double median = 0.0;

    /* Sort values of the array */
    for(i = 0; i < numOfElements; i++)
    {
        for(j = 0; j < numOfElements-1; j++)
        {
            if(a[j] > a[j+1])
            {
                temp = a[j];
                a[j] = a[j+1];
                a[j+1] = temp;
            }
        }
    }

    /* if number of elements are even */
    if(numOfElements % 2 == 0)
    {
        median = ( a[(numOfElements-1)/2] + a[numOfElements/2] ) / 2.0;
    }
    /* if number of elements are odd */
    else
    {
        median = a[numOfElements / 2];
    }

    return median;
}


/*** FUNCTIONS  ***/

/*
 * SetHeader() : To add header values to  curl_slist
 *             Arguments : header -> header values (string)
 *             Return : Nil
 */
void SetHeader(char* header)
{
    headerHandle = curl_slist_append(headerHandle, header);
}


/*
 * SetVerbose() : To enable verbose
 *             Arguments : verboseSetting -> true/false (boolean)
 *             Return : Nil
 */
void SetVerbose(bool verboseSetting)
{
    enableDebug = verboseSetting;
}


/*
 * GetHttpStats() : To get http statistic values
 *             Arguments : numOfReqs -> Number of requests to be made (int)
 *                         stats -> Pointer to the structure STATS where the data has to be filled.
 *                                  Caller is reponsible to allocate memory.
 *             Return : returns success/failure (int)
 */
int GetHttpStats(int numOfReqs, STATS* stats)
{
    CURL *curl;
    CURLcode retValue;
    int i;

    char* ipAddress;
    long responseCode;
    double* namelookupTime;
    double* connectTime;
    double* startTime;
    double* totalTime;

    /* Allocate memory for the arrays to store time data on each request */
    namelookupTime = (double*) malloc(numOfReqs * sizeof(double));
    connectTime = (double*) malloc(numOfReqs * sizeof(double));
    startTime = (double*) malloc(numOfReqs * sizeof(double));
    totalTime = (double*) malloc(numOfReqs * sizeof(double));

    memset(namelookupTime, 0.0, (numOfReqs * sizeof(double)));
    memset(connectTime, 0.0, (numOfReqs * sizeof(double)));
    memset(startTime, 0.0, (numOfReqs * sizeof(double)));
    memset(totalTime, 0.0, (numOfReqs * sizeof(double)));

    /* Init curl */
    curl = curl_easy_init();
    if(curl)
    {
        /* Set curl options */
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerHandle);
        curl_easy_setopt(curl, CURLOPT_URL, TEST_URL);
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
        if(enableDebug)
        {
            curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        }

        /* Execute curl perform number of times specifed in numOfReqs; check for return code */
        for(i = 0; i < numOfReqs; i++)
        {
            /* Perform the request */
            retValue = curl_easy_perform(curl);
            if(retValue != CURLE_OK)
            {
                printf("ERROR! Curl Perform Failed : %s\n", curl_easy_strerror(retValue));
                return STATS_FAILURE;
            }
            /* If curl is success, get info like responseCode, ipAddress, namelookupTime, 
               connectTime, startTime, totalTime */
            else
            {
                retValue = curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &responseCode);
                if(retValue != CURLE_OK)
                {
                    printf("ERROR! Failed to get response code!\n");
                    return STATS_FAILURE;
                }
            
                retValue = curl_easy_getinfo(curl, CURLINFO_PRIMARY_IP, &ipAddress);
                if(retValue != CURLE_OK)
                {
                    printf("ERROR! Failed to get ip address!\n");
                    return STATS_FAILURE;
                }

                retValue = curl_easy_getinfo(curl, CURLINFO_NAMELOOKUP_TIME, &(namelookupTime[i]));
                if(retValue != CURLE_OK)
                {
                    printf("ERROR! Failed to get namelookup time info!\n");
                    return STATS_FAILURE;
                }

                retValue = curl_easy_getinfo(curl, CURLINFO_CONNECT_TIME, &(connectTime[i]));
                if(retValue != CURLE_OK)
                {
                    printf("ERROR! Failed to get connect time info!\n");
                    return STATS_FAILURE;
                }

                retValue = curl_easy_getinfo(curl, CURLINFO_STARTTRANSFER_TIME, &(startTime[i]));
                if(retValue != CURLE_OK)
                {
                    printf("ERROR! Failed to get start transfer time info!\n");
                    return STATS_FAILURE;
                }

                retValue = curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &(totalTime[i]));
                if(retValue != CURLE_OK)
                {
                    printf("ERROR! Failed to get total time info!\n");
                    return STATS_FAILURE;
                }
            }
        }

        /* Cleanup curl handles */
        curl_easy_cleanup(curl);
        curl_slist_free_all(headerHandle);
    }
    else
    {
        printf("ERROR! Curl init Failed!\n");
        return STATS_FAILURE;
    }

    /* Fill the structure with values */
    strcpy(stats->ipAddress, ipAddress);
    stats->responseCode = responseCode;
    stats->namelookupTimeMedian = FindMedian(namelookupTime, numOfReqs);
    stats->connectTimeMedian = FindMedian(connectTime, numOfReqs);
    stats->startTimeMedian = FindMedian(startTime, numOfReqs);
    stats->totalTimeMedian = FindMedian(totalTime, numOfReqs);

    /* Free allocated memory blocks */
    free(namelookupTime); namelookupTime = NULL;
    free(connectTime); connectTime = NULL;
    free(startTime); startTime = NULL;
    free(totalTime); totalTime = NULL;

    return STATS_SUCCESS;

}
