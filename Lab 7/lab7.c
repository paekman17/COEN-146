/*
 * Name: Ethan Paek
 * Date: May 19th, 2020
 * Title: COEN 146 Lab #7 - Link State routing
 * Description: This program computes and demonstrates link state (LS) routing algorithm
 */

#include <unistd.h>
        // Import the required libraries
        //******STUDENT WORK******
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>

//defines
#define	N			6
#define	INFINITE	1000
#define MIN(a,b)	(((a) < (b)) ? (a) : (b))

// types
typedef struct machines {
	//******STUDENT WORK******	
    char name[50];
    char ip[50];
    int port;
} MACHINES;

// global variables
MACHINES machines[N];
int costs[N][N];
int distances[N];
int	my_id, nodes;
int	sock;
struct sockaddr_in addr;
struct sockaddr_in otheraddr;
socklen_t addr_size;
pthread_mutex_t	lock;

// functions
void *run_link_state (void *);
void *receive_info (void *);
void print_costs (void);


/**********************
 * main
 **********************/
int main (int argc, char *argv[]) {
    //argv[1] = router ID, argv[2] = N nodes, argv[3] = cost table file, argv[4] = machine table file
	FILE *fp;
	int	i, j;
	pthread_t thr1, thr2;
	int	id, cost;
	//int	packet[3];

	if (argc < 4){
		// check the number of arguments
        //******STUDENT WORK******
        printf("Need 4 arguments: router_ID, nodes, costs_file, machine_file\n");
        return 0;
	}

    my_id = atoi(argv[1]);
    nodes = atoi(argv[2]);

	if (my_id >= N){
		printf ("wrong id\n");
		return 1;
	}

	if (nodes != N){
		printf ("wrong number of nodes\n");
		return 1;
	}

	//get info on machines from the file using fopen and fscanf and populate the array of machines-struct
	//******STUDENT WORK******
	char *costs_file = argv[3];
	char *machines_file = argv[4];

    //get costs
    if ((fp = fopen (costs_file, "r")) == NULL) {
        printf ("can't open %s\n", costs_file);
        exit(1);
    }

    for (i = 0; i < nodes; i++) {
        for (j = 0; j < nodes; j++) {
            fscanf (fp, "%d", &costs[i][j]);
            printf("cost [%d][%d] = %d\n", i, j, costs[i][j]);
        }
    }
    fclose (fp);

    // get machine info
    if ((fp = fopen (machines_file, "r")) == NULL) {
        printf ("can't open %s\n", machines_file);
        exit(1);
    }

    for (i = 0; i < nodes; i++) {
        fscanf (fp, "%s%s%d", machines[i].name, machines[i].ip, &machines[i].port);
        printf("%s %s %d\n", machines[i].name, machines[i].ip, machines[i].port);
    }
    fclose (fp);
    print_costs();

    // init address
    //******STUDENT WORK******
    addr.sin_family = AF_INET;
    addr.sin_port = htons ((short)machines[my_id].port);
    addr.sin_addr.s_addr = htonl (INADDR_ANY);
    memset((char *)addr.sin_zero, '\0', sizeof (addr.sin_zero));
    addr_size = sizeof (addr);

    // create socket
    //******STUDENT WORK******
    if ((sock = socket (AF_INET, SOCK_DGRAM, 0)) < 0) {
        printf ("socket error\n");
        return 1;
    }

    // bind
    //******STUDENT WORK******
    if (bind(sock, (struct sockaddr *)&addr, sizeof (addr)) != 0) {
        printf ("bind error\n");
        return 1;
    }

	// create threads and initiate mutex
	pthread_mutex_init (&lock, NULL);
	pthread_create (&thr1, NULL, receive_info, NULL);
	pthread_create (&thr2, NULL, run_link_state, NULL);

	// read changes from the keyboard, update the costs matrix inform other nodes about the changes
	// This section can be inside the main or can be a thread by itself
    for (i = 0; i < 3; i++){
        //******STUDENT WORK******
        //printf ("sent\n");
        printf ("Input new cost from this router (%d): <neighbor id> <cost>\n", my_id);
        scanf ("%d%d", &id, &cost);
        if (id >= nodes  ||  id == my_id) {
            printf ("wrong id\n");
            break;
        }

        pthread_mutex_lock (&lock);
        costs[my_id][id] = cost;
        costs[id][my_id] = cost;
        pthread_mutex_unlock (&lock);

        print_costs();

        int	pkt[3];

        pkt[0] = htonl(my_id);
        pkt[1] = htonl(id);
        pkt[2] = htonl(cost);


        otheraddr.sin_family = AF_INET;
        addr_size = sizeof (otheraddr);

        for (int j = 0; j < nodes; j++) {
            if (j != my_id) {
                otheraddr.sin_port = htons ((short)machines[j].port);
                inet_pton (AF_INET, machines[j].ip, &otheraddr.sin_addr.s_addr);
                sendto (sock, pkt, sizeof (pkt), 0, (struct sockaddr *)&otheraddr, addr_size);
            }
        }
        printf ("Sent packet: %d %d %d\n", my_id, id, cost);
    }
	sleep (5);
}


