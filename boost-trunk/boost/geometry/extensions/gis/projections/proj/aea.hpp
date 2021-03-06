#ifndef BOOST_GEOMETRY_PROJECTIONS_AEA_HPP
#define BOOST_GEOMETRY_PROJECTIONS_AEA_HPP

// Boost.Geometry - extensions-gis-projections (based on PROJ4)
// This file is automatically generated. DO NOT EDIT.

// Copyright (c) 2008-2011 Barend Gehrels, Amsterdam, the Netherlands.

// Use, modification and distribution is subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This file is converted from PROJ4, http://trac.osgeo.org/proj
// PROJ4 is originally written by Gerald Evenden (then of the USGS)
// PROJ4 is maintained by Frank Warmerdam
// PROJ4 is converted to Boost.Geometry by Barend Gehrels (Geodan, Amsterdam)

// Original copyright notice:

// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
// DEALINGS IN THE SOFTWARE.

#include <boost/concept_check.hpp>
#include <boost/math/special_functions/hypot.hpp>

#include <boost/geometry/extensions/gis/projections/impl/base_static.hpp>
#include <boost/geometry/extensions/gis/projections/impl/base_dynamic.hpp>
#include <boost/geometry/extensions/gis/projections/impl/projects.hpp>
#include <boost/geometry/extensions/gis/projections/impl/factory_entry.hpp>
#include <boost/geometry/extensions/gis/projections/impl/pj_msfn.hpp>
#include <boost/geometry/extensions/gis/projections/impl/pj_qsfn.hpp>
#include <boost/geometry/extensions/gis/projections/impl/pj_mlfn.hpp>

#include <boost/geometry/extensions/gis/projections/epsg_traits.hpp>

namespace boost { namespace geometry { namespace projection
{
    #ifndef DOXYGEN_NO_DETAIL
    namespace detail { namespace aea{
            static const double EPS10 = 1.e-10;
            static const double TOL7 = 1.e-7;
            static const int N_ITER = 15;
            static const double EPSILON = 1.0e-7;
            static const double TOL = 1.0e-10;

            struct par_aea
            {
                double    ec;
                double    n;
                double    c;
                double    dd;
                double    n2;
                double    rho0;
                double    rho;
                double    phi1;
                double    phi2;
                double    en[EN_SIZE];
                int        ellips;
            };





            /* determine latitude angle phi-1 */
                inline double
            phi1_(double qs, double Te, double Tone_es) {
                int i;
                double Phi, sinpi, cospi, con, com, dphi;

                Phi = asin (.5 * qs);
                if (Te < EPSILON)
                    return( Phi );
                i = N_ITER;
                do {
                    sinpi = sin (Phi);
                    cospi = cos (Phi);
                    con = Te * sinpi;
                    com = 1. - con * con;
                    dphi = .5 * com * com / cospi * (qs / Tone_es -
                       sinpi / com + .5 / Te * log ((1. - con) /
                       (1. + con)));
                    Phi += dphi;
                } while (fabs(dphi) > TOL && --i);
                return( i ? Phi : HUGE_VAL );
            }

            // template class, using CRTP to implement forward/inverse
            template <typename Geographic, typename Cartesian, typename Parameters>
            struct base_aea_ellipsoid : public base_t_fi<base_aea_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>
            {

                 typedef double geographic_type;
                 typedef double cartesian_type;

                mutable par_aea m_proj_parm;

                inline base_aea_ellipsoid(const Parameters& par)
                    : base_t_fi<base_aea_ellipsoid<Geographic, Cartesian, Parameters>,
                     Geographic, Cartesian, Parameters>(*this, par) {}

                inline void fwd(geographic_type& lp_lon, geographic_type& lp_lat, cartesian_type& xy_x, cartesian_type& xy_y) const
                {
                    if ((this->m_proj_parm.rho = this->m_proj_parm.c - (this->m_proj_parm.ellips ? this->m_proj_parm.n * pj_qsfn(sin(lp_lat),
                        this->m_par.e, this->m_par.one_es) : this->m_proj_parm.n2 * sin(lp_lat))) < 0.) throw proj_exception();
                    this->m_proj_parm.rho = this->m_proj_parm.dd * sqrt(this->m_proj_parm.rho);
                    xy_x = this->m_proj_parm.rho * sin( lp_lon *= this->m_proj_parm.n );
                    xy_y = this->m_proj_parm.rho0 - this->m_proj_parm.rho * cos(lp_lon);
                }

