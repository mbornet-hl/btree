
ALL		: CI CG EA

CI		:
		cd ci; make -f ci_makefile

CG		:
		cd cg; make -f cg_makefile

EA		:
		cd ea; make -f ea_makefile

clean	:
		rm -f */lib ea/ea_latex_tree
