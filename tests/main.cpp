#include <iostream>
#include <omp.h>
#include <string>
#include <vector>
#include "parallel_matcher.h"

void Test_GetWitnessArray() {
   ParallelMatcher pm;
   pm.Test_GetWitnessArray();
}

void Test_Duel(){
   ParallelMatcher pm;
   pm.Test_Duel(1,2);
   pm.Test_Duel(3,4);
   pm.Test_Duel(5,6);
   pm.Test_Duel(7,8);
   pm.Test_Duel(9,10);
   pm.Test_Duel(11,12);
   pm.Test_Duel(13,14);
   pm.Test_Duel(15,16);
   pm.Test_Duel(1,4);
   pm.Test_Duel(6,8);
   pm.Test_Duel(9,11);
   pm.Test_Duel(14,16);

}

void Test_MatchNonPeriodic(){
   ParallelMatcher pm;
   pm.Test_MatchNonPeriodic();
}

int main(int argc, char *argv[])
{
   // Test_GetWitnessArray();
   // Test_Duel();
   Test_MatchNonPeriodic();
   return 0;
}