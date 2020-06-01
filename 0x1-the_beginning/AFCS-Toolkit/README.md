# AFCS-Toolkit

## What is this?
-	Some *Python3 scripts* that may help in solving AFCS-related problems

## How to run it?
### Windows
-	Open **PowerShell** and type **python3 <script_name>** [<arguments>]

### Linux
1.	Like above
2.  $_ chmod +x **<script_name>**

	$_ **./<script_name>**

-	**Note1:** *<script_name>* is one of those .py files present in this folder.
-	**Note2:** If you run any of these scripts with no command-line arguments, it will show you how to use it.

## Description
#### *find_primitive_roots_mod_m.py*
-	Finds all primite roots modulo m
-	Usage: python3 ./find_primitive_roots_mod_m.py m φ(m) φ(φ(m))

#### *linear_congruential_equation_solver.py*
-	Solves equations of the form: *ax ≡ b mod m*
-	Usage: python3 ./linear_congruential_equation_solver.py a b m

#### *order_finder.py*
-	Finds the order of a modulo m, by applying the formula: **ordm(a) = min{k >= 1 | a^k ≡ 1 mod m}**
-	Usage: python3 ./order_finder.py a m

#### *quadratic_residue.py*
-	Finds the solution (if it has) for equations of the form: *x^2 ≡ a mod p*
-	Usage: python3 ./quadratic_residue.py a p

#### *simplify_mod_m.py*
-	Computes the reminder modulo m
-	e.g:  202 mod 3 is equivalent to 1 mod 3
-	Usage: python3 ./simplify_mod_m.py a m

## Requirements
-	Python3 installed, **at least 3.6**
