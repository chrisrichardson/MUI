/*****************************************************************************
* Multiscale Universal Interface Code Coupling Library                       *
*                                                                            *
* Copyright (C) 2019 Y. H. Tang, S. Kudo, X. Bian, Z. Li, G. E. Karniadakis  *
*                                                                            *
* This software is jointly licensed under the Apache License, Version 2.0    *
* and the GNU General Public License version 3, you may use it according     *
* to either.                                                                 *
*                                                                            *
* ** Apache License, version 2.0 **                                          *
*                                                                            *
* Licensed under the Apache License, Version 2.0 (the "License");            *
* you may not use this file except in compliance with the License.           *
* You may obtain a copy of the License at                                    *
*                                                                            *
* http://www.apache.org/licenses/LICENSE-2.0                                 *
*                                                                            *
* Unless required by applicable law or agreed to in writing, software        *
* distributed under the License is distributed on an "AS IS" BASIS,          *
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   *
* See the License for the specific language governing permissions and        *
* limitations under the License.                                             *
*                                                                            *
* ** GNU General Public License, version 3 **                                *
*                                                                            *
* This program is free software: you can redistribute it and/or modify       *
* it under the terms of the GNU General Public License as published by       *
* the Free Software Foundation, either version 3 of the License, or          *
* (at your option) any later version.                                        *
*                                                                            *
* This program is distributed in the hope that it will be useful,            *
* but WITHOUT ANY WARRANTY; without even the implied warranty of             *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              *
* GNU General Public License for more details.                               *
*                                                                            *
* You should have received a copy of the GNU General Public License          *
* along with this program.  If not, see <http://www.gnu.org/licenses/>.      *
*****************************************************************************/

/**
 * @file chrono_sampler_gauss.h
 * @author Y. H. Tang
 * @date 19 February 2014
 * @brief Temporal sampler that applies Gaussian interpolation and is
 * symmetric for past and future.
 */

#ifndef MUI_SAMPLER_TIME_GAUSS_H_
#define MUI_SAMPLER_TIME_GAUSS_H_

#include "../util.h"
#include "../config.h"

namespace mui {

template<typename CONFIG=default_config> class chrono_sampler_gauss {
public:
	using REAL       = typename CONFIG::REAL;
	using INT        = typename CONFIG::INT;
	using time_type  = typename CONFIG::time_type;
	
	chrono_sampler_gauss( time_type newcutoff, REAL newsigma ) {
		sigma  = newsigma;
		cutoff = newcutoff;
	}

	// Filter using single value for time
	template<typename TYPE>
	TYPE filter( time_type focus, const std::vector<std::pair<time_type, TYPE> > &points ) const {
		REAL wsum = REAL(0);
		TYPE vsum = TYPE(0);
		for( auto i: points ) {
			time_type dt = std::abs(i.first - focus);
			if ( dt < cutoff ) {
				REAL w = pow( 2*PI*sigma, -0.5 ) * exp( -0.5 * dt * dt / sigma );
				vsum += i.second * w;
				wsum += w;
			}
		}
		return ( wsum > std::numeric_limits<REAL>::epsilon() ) ? ( vsum / wsum ) : TYPE(0);
	}

	// Filter using two values for time (sub iteration)
	template<typename TYPE>
	TYPE filter( std::pair<time_type,time_type> focus, const std::vector<std::pair<std::pair<time_type,time_type>, TYPE> > &points ) const {
		REAL wsum = REAL(0);
		TYPE vsum = TYPE(0);
		for( auto i: points ) {
			time_type dt = std::abs(i.first.first - focus.first);
			if ( dt < cutoff ) {
				REAL w = pow( 2*PI*sigma, -0.5 ) * exp( -0.5 * dt * dt / sigma );
				vsum += i.second * w;
				wsum += w;
			}
		}
		return ( wsum > std::numeric_limits<REAL>::epsilon() ) ? ( vsum / wsum ) : TYPE(0);
	}

	time_type get_upper_bound( time_type focus ) const {
		return focus + cutoff;
	}

	time_type get_lower_bound( time_type focus ) const {
		return focus - cutoff;
	}

	time_type tolerance() const {
		return time_type(0);
	}

protected:
	time_type cutoff;
	REAL sigma;
};

}

#endif /* MUI_SAMPLER_GAUSS_H_ */
