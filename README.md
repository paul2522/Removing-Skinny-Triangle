# Removing-Skinny-Triangle
* Removing Skinny triangle with OFF file and edge information file

## Input File
* Edited OFF file(OFF file with skinny triangle boolean information, name : V_{step size}step_emd_{emd number}.OFF)
* Edge Information txt file(name : {step size}step_emd_{emd number}_EdgePassCheck1.txt)

## Input
* Emd number

## Output
* number of face after remove
* extra error face(2 same point in three vertex set)
* list of number of skinny triangle by case

## Output File
* Voxel Case txt file(name : VoxelCase.txt)
  - index {num} : case {case num}
* Skinny triangle Information txt file(name : {emd number}_ErrorCase.txt)
  - list of number of skinny triangle by case
  - list of cases that remove skinny triangle
  - Entire Error Case(about voxel)
  - Actual erased case(about voxel)
  - Removal proprotion(about voxel)
  - Number of removed face by algorithm
  - Extra number of removed face by same position vertex
  - number of whole face, removed face, remain face
* New OFF file removed skinny triangle(name : new{emd num}.OFF}
  - final result
  - veretx num, face num, 0
  - vertex position
  - Index set of vertex that construct face(skinny triangle removed)
