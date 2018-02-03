/*************************************************************************

 hypervolume computation

 ---------------------------------------------------------------------

                       Copyright (c) 2005, 2006
                  Carlos Fonseca <cmfonsec@ualg.pt>
             Manuel Lopez-Ibanez <m.lopez-ibanez@napier.ac.uk>
                    Luis Paquete <lpaquete@ualg.pt>

 This program is free software (software libre); you can redistribute
 it and/or modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2 of the
 License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but
 WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, you can obtain a copy of the GNU
 General Public License at:
                 http://www.gnu.org/copyleft/gpl.html
 or by writing to:
           Free Software Foundation, Inc., 59 Temple Place,
                 Suite 330, Boston, MA 02111-1307 USA

 ----------------------------------------------------------------------

 Relevant literature:

 [1]  C. M. Fonseca, L. Paquete, and M. Lopez-Ibanez. An
      improved dimension-sweep algorithm for the hypervolume
      indicator. In IEEE Congress on Evolutionary Computation,
      pages 1157-1163, Vancouver, Canada, July 2006.

 [2]  L. Paquete, C. M. Fonseca and M. Lopez-Ibanez. An optimal
      algorithm for a special case of Klee's measure problem in three
      dimensions. Technical Report CSI-RT-I-01/2006, CSI, Universidade
      do Algarve, 2006.

*************************************************************************/

#include "hv.h"
#include "avl.h"

#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <float.h>

typedef struct dlnode {
  double *x;                    /* The data vector              */
  struct dlnode **next;         /* Next-node vector             */
  struct dlnode **prev;         /* Previous-node vector         */
  struct avl_node_t * tnode;
  int ignore;
#if VARIANT >= 2
  double *area;                 /* Area */
#endif
#if VARIANT >= 3
  double *vol;                  /* Volume */
#endif
} dlnode_t;


static avl_tree_t *tree;
extern int stop_dimension;

static int compare_node( const void *p1, const void* p2)
{
    const double x1 = *((*(const dlnode_t **)p1)->x);
    const double x2 = *((*(const dlnode_t **)p2)->x);

    if ( x1 == x2 )
        return 0;
    else
        return ( x1 < x2 ) ? -1 : 1;
}

static int compare_tree_asc( const void *p1, const void *p2)
{
    const double x1= *((const double *)p1+1);
    const double x2= *((const double *)p2+1);

    if (x1 != x2)
        return (x1 > x2) ? -1 : 1;
    else
        return 0;
}


/*
 * Setup circular double-linked list in each dimension
 */

static dlnode_t *
setup_cdllist(double *data, int d, int n)
{
    dlnode_t *head;
    dlnode_t **scratch;
    int i, j;

    head  = (dlnode_t *)malloc ((n+1) * sizeof(dlnode_t));

    head->x = data;
    head->ignore = 0;  /* should never get used */
    head->next = (dlnode_t**) malloc( d * (n+1) * sizeof(dlnode_t*));
    head->prev = (dlnode_t**) malloc( d * (n+1) * sizeof(dlnode_t*));
    head->tnode = (avl_node_t *) malloc( sizeof(avl_node_t));

#if VARIANT >= 2
    head->area = (double *) malloc(d * (n+1) * sizeof(double));
#endif
#if VARIANT >= 3
    head->vol = (double *) malloc(d * (n+1) * sizeof(double));
#endif

    for (i = 1; i <= n; i++) {
        head[i].x = head[i-1].x + d ;/* this will be fixed a few lines below... */
        head[i].ignore = 0;
        head[i].next = head[i-1].next + d;
        head[i].prev = head[i-1].prev + d;
        head[i].tnode = (avl_node_t*) malloc(sizeof(avl_node_t));
#if VARIANT >= 2
        head[i].area = head[i-1].area + d;
#endif
#if VARIANT >= 3
        head[i].vol = head[i-1].vol + d;
#endif
    }
    head->x = NULL; /* head contains no data */

    scratch = (dlnode_t **) malloc(n * sizeof(dlnode_t*));
    for (i = 0; i < n; i++)
        scratch[i] = head + i + 1;

    for (j = d-1; j >= 0; j--) {
        for (i = 0; i < n; i++)
            scratch[i]->x--;
        qsort(scratch, n, sizeof(dlnode_t*), compare_node);
        head->next[j] = scratch[0];
        scratch[0]->prev[j] = head;
        for (i = 1; i < n; i++) {
            scratch[i-1]->next[j] = scratch[i];
            scratch[i]->prev[j] = scratch[i-1];
        }
        scratch[n-1]->next[j] = head;
        head->prev[j] = scratch[n-1];
    }

    free(scratch);

    return head;
}

