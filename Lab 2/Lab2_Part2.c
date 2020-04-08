/*
Name: Ethan Paek
Date: 4/7/20
Title: Lab #2 Part 2 – Computing connection times
Description: Compute various times listed in COEN 146 Lab 2 Step #5
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define RTT0 3
#define RTT1 20
#define RTT2 26
#define RTTHTTP 47
#define objects 6

int main(){

	// part a
	int client_receive = RTT0 + RTT1 + RTT2 + (2 * RTTHTTP);
	printf("Time elapsed from when client clicks on link until client receives object: %d msecs\n", client_receive);

	// part b
	int nonpers_nopar = client_receive + (objects * 2 * RTTHTTP);	
	printf("Time elapsed while referencing 6 very small objects, assuming non-persistent HTTP and no parallel TCP connections: %d msecs\n", nonpers_nopar);

	// part c
	int n = 5; // parallel connections
	// non-persistent case:
	int non_pers = client_receive + (2 * ceil(objects/n) * RTTHTTP);
	printf("Time elapsed for parallel TCP connections and non-persistent case: %d msecs\n", non_pers);

	// persistent case:
	int pers = client_receive + (ceil(objects/n) * RTTHTTP);
	printf("Time elapsed for parallel TCP connections and persistent case: %d msecs\n", pers);

	return 0;
}