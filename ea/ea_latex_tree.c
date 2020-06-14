/*
 * Copyright (C) 2019, Martial Bornet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 *   @(#)  [MB] ea_latex_tree.c Version 1.9 du 20/06/14 - 
 */


#include  <stdio.h>
#include  <unistd.h>
#include  <stdlib.h>
#include  <string.h>

#include  "../cc/cc_types.h"
#include  "../cd/cd_cpub.h"
#include  "../ci/ci_cpub.h"
#include  "../ci/ci_epub.h"

/* Description of an integer element
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
struct ea_int {
     ci_node                        node;
     cc_uint32                      i;
};
typedef struct ea_int              *ea_ref_int;

/* LaTeX header
   ~~~~~~~~~~~~ */
char *ea_latex_header[] = {
     "\\documentclass{article}",
     "\\usepackage{tikz-qtree}",
     "\\begin{document}",
     "\\tikzset{every tree node/.style={minimum width=2em,draw,circle},",
     "        blank/.style={draw=none},",
     "        edge from parent/.style=",
     "        {draw,edge from parent path={(\\tikzparentnode) -- (\\tikzchildnode)}},",
     "        level distance=1.5cm}",
     "\\begin{tikzpicture}",
     "\\Tree",
     0
};

/* LaTeX footer
   ~~~~~~~~~~~~ */
char *ea_latex_footer[] = {
     "\\end{tikzpicture}",
     "\\end{document}",
     0
};

/*******************************************************************************

                              EA_USAGE

*******************************************************************************/
void ea_usage(char *progname)
{
     fprintf(stderr, "Usage: %s max\n", progname);
}

/******************************************************************************

                              EA_NEW_INT

******************************************************************************/
ea_ref_int ea_new_int(int value)
{
     ea_ref_int                     _elt;

     /* Allocate memory for the data descriptor
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
     if ((_elt = (ea_ref_int) malloc(sizeof *_elt)) == 0) {
          fprintf(stderr, "%s(): cannot allocate memory !\n", __func__);
          exit(1);
     }

     /* Initialize the node descriptor
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
     ci_init_node(&_elt->node);
     _elt->node.data               = _elt;

     /* Copy data into the node
        ~~~~~~~~~~~~~~~~~~~~~~~ */
     _elt->i                       = value;

     return _elt;
}

