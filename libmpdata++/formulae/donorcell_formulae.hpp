/** @file
* @copyright University of Warsaw
* @section LICENSE
* GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
*/

#pragma once

#include <libmpdata++/formulae/idxperm.hpp>
#include <libmpdata++/formulae/opts.hpp>

namespace libmpdataxx
{
  namespace formulae
  {
    namespace donorcell
    {
      using namespace arakawa_c;
      using idxperm::pi;
      using opts::opts_t;

      const int n_tlev = 2, halo = 1;

      template<opts_t opts, class T1, class T2, class T3> 
      inline auto F(
	const T1 &psi_l, const T2 &psi_r, const T3 &GC
      ) return_macro(, 
        pospart<opts>(GC) * psi_l +
        negpart<opts>(GC) * psi_r
      ) 

      template <opts_t opts, class arr_1d_t>
      inline auto donorcell( 
	const arr_1d_t &psi, 
	const arr_1d_t &GC, 
	const rng_t &i
      ) return_macro(,
	F<opts>(
	  psi(i  ), 
	  psi(i+1), 
	   GC(i+h)
	) -
	F<opts>(
	  psi(i-1), 
	  psi(i  ), 
	   GC(i-h)
	)
      )

      template<opts_t opts, int d, class arr_2d_t>  
      inline auto donorcell( 
	const arr_2d_t &psi, 
	const arr_2d_t &GC, 
	const rng_t &i, 
	const rng_t &j
      ) return_macro(,
	F<opts>(
	  psi(pi<d>(i,   j)), 
	  psi(pi<d>(i+1, j)), 
	   GC(pi<d>(i+h, j))
	) -
	F<opts>(
	  psi(pi<d>(i-1, j)), 
	  psi(pi<d>(i,   j)), 
	   GC(pi<d>(i-h, j))
	)
      )

      template<opts_t opts, int d, class arr_3d_t>  
      inline auto donorcell( 
	const arr_3d_t &psi, 
	const arr_3d_t &GC, 
	const rng_t &i, 
	const rng_t &j,
	const rng_t &k
      ) return_macro(,
	F<opts>(
	  psi(pi<d>(i,   j, k)), 
	  psi(pi<d>(i+1, j, k)), 
	   GC(pi<d>(i+h, j, k))
	) -
	F<opts>(
	  psi(pi<d>(i-1, j, k)), 
	  psi(pi<d>(i,   j, k)), 
	   GC(pi<d>(i-h, j, k))
	)
      )

      template <opts_t opts, class arr_1d_t>
      void op_1d(
	const arrvec_t<arr_1d_t> &psi, 
	const arr_1d_t &GC, 
	const arr_1d_t &G, 
	const int n,
	const rng_t &i
      ) { 
	psi[n+1](i) = psi[n](i)
	  - donorcell<opts>(psi[n], GC, i) / formulae::G<opts>(G, i);
      }

      // infinite-gauge version (referred to as F(1,1,U) in the papers)
      template <opts_t opts, class arr_1d_t>
      void op_1d_iga(
	const arrvec_t<arr_1d_t> &psi, 
	const arr_1d_t &GC, 
	const arr_1d_t &G, 
	const int n,
	const rng_t &i
      ) { 
	psi[n+1](i) = psi[n](i) - (GC(i+h) - GC(i-h)) / formulae::G<opts>(G, i);
      }

      template <opts_t opts, class arr_2d_t>
      void op_2d(
	const arrvec_t<arr_2d_t> &psi,
	const arrvec_t<arr_2d_t> &GC, 
	const arr_2d_t &G, 
        const int n,
	const rng_t &i, const rng_t &j
      ) { 
	psi[n+1](i,j) = psi[n](i,j) - (             // note: this parenthesis is crucial!
	  donorcell<opts, 0>(psi[n], GC[0], i, j) + //       without it, magnitude difference
	  donorcell<opts, 1>(psi[n], GC[1], j, i)   //       between psi and the fluxes
        ) / formulae::G<opts, 0>(G, i, j);          //       may cause psi-0 != psi !
      }

      // infinite-gauge version (referred to as F(1,1,U) in the papers)
      template <opts_t opts, class arr_2d_t>
      void op_2d_iga(
	const arrvec_t<arr_2d_t> &psi,
	const arrvec_t<arr_2d_t> &GC, 
	const arr_2d_t &G, 
        const int n,
	const rng_t &i, const rng_t &j
      ) { 


	psi[n+1](i,j) = psi[n](i,j) - (             // note: see above
	  (GC[0](i+h, j) - GC[0](i-h, j)) +
	  (GC[1](i, j+h) - GC[1](i, j-h))
        ) / formulae::G<opts, 0>(G, i, j); 

/*
auto ix = minIndex(psi[n+1]);

std::cerr << "  ix=" << ix << std::endl;

std::cerr 
  << "  " << psi[n](ix[0], ix[1]) << std::endl
  << "  " << GC[0](ix[0]+0, ix[1]) - GC[0](ix[0]-1, ix[1]) +  GC[1](ix[0], ix[1]+0) - GC[1](ix[0], ix[1]-1) << std::endl
  << "  " << psi[n+1](ix[0], ix[1]) 
  << std::endl;
*/

      }

      template <opts_t opts, class arr_3d_t>
      void op_3d(
	const arrvec_t<arr_3d_t> &psi, 
	const arrvec_t<arr_3d_t> &GC, 
	const arr_3d_t &G, 
        const int n,
	const rng_t &i, const rng_t &j, const rng_t &k
      ) { 
	psi[n+1](i,j,k) = psi[n](i,j,k) - (             // note: see above
	  donorcell<opts, 0>(psi[n], GC[0], i, j, k) +
	  donorcell<opts, 1>(psi[n], GC[1], j, k, i) +
	  donorcell<opts, 2>(psi[n], GC[2], k, i, j)
        ) / formulae::G<opts, 0>(G, i, j, k);
      }
      
      // infinite-gauge version (referred to as F(1,1,U) in the papers)
      template <opts_t opts, class arr_3d_t>
      void op_3d_iga(
	const arrvec_t<arr_3d_t> &psi,
	const arrvec_t<arr_3d_t> &GC, 
	const arr_3d_t &G, 
        const int n,
	const rng_t &i, const rng_t &j, const rng_t &k
      ) { 
	psi[n+1](i,j,k) = psi[n](i,j,k) - (             // note: see above
	  (GC[0](i+h, j, k) - GC[0](i-h, j, k)) +
	  (GC[1](i, j+h, k) - GC[1](i, j-h, k)) +
	  (GC[2](i, j, k+h) - GC[2](i, j, k-h))
        ) / formulae::G<opts, 0>(G, i, j, k); 
      }

    }; // namespace donorcell 
  }; // namespace formulae
}; // namespace libmpdataxx