                inline void inv(cartesian_type& xy_x, cartesian_type& xy_y, geographic_type& lp_lon, geographic_type& lp_lat) const
                {
                    if( (this->m_proj_parm.rho = boost::math::hypot(xy_x, xy_y = this->m_proj_parm.rho0 - xy_y)) != 0.0 ) {
                        if (this->m_proj_parm.n < 0.) {
                            this->m_proj_parm.rho = -this->m_proj_parm.rho;
                            xy_x = -xy_x;
                            xy_y = -xy_y;
                        }
                        lp_lat =  this->m_proj_parm.rho / this->m_proj_parm.dd;
                        if (this->m_proj_parm.ellips) {
                            lp_lat = (this->m_proj_parm.c - lp_lat * lp_lat) / this->m_proj_parm.n;
                            if (fabs(this->m_proj_parm.ec - fabs(lp_lat)) > TOL7) {
                                if ((lp_lat = phi1_(lp_lat, this->m_par.e, this->m_par.one_es)) == HUGE_VAL)
                                    throw proj_exception();
                            } else
                                lp_lat = lp_lat < 0. ? -HALFPI : HALFPI;
                        } else if (fabs(lp_lat = (this->m_proj_parm.c - lp_lat * lp_lat) / this->m_proj_parm.n2) <= 1.)
                            lp_lat = asin(lp_lat);
                        else
                            lp_lat = lp_lat < 0. ? -HALFPI : HALFPI;
                        lp_lon = atan2(xy_x, xy_y) / this->m_proj_parm.n;
                    } else {
                        lp_lon = 0.;
                        lp_lat = this->m_proj_parm.n > 0. ? HALFPI : - HALFPI;
                    }
                }
            };

            template <typename Parameters>
            void setup(Parameters& par, par_aea& proj_parm)
            {
                boost::ignore_unused_variable_warning(par);
                boost::ignore_unused_variable_warning(proj_parm);
                double cosphi, sinphi;
                int secant;
                if (fabs(proj_parm.phi1 + proj_parm.phi2) < EPS10) throw proj_exception(-21);
                proj_parm.n = sinphi = sin(proj_parm.phi1);
                cosphi = cos(proj_parm.phi1);
                secant = fabs(proj_parm.phi1 - proj_parm.phi2) >= EPS10;
                if( (proj_parm.ellips = (par.es > 0.))) {
                    double ml1, m1;
                    pj_enfn(par.es, proj_parm.en);
                    m1 = pj_msfn(sinphi, cosphi, par.es);
                    ml1 = pj_qsfn(sinphi, par.e, par.one_es);
                    if (secant) { /* secant cone */
                        double ml2, m2;
                        sinphi = sin(proj_parm.phi2);
                        cosphi = cos(proj_parm.phi2);
                        m2 = pj_msfn(sinphi, cosphi, par.es);
                        ml2 = pj_qsfn(sinphi, par.e, par.one_es);
                        proj_parm.n = (m1 * m1 - m2 * m2) / (ml2 - ml1);
                    }
                    proj_parm.ec = 1. - .5 * par.one_es * log((1. - par.e) /
                        (1. + par.e)) / par.e;
                    proj_parm.c = m1 * m1 + proj_parm.n * ml1;
                    proj_parm.dd = 1. / proj_parm.n;
                    proj_parm.rho0 = proj_parm.dd * sqrt(proj_parm.c - proj_parm.n * pj_qsfn(sin(par.phi0),
                        par.e, par.one_es));
                } else {
                    if (secant) proj_parm.n = .5 * (proj_parm.n + sin(proj_parm.phi2));
                    proj_parm.n2 = proj_parm.n + proj_parm.n;
                    proj_parm.c = cosphi * cosphi + proj_parm.n2 * sinphi;
                    proj_parm.dd = 1. / proj_parm.n;
                    proj_parm.rho0 = proj_parm.dd * sqrt(proj_parm.c - proj_parm.n2 * sin(par.phi0));
                }
                // par.inv = e_inverse;
                // par.fwd = e_forward;
            }


            // Albers Equal Area
            template <typename Parameters>
            void setup_aea(Parameters& par, par_aea& proj_parm)
            {
                proj_parm.phi1 = pj_param(par.params, "rlat_1").f;
                proj_parm.phi2 = pj_param(par.params, "rlat_2").f;
                setup(par, proj_parm);
            }

            // Lambert Equal Area Conic
            template <typename Parameters>
            void setup_leac(Parameters& par, par_aea& proj_parm)
            {
                proj_parm.phi2 = pj_param(par.params, "rlat_1").f;
                proj_parm.phi1 = pj_param(par.params, "bsouth").i ? - HALFPI: HALFPI;
                setup(par, proj_parm);
            }

        }} // namespace detail::aea
    #endif // doxygen

    /*!
        \brief Albers Equal Area projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Conic
         - Spheroid
         - Ellipsoid
         - lat_1= lat_2=
        \par Example
        \image html ex_aea.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct aea_ellipsoid : public detail::aea::base_aea_ellipsoid<Geographic, Cartesian, Parameters>
    {
        inline aea_ellipsoid(const Parameters& par) : detail::aea::base_aea_ellipsoid<Geographic, Cartesian, Parameters>(par)
        {
            detail::aea::setup_aea(this->m_par, this->m_proj_parm);
        }
    };

    /*!
        \brief Lambert Equal Area Conic projection
        \ingroup projections
        \tparam Geographic latlong point type
        \tparam Cartesian xy point type
        \tparam Parameters parameter type
        \par Projection characteristics
         - Conic
         - Spheroid
         - Ellipsoid
         - lat_1= south
        \par Example
        \image html ex_leac.gif
    */
    template <typename Geographic, typename Cartesian, typename Parameters = parameters>
    struct leac_ellipsoid : public detail::aea::base_aea_ellipsoid<Geographic, Cartesian, Parameters>
    {
        inline leac_ellipsoid(const Parameters& par) : detail::aea::base_aea_ellipsoid<Geographic, Cartesian, Parameters>(par)
        {
            detail::aea::setup_leac(this->m_par, this->m_proj_parm);
        }
    };

