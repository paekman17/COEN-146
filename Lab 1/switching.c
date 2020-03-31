// Name: Ethan Paek
// Date: 3/31/2020
// Title: Lab1 - Circuit Switching and Packet Switching
// Description: Created to complete Steps 6-7 for Lab 1. Designed to implement quantitative comparisons between circuit switching and packet switching.

#include <math.h> //pow()
#include <stdio.h> //printf, stderr
#include <stdlib.h> //atoi

//used to calculate the factorial of an integer (ex: 3! = factorial(3) = 6)
int factorial(int num){
  int result = 1;
  for(int i = 1; i <= num; i++){
    result = (result * i);
  }
  return result;
}

//binomial distribution
double binomial(int x, int y){
  double result = (factorial(x)/(factorial(y)*factorial(x-y)));
  return result;
}

int main(){
  /*
  Variable Descriptions:
  int linkBandwidth; //bandwidth of a network link
  int userBandwidth; //bandwidth required for a given user
  int nCSusers; //number of circuit switching users
  int nPSusers; //number of packet switching users
  double tPSuser; //percentage of time a packet switching user needs to transmit
  double pPSusersBusy; //probability that a given (specific) packet switching user is busy transmitting
  double pPSusersNotBusy; //probability that one (specific) packet switching user is not busy transmitting
  */

  int linkBandwidth = 200;
  int userBandwidth = 20;
  double tPSuser = 0.10;
  int nPSusers = 19;

  //Step 6
  int nCSusers = linkBandwidth/userBandwidth;
  printf("6a - nCSusers: %d \n", nCSusers);

  //Step 7a
  double pPSusers = tPSuser;
  printf("7a - pPSusers: %lf \n", pPSusers);

  //Step 7b
  double pPSusersNotBusy = 1 - pPSusers;
  printf("7b - pPSusersNoyBusy: %lf \n", pPSusersNotBusy);

  //Step 7c
  double all = pow((1 - pPSusers),(nPSusers - 1));
  printf("7c - Probability of all other users: %e \n", all);

  //Step 7d
  double notTransmitting = pPSusers * pow(pPSusersNotBusy,(nPSusers-1));
  printf("7d - Probability that one specific user is transmitting and remaining users are not: %e \n", notTransmitting);

  //Step 7e
  double onlyOne = nPSusers * notTransmitting;
  printf("7e - Probability that exactly one user is busy: %e \n", onlyOne);

  //Step 7f
  double onlyTen = pow(pPSusers,10) * pow(pPSusersNotBusy, (nPSusers-10));
  printf("7f - Probability that 10 specific users are transmitting: %e \n", onlyTen);

  //Step 7g
  double coefficient = binomial(nPSusers, 10);
  double anyTen = coefficient * pow(pPSusers,10) * pow(pPSusersNotBusy, (nPSusers-10));
  printf("7g - Probability that any 10 users are transmitting: %e \n", anyTen);

  //Step 7f
  double sum = 0;
  for(int i = 11; i <= nPSusers; i++){
    sum += binomial(nPSusers, i) * pow(pPSusers, i) * pow(pPSusersNotBusy,(nPSusers-i));
  }
  printf("7h - Probability that more than 10 users are transmitting: %e \n", sum);
  
  return 0;
}
