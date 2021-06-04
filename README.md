# MPI-oddEven-sorting
MPI program that implements a shell-sort like parallel algorithm that sorts an array of integers. The initial array is partitioned into equal size sub-arrays which are distributed to the processes (one per process).

It consists of two phases:

(i) the processes that are far away from each other compare and split their assigned sub-arrays (using a hypercube pattern of communication);

(ii) perform odd-even transposition iterations as long as the sub-arrays are changing.

## Phase I: Hypercube Compare-Exchange

for i = (d − 1) to 0 do

  partner = my rank XOR 2i ;
  
  if (i-th bit of my rank) = 1 then
  
   compare-split-hi(partner);
    
  else
  
   compare-split-low(partner);
    
## Phase II: Odd-even Transposition Iterations

done = FALSE

while done = FALSE do

  {Perform odd-even iterations}
  
  if received items need to be passed further then
  
   broadcast FALSE to all processes;
   
  else
  
   broadcast TRUE to all processes;
    
  if all processes broadcast TRUE then
  
   done = TRUE
    
    
d - the number of bits required to represent the ID’s of the processes (d=3 for 8 processes).

compare-split-hi(i) - performs a compare-and-split operation so that processor i keeps half of the merged sub-arrays containing the greatest integers.

compare-split-low(i) - performs a compare-and-split operation so that processor i keeps half of the merged sub-arrays containing the smallest integers.