    #ifndef DOXYGEN_NO_DETAIL
    namespace detail
    {

        // Factory entry(s)
        template <typename Geographic, typename Cartesian, typename Parameters>
        class aea_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    return new base_v_fi<aea_ellipsoid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        class leac_entry : public detail::factory_entry<Geographic, Cartesian, Parameters>
        {
            public :
                virtual projection<Geographic, Cartesian>* create_new(const Parameters& par) const
                {
                    return new base_v_fi<leac_ellipsoid<Geographic, Cartesian, Parameters>, Geographic, Cartesian, Parameters>(par);
                }
        };

        template <typename Geographic, typename Cartesian, typename Parameters>
        inline void aea_init(detail::base_factory<Geographic, Cartesian, Parameters>& factory)
        {
            factory.add_to_factory("aea", new aea_entry<Geographic, Cartesian, Parameters>);
            factory.add_to_factory("leac", new leac_entry<Geographic, Cartesian, Parameters>);
        }

    } // namespace detail
    // Create EPSG specializations
    // (Proof of Concept, only for some)

    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<2964, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=55 +lat_2=65 +lat_0=50 +lon_0=-154 +x_0=0 +y_0=0 +ellps=clrk66 +datum=NAD27 +to_meter=0.3048006096012192";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3005, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=50 +lat_2=58.5 +lat_0=45 +lon_0=-126 +x_0=1000000 +y_0=0 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3083, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=27.5 +lat_2=35 +lat_0=18 +lon_0=-100 +x_0=1500000 +y_0=6000000 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3085, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=27.5 +lat_2=35 +lat_0=18 +lon_0=-100 +x_0=1500000 +y_0=6000000 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3086, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=24 +lat_2=31.5 +lat_0=24 +lon_0=-84 +x_0=400000 +y_0=0 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3087, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=24 +lat_2=31.5 +lat_0=24 +lon_0=-84 +x_0=400000 +y_0=0 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3153, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=50 +lat_2=58.5 +lat_0=45 +lon_0=-126 +x_0=1000000 +y_0=0 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3174, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=42.122774 +lat_2=49.01518 +lat_0=45.568977 +lon_0=-84.455955 +x_0=1000000 +y_0=1000000 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3175, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=42.122774 +lat_2=49.01518 +lat_0=45.568977 +lon_0=-83.248627 +x_0=1000000 +y_0=1000000 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3309, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=34 +lat_2=40.5 +lat_0=0 +lon_0=-120 +x_0=0 +y_0=-4000000 +ellps=clrk66 +datum=NAD27 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3310, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=34 +lat_2=40.5 +lat_0=0 +lon_0=-120 +x_0=0 +y_0=-4000000 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3311, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=34 +lat_2=40.5 +lat_0=0 +lon_0=-120 +x_0=0 +y_0=-4000000 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3338, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=55 +lat_2=65 +lat_0=50 +lon_0=-154 +x_0=0 +y_0=0 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3467, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=55 +lat_2=65 +lat_0=50 +lon_0=-154 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3488, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=34 +lat_2=40.5 +lat_0=0 +lon_0=-120 +x_0=0 +y_0=-4000000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3513, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=24 +lat_2=31.5 +lat_0=24 +lon_0=-84 +x_0=400000 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3577, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=-18 +lat_2=-36 +lat_0=0 +lon_0=132 +x_0=0 +y_0=0 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3578, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=61.66666666666666 +lat_2=68 +lat_0=59 +lon_0=-132.5 +x_0=500000 +y_0=500000 +ellps=GRS80 +datum=NAD83 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3579, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=61.66666666666666 +lat_2=68 +lat_0=59 +lon_0=-132.5 +x_0=500000 +y_0=500000 +ellps=GRS80 +units=m";
        }
    };


    template<typename LatLongRadian, typename Cartesian, typename Parameters>
    struct epsg_traits<3665, LatLongRadian, Cartesian, Parameters>
    {
        typedef aea_ellipsoid<LatLongRadian, Cartesian, Parameters> type;
        static inline std::string par()
        {
            return "+proj=aea +lat_1=27.5 +lat_2=35 +lat_0=18 +lon_0=-100 +x_0=1500000 +y_0=6000000 +ellps=GRS80 +towgs84=0,0,0,0,0,0,0 +units=m";
        }
    };


    #endif // doxygen

}}} // namespace boost::geometry::projection

#endif // BOOST_GEOMETRY_PROJECTIONS_AEA_HPP