/**********************
 * receive info
 **********************/
//If there is a cost update on other machine, they will let us know through a UDP packet. We need to receive that packet and update our costs table
void *receive_info (void *arg){
	int	new[3];
	int	nbytes;
	//int	n0, n1, n2;
	int n0, n1;
	while(1){
	    recvfrom(sock, &new, sizeof(new), 0, NULL, NULL); // receive packet

	    // parse data from packet
	    // ntohl() function converts the unsigned integer netlong from network byte order to host byte order
	    n0 = ntohl(new[0]);
	    n1 = ntohl(new[1]);
	    nbytes = ntohl(new[2]);

        printf("Received packet: %d %d %d\n", n0, n1, nbytes);

        pthread_mutex_lock(&lock); // lock before accessing costs table

        // update costs
        costs[n0][n1] = nbytes;
        costs[n1][n0] = nbytes;

        pthread_mutex_unlock(&lock); // unlock costs table

        // print out new costs
        print_costs();
	}
}


/**********************
 * run_link_state
 **********************/
// This thread runs the Dijkstra's algorithm on the cost table and calculates the shortest path
void *run_link_state (void *arg){
	int	taken[N];
	int	min, spot;
	int	i, j, k;
	int	r;

	//******STUDENT WORK******
    while (1) {
        r = rand() % 10;
        sleep(r);
        printf("Running link state.\n");
        for (i = 0; i < nodes; i++) {
            taken[i] = 0;
            pthread_mutex_lock(&lock);
            distances[i] = costs[my_id][i];
            pthread_mutex_unlock(&lock);
        }
        taken[my_id] = 1;

        for (i = 0; i < nodes; i++) {
            // find closest node
            min = INFINITE;
            for (j = 1; j < nodes; j++) {
                if (taken[j] == 0 && distances[j] < min) {
                    min = distances[j];
                    spot = j;
                }
            }
            taken[spot] = 1;

            // recalculate distances
            for (j = 0; j < nodes; j++) {
                if (taken[j] == 0) {
                    pthread_mutex_lock(&lock);
                    distances[j] = MIN(distances[j], (distances[spot] + costs[spot][j]));
                    pthread_mutex_unlock(&lock);
                }
            }
        }

        printf("new-shortest distances:\n");
        for (i = 0; i < N; i++)
            printf("%d ", distances[i]);
        printf("\n");
    }
}


/**********************
 * print costs
 **********************/
void print_costs (void) {
	int i, j;
    puts("Costs table:");
	for (i = 0; i < N; i++){
		for (j = 0; j < N; j++){
			pthread_mutex_lock (&lock);
			printf ("%d ", costs[i][j]);
			pthread_mutex_unlock (&lock);
		}
		printf ("\n");
	}
}