/******************************************************************************

                              EA_CMP_INT

******************************************************************************/
cc_uint16 ea_cmp_int(ci_node *n1, ci_node *n2)
{
     ea_ref_int                _i1, _i2;
     cc_uint16                 _retcode;

     /* Get the data of the nodes
        ~~~~~~~~~~~~~~~~~~~~~~~~~ */
     _i1                      = n1->data;
     _i2                      = n2->data;

     /* Compare the data of the two nodes
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
     if (_i2->i < _i1->i) {
          _retcode                 = CI_CMP_LT;
     }
     else if (_i2->i > _i1->i) {
          _retcode                 = CI_CMP_GT;
     }
     else {
          _retcode                 = CI_CMP_EQ;
     }

     return _retcode;
}

/******************************************************************************

                              EA_DISP_NODE_INFO

******************************************************************************/
void ea_disp_node_info(ci_node *node, cc_uint16 not_used)
{
     /* Display node info as a line
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
#define   EA_SZ                    (16)
     ea_ref_int                     _elt, _elt_left, _elt_right, _elt_up;
     cc_uint16                      _sz = 5;
     char                           _value[EA_SZ], _left[EA_SZ], _right[EA_SZ],
                                    _up[EA_SZ], *_where;
     cc_uint8                       _H_left, _H_right;

     _elt                          = node->data;
     
     /* Initialize node value
        ~~~~~~~~~~~~~~~~~~~~~ */
     sprintf(_value, "[%*d]", _sz, _elt->i);

     /* Initialize left value
        ~~~~~~~~~~~~~~~~~~~~~ */
     if (node->left) {
          _elt_left                     = node->left->data;
          sprintf(_left, "[%*d]", _sz, _elt_left->i);
     }
     else {
          sprintf(_left, " %*s ", _sz, "");
     }

     /* Initialize right value
        ~~~~~~~~~~~~~~~~~~~~~~ */
     if (node->right) {
          _elt_right                    = node->right->data;
          sprintf(_right, "[%*d]", _sz, _elt_right->i);
     }
     else {
          sprintf(_right, " %*s ", _sz, "");
     }

     /* Initialize up value
        ~~~~~~~~~~~~~~~~~~~ */
     if (node->up) {
          _elt_up                       = node->up->data;
          sprintf(_up, "[%*d]", _sz, _elt_up->i);
     }
     else {
          sprintf(_up, " %*s ", _sz, "");
     }

     /* Initialize "where" value
        ~~~~~~~~~~~~~~~~~~~~~~~~ */
     switch (node->where) {
     
     case CI_DOWN:
          _where                   = "DOWN ";
          break;

     case CI_LEFT:
          _where                   = "LEFT ";
          break;

     case CI_RIGHT:
          _where                   = "RIGHT";
          break;

     default:
          _where                   = " ??? ";
          break;
     }

     _H_left                  = node->H_left;
     _H_right                 = node->H_right;

     /* Display node info on one line
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
     printf("%% %s : left = %s right = %s up = %s where = %s H_left = %2d H_right = %2d\n",
            _value, _left, _right, _up, _where, _H_left, _H_right);

}

/******************************************************************************

                         EA_INDENT

******************************************************************************/
void ea_indent(int indent)
{
     int                            _i;

     for (_i = 0; _i < indent; _i++) {
          printf("     ");
     }
}

/******************************************************************************

                         EA_DISPLAY_LATEX_NODE

******************************************************************************/
void ea_display_latex_node(ci_node *node, cc_uint16 phase)
{
     ea_ref_int                     _elt;
     int                            _leaf;
     static int                     _level = 0;

     _elt                          = node->data;
     _leaf                         = node->left == 0 && node->right == 0;

     if (phase == CI_PHASE_BEGIN) {
          if (_leaf) {
               _level++;
               ea_indent(_level);
               printf("{%d}\n", _elt->i);
               _level--;
          }
          else {
               _level++;
               ea_indent(_level);
               printf("[.");
               printf("%d\n", _elt->i);
          }

          if ((node->left == 0) && (node->right != 0)) {
               _level++;
               ea_indent(_level);
               printf("\\edge[blank]; \\node[blank]{};\n");
               _level--;
          }
     }
     else {
          if (!_leaf) {
               ea_indent(_level);
               printf("]\n");
               _level--;
          }
     }
}

/******************************************************************************

                              EA_DISP_STRINGS_ARRAY

******************************************************************************/
void ea_disp_strings_array(char **array)
{
     char                     **_p;

     for (_p = array; *_p != 0; _p++) {
          printf("%s\n", *_p);
     }
}

/*******************************************************************************

                              MAIN

*******************************************************************************/
int main(int argc, char *argv[])
{
     cc_uint32                      _i, _max, _nb_elts;
     ea_ref_int                     _elt;
     ci_root                        _root;

     /* Check arguments
        ~~~~~~~~~~~~~~~ */
     if (argc != 2) {
          ea_usage(argv[0]);
          exit(1);
     }
     _max                          = atol(argv[1]);

     /* Init tree root
        ~~~~~~~~~~~~~~ */
     ci_init_root(&_root);

     /* Loop from 1 to _max
        ~~~~~~~~~~~~~~~~~~~ */
     for (_i = 1; _i <= _max; _i++) {
//   for (_i = _max; _i > 0; _i--) {
          /* Allocate an element
             ~~~~~~~~~~~~~~~~~~~ */
          _elt                          = ea_new_int(_i);

          /* Add the element into the tree
             ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
          if (ci_add_node(&_root, &_elt->node, ea_cmp_int, 0) != 0) {
               fprintf(stderr, "%s: add node error\n", argv[0]);
               exit(1);
          }
     }

     _nb_elts                      = _root.nb;
     printf("%% %d elements in the tree\n", _nb_elts);

     /* Display btree as an array in a LaTeX comment
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
     _nb_elts                      = ci_cross(_root.root, ea_disp_node_info, CI_T_LNR);

     /* Create the LaTeX file for the btree
        ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ */
     ea_disp_strings_array(ea_latex_header);
     _nb_elts                      = ci_cross(_root.root, ea_display_latex_node, CI_T_NLR);
     ea_disp_strings_array(ea_latex_footer);

     return 0;
}