static void delete_ (dlnode_t *nodep, int dim, double * bound __variant3_only)
{
    int i;

    for (i = 0; i < dim; i++) {
        nodep->prev[i]->next[i] = nodep->next[i];
        nodep->next[i]->prev[i] = nodep->prev[i];
#if VARIANT >= 3
        if (bound[i] > nodep->x[i])
            bound[i] = nodep->x[i];
#endif
  }
}

static void reinsert (dlnode_t *nodep, int dim, double * bound __variant3_only)
{
    int i;

    for (i = 0; i < dim; i++) {
        nodep->prev[i]->next[i] = nodep;
        nodep->next[i]->prev[i] = nodep;
#if VARIANT >= 3
        if (bound[i] > nodep->x[i])
            bound[i] = nodep->x[i];
#endif
    }
}

static double
hv_recursive( dlnode_t *list, int dim, int c, const double * ref,
              double * bound)
{
    dlnode_t *p0,*p1,*pp;
    double hypera,hyperv=0;
    double height;


    /* ------------------------------------------------------
       General case for dimensions higher than stop_dimension
       ------------------------------------------------------ */
    if ( dim > stop_dimension ) {
        p0 = list;
#if VARIANT >= 2
        for (p1 = p0->prev[dim]; p1->x; p1 = p1->prev[dim]) {
            if (p1->ignore < dim)
                p1->ignore = 0;
        }
#endif
        while (
#if VARIANT >= 3
            p0->prev[dim]->x[dim] > bound[dim] &&
#endif
            c > 1 ) {
            p0 = p0->prev[dim];
            delete_(p0, dim, bound);
            c--;
        }
        p1 = p0->prev[dim];

#if VARIANT == 1
        hypera = hv_recursive(list, dim-1, c, ref, bound);
#elif VARIANT >= 3
        if (c > 1)
            hyperv = p1->prev[dim]->vol[dim] + p1->prev[dim]->area[dim]
                * (p1->x[dim] - p1->prev[dim]->x[dim]);
        else {
            p1->area[0] = 1;
            int i;
            for (i = 1; i <= dim; i++)
                p1->area[i] = p1->area[i-1] * (ref[i-1] - p1->x[i-1]);
        }
        p1->vol[dim] = hyperv;
#endif
#if VARIANT >= 2
        if (p1->ignore >= dim)
            p1->area[dim] = p1->prev[dim]->area[dim];
        else {
            p1->area[dim] = hv_recursive(list, dim-1, c, ref, bound);
            if (p1->area[dim] <= p1->prev[dim]->area[dim])
                p1->ignore = dim;
        }
#endif

        while (p0->x != NULL) {
            hyperv +=
#if VARIANT == 1
                hypera
#elif VARIANT >= 2
                p1->area[dim]
#endif
                * (p0->x[dim] - p1->x[dim]);
#if VARIANT >= 3
            bound[dim] = p0->x[dim];
#endif
            reinsert(p0, dim, bound);
            c++;
            p1 = p0;
            p0 = p0->next[dim];
#if VARIANT >= 3
            p1->vol[dim] = hyperv;
#endif
#if VARIANT == 1
            hypera = hv_recursive(list, dim-1, c, ref, NULL);
#elif VARIANT >= 2
            if (p1->ignore >= dim)
                p1->area[dim] = p1->prev[dim]->area[dim];
            else {
                p1->area[dim] = hv_recursive(list, dim-1, c, ref, bound);
                if (p1->area[dim] <= p1->prev[dim]->area[dim])
                    p1->ignore = dim;
            }
#endif
        }
        hyperv +=
#if VARIANT == 1
            hypera
#elif VARIANT >= 2
            p1->area[dim]
#endif
            * (ref[dim] - p1->x[dim]);

        return hyperv;
    }

    /* ---------------------------
       special case of dimension 3
       --------------------------- */
    else if (dim == 2) {

        pp = list->next[2];
        avl_init_node(pp->tnode,pp->x);
        avl_insert_top(tree,pp->tnode);

        hypera = (ref[0] - pp->x[0])*(ref[1] - pp->x[1]);

        if (c == 1)
            height = ref[2] - pp->x[2];
        else
            height = pp->next[2]->x[2] - pp->x[2];

        hyperv = hypera * height;

        while ((pp=pp->next[2])->x) {
            height = (pp==list->prev[2])
                ? ref[2] - pp->x[2]
                : pp->next[2]->x[2] - pp->x[2];
#if VARIANT >= 2
            if (pp->ignore>=2)
                hyperv += hypera * height;
            else {
#endif
                const double * prv_ip, * nxt_ip;
                avl_node_t *tnode;

                avl_init_node(pp->tnode, pp->x);

                if (avl_search_closest(tree, pp->x, &tnode) <= 0) {
                    nxt_ip = (double *)(tnode->item);
                    tnode = tnode->prev;
                } else {
                    nxt_ip = (tnode->next!=NULL)
                        ? (double *)(tnode->next->item)
                        : ref;
                }

                if (nxt_ip[0] > pp->x[0]) {

                    avl_insert_after(tree, tnode, pp->tnode);

                    if (tnode !=NULL) {
                        prv_ip = (double *)(tnode->item);

                        if (prv_ip[0] > pp->x[0]) {
                            const double * cur_ip;

                            tnode = pp->tnode->prev;
                            // cur_ip = point dominated by pp with highest [0]-coordinate
                            cur_ip = (double *)(tnode->item);
                            while (tnode->prev) {
                                prv_ip = (double *)(tnode->prev->item);
                                hypera -= (prv_ip[1] - cur_ip[1])*(nxt_ip[0] - cur_ip[0]);
                                if (prv_ip[0] < pp->x[0])
                                    break; // prv is not dominated by pp
                                cur_ip = prv_ip;
                                avl_unlink_node(tree,tnode);
                                tnode = tnode->prev;
                            }

                            avl_unlink_node(tree,tnode);

                            if (!tnode->prev) {
                                hypera -= (ref[1] - cur_ip[1])*(nxt_ip[0] - cur_ip[0]);
                                prv_ip = ref;
                            }
                        }
                    } else
                        prv_ip = ref;

                    hypera += (prv_ip[1] - pp->x[1])*(nxt_ip[0] - pp->x[0]);

                }
                else
                    pp->ignore = 2;

                if (height > 0)
                    hyperv += hypera * height;

#if VARIANT >= 2
            }
#endif
        }
        avl_clear_tree(tree);
        return hyperv;
    }

    /* special case of dimension 2 */
    else if (dim == 1) {
        p1 = list->next[1];
        hypera = p1->x[0];
        while ((p0 = p1->next[1])->x) {
            hyperv += (ref[0] - hypera) * (p0->x[1] - p1->x[1]);
            if (p0->x[0] < hypera)
                hypera = p0->x[0];
            p1 = p0;
        }
        hyperv += (ref[0] - hypera) * (ref[1] - p1->x[1]);
        return hyperv;
    }

    /* special case of dimension 1 */
    else if (dim == 0) {
        return (ref[0] - list->next[0]->x[0]);
    }

    else {
        fprintf(stderr, "%s:%d: unreachable condition! \n"
                "This is a bug, please report it to "
                "m.lopez-ibanez@napier.ac.uk\n", __FILE__, __LINE__);
        exit(EXIT_FAILURE);
    }
}

double hv(double *data, int d, int n, const double *ref)
{
    dlnode_t *list;
    double hyperv;
    double * bound = NULL;

#if VARIANT >= 3
    int i;

    bound = (double *) malloc (d * sizeof(double));
    for (i = 0; i < d; i++) bound[i] = -DBL_MAX;
#endif

    tree  = avl_alloc_tree ((avl_compare_t) compare_tree_asc,
                            (avl_freeitem_t) free);

    list = setup_cdllist(data, d, n);

    hyperv = hv_recursive(list, d-1, n, ref, bound);

		avl_free_tree(tree);
#if VARIANT >= 3
		free(bound);
#endif
	

    return hyperv;
}
