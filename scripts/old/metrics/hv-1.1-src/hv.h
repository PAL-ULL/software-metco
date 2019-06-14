/*************************************************************************

 hv.h 

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


*************************************************************************/
#ifndef HV_H_
#define HV_H_

#if !defined(VARIANT) || VARIANT < 1 || VARIANT > 4
#error You should define a variant, e.g., 'make VARIANT=4'
#endif

double hv(double *data, int d, int n, const double *ref);

#if __GNUC__ >= 3
# define __unused	__attribute__ ((unused))
#else
# define __unused	/* no 'unused' attribute available */
#endif

#if VARIANT < 3
# define __variant3_only __unused
#else
# define __variant3_only
#endif

#if VARIANT < 2
# define __variant2_only __unused
#else
# define __variant2_only
#endif

#endif
