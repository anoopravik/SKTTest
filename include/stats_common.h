#ifndef STATS_COMMON_H
#define STATS_COMMON_H

/* Macros */
#define STATS_SUCCESS 1
#define STATS_FAILURE 0

/* Structure to hold http statistics values. Caller is reponsible for
   allocating and freeing memory block */
typedef struct HttpStats
{
    long responseCode;
    char ipAddress[100];
    double namelookupTimeMedian;
    double connectTimeMedian;
    double startTimeMedian;
    double totalTimeMedian;
}STATS;

/******* API's exposed to the application *********/

/* GetHttpStats(): to get statistics for http request. Fuction will take two arguments.
 *                 numOfReqs => Number of requsts made.
 *                 stats => pointer to the structure HttpStats to hold values. Caller should manage memory for the structure 
 *                 Returns STATS_SUCCESS/STATS_FAILURE
 */
int GetHttpStats (int numOfReqs, STATS* stats);

/*
 * SetHeader() : Used to set header values for http request.
 *               header : Takes header string as argument.
 *               returns nil.
 */
void SetHeader (char* header);

/*
 * SetVerbose() : Add more verbose for http request.
 *               verboseSetting : Takes verbose setting(true/false) as argument.
 *               returns nil.
 */
void SetVerbose (bool verboseSetting);

#endif
