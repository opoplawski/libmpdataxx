// 1D cyclic boundary conditions for libmpdata++
//
// licensing: GPU GPL v3
// copyright: University of Warsaw

#pragma once

#include <libmpdata++/bcond/detail/bcond_common.hpp>

namespace libmpdataxx
{
  namespace bcond
  {
    template <typename real_t, bcond_e knd, drctn_e dir, int n_dims, int dim>    
    class bcond<       real_t,         knd,         dir,     n_dims,     dim,
      typename std::enable_if<
        knd == cyclic && 
        dir == left   && 
        n_dims == 1
      >::type
    > : public detail::bcond_common<real_t>
    {
      using parent_t = detail::bcond_common<real_t>;
      using arr_t = blitz::Array<real_t, 1>;
      using parent_t::parent_t; // inheriting ctor

      public:

      void fill_halos_sclr(const arr_t &a, const bool deriv = false)
      {
	a(this->left_halo_sclr) = a(this->rght_intr_sclr);
      }

      void fill_halos_vctr_alng(const arrvec_t<arr_t> &av)
      {
	av[0](this->left_halo_vctr) = av[0](this->rght_intr_vctr);
      }
    };

    template <typename real_t, bcond_e knd, drctn_e dir, int n_dims, int dim>
    class bcond<       real_t,         knd,         dir,     n_dims,     dim,
      typename std::enable_if<
        knd == cyclic &&
        dir == rght   &&
        n_dims == 1
      >::type
    > : public detail::bcond_common<real_t>
    {
      using parent_t = detail::bcond_common<real_t>;
      using arr_t = blitz::Array<real_t, 1>;
      using parent_t::parent_t; // inheriting ctor

      public:

      void fill_halos_sclr(const arr_t &a, const bool deriv = false)
      {
	a(this->rght_halo_sclr) = a(this->left_intr_sclr);
      }

      void fill_halos_vctr_alng(const arrvec_t<arr_t> &av)
      {
	av[0](this->rght_halo_vctr) = av[0](this->left_intr_vctr);
      }
      
    };
  }; // namespace bcond
}; // namespace libmpdataxx
