/** @file 
* @copyright University of Warsaw
* @section LICENSE
* GPLv3+ (see the COPYING file or http://www.gnu.org/licenses/)
*/

#pragma once

#include <libmpdata++/solvers/detail/solver_common.hpp>
#include <libmpdata++/arakawa_c.hpp>
#include <libmpdata++/bcond/bcond.hpp>

namespace libmpdataxx
{
  namespace solvers
  {
    namespace detail
    {
      using namespace libmpdataxx::arakawa_c;

      template<typename real_t, int n_eqs, int n_tlev, int halo>
      class solver<real_t, 2, n_eqs, n_tlev, halo> : public solver_common<real_t, 2, n_eqs, n_tlev, halo>
      {
	using parent_t = solver_common<real_t, 2, n_eqs, n_tlev, halo>;

	protected:
      
        typedef std::unique_ptr<bcond::bcond_t<real_t>> bc_p; // TODO: move to parent
	bc_p bcxl, bcxr, bcyl, bcyr;

	rng_t i, j;
        idx_t<parent_t::n_dims> ijk;

	void xchng(int e, int lev = 0) // for previous time levels
	{
          this->xchng(this->mem->psi[e][ this->n[e] - lev], i^halo, j^halo);
	}

	void xchng(typename parent_t::arr_t psi, rng_t range_i, rng_t range_j) // for a given array
	{
          this->mem->barrier();
	  bcxl->fill_halos_sclr(psi, range_j);
	  bcxr->fill_halos_sclr(psi, range_j);
	  bcyl->fill_halos_sclr(psi, range_i);
	  bcyr->fill_halos_sclr(psi, range_i);
          this->mem->barrier();
	}

        public:
 
        struct ctor_args_t
        {   
          typename parent_t::mem_t *mem;
          bc_p &bcxl, &bcxr, &bcyl, &bcyr; 
          const rng_t &i, &j; 
        };  

        protected:

	// ctor
	solver(ctor_args_t args) :
	  parent_t(args.mem),
	  i(args.i), 
	  j(args.j),  
          ijk({args.i, args.j}),
	  bcxl(std::move(args.bcxl)), 
	  bcxr(std::move(args.bcxr)), 
	  bcyl(std::move(args.bcyl)),
	  bcyr(std::move(args.bcyr))
	{}

        // memory allocation logic using static methods

	public:

	static void alloc(typename parent_t::mem_t *mem, const int nx, const int ny) 
        {
          // psi 
	  for (int e = 0; e < n_eqs; ++e) // equations
	    for (int n = 0; n < n_tlev; ++n) // time levels
	      mem->psi[e].push_back( new typename parent_t::arr_t( parent_t::rng_sclr(nx), parent_t::rng_sclr(ny)));

          // Courant field components (Arakawa-C grid)
	  mem->C.push_back(new typename parent_t::arr_t( parent_t::rng_vctr(nx), parent_t::rng_sclr(ny) ));
	  mem->C.push_back(new typename parent_t::arr_t( parent_t::rng_sclr(nx), parent_t::rng_vctr(ny) ));
        }

        protected:

        // helper method to allocate a temporary space composed of vector-component arrays
        static void alloc_tmp_vctr(
          typename parent_t::mem_t *mem, const int nx, const int ny,
          const char * __file__
        )
        {
          mem->tmp[__file__].push_back(new arrvec_t<typename parent_t::arr_t>());
          mem->tmp[__file__].back().push_back(new typename parent_t::arr_t( parent_t::rng_vctr(nx), parent_t::rng_sclr(ny) )); 
          mem->tmp[__file__].back().push_back(new typename parent_t::arr_t( parent_t::rng_sclr(nx), parent_t::rng_vctr(ny) )); 
        }

        // helper method to allocate n_arr scalar temporary arrays 
        static void alloc_tmp_sclr(
          typename parent_t::mem_t *mem, const int nx, const int ny,
          const char * __file__, const int n_arr
        )   
        {   
          mem->tmp[__file__].push_back(new arrvec_t<typename parent_t::arr_t>());
          for (int n = 0; n < n_arr; ++n)
            mem->tmp[__file__].back().push_back(new typename parent_t::arr_t( 
              parent_t::rng_sclr(nx),
              parent_t::rng_sclr(ny)
            ));
        } 
      };
    }; // namespace detail
  }; // namespace solvers
}; // namespace libmpdataxx