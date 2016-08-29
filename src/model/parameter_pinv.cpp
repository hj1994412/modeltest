/*
  Copyright (C) 2016 Diego Darriba

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Affero General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Affero General Public License for more details.

  You should have received a copy of the GNU Affero General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Contact: Diego Darriba <Diego.Darriba@h-its.org>,
  Heidelberg Institute for Theoretical Studies,
  Schloss-Wolfsbrunnenweg 35, D-69118 Heidelberg, Germany
*/

#include "partition.h"
#include "model/parameter_pinv.h"

#define MIN_PINV 0.02
#define MAX_PINV 0.95

using namespace std;

namespace modeltest
{

ParameterPinv::ParameterPinv( void )
{
  pinv = 0.5;
  min_pinv = MIN_PINV;
  max_pinv = MAX_PINV;
}

ParameterPinv::ParameterPinv( const ParameterPinv & other )
{
  pinv = other.pinv;
}

ParameterPinv::~ParameterPinv( void )
{

}

bool ParameterPinv::initialize(mt_opt_params_t * params,
                               Partition const& partition)
{
  max_pinv = partition.get_empirical_pinv();
  min_pinv = MIN_PINV;
  if (max_pinv > min_pinv)
  {
    pinv = (min_pinv + max_pinv) / 2;
  }
  else
  {
    pinv = max_pinv = MIN_PINV;
  }

  for (mt_index_t i=0; i<params->partition->rate_cats; ++i)
    pll_update_invariant_sites_proportion(params->partition,
                                          params->params_indices[i],
                                          pinv);
  return true;
}

double ParameterPinv::optimize(mt_opt_params_t * params,
                               double loglikelihood,
                               double tolerance,
                               bool first_guess)
{
  UNUSED(first_guess);
  double cur_logl;

  cur_logl = pllmod_algo_opt_pinv (params->partition,
                                   params->tree,
                                   params->params_indices,
                                   MIN_PINV,
                                   MAX_PINV,
                                   tolerance);

  pinv = params->partition->prop_invar[0];

  assert(!loglikelihood || (cur_logl - loglikelihood)/loglikelihood < 1e-10);

  return cur_logl;
}

void ParameterPinv::print(std::ostream  &out) const
{

}

double ParameterPinv::get_pinv() const
{
  return pinv;
}

void ParameterPinv::set_pinv( double value )
{
  pinv = value;
}

mt_size_t ParameterPinv::get_n_free_parameters( void ) const
{
  return 1;
}

} /* namespace modeltest */