// Copyright (c) 2003,2004  INRIA Sophia-Antipolis (France) and
// Notre Dame University (U.S.A.).  All rights reserved.
//
// This file is part of CGAL (www.cgal.org); you may redistribute it under
// the terms of the Q Public License version 1.0.
// See the file LICENSE.QPL distributed with CGAL.
//
// Licensees holding a valid commercial license may use this file in
// accordance with the commercial license agreement provided with the software.
//
// This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
// WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
//
// $Source$
// $Revision$ $Date$
// $Name$
//
// Author(s)     : Menelaos Karavelas <mkaravel@cse.nd.edu>



#ifndef CGAL_SVD_INCIRCLE_2_H
#define CGAL_SVD_INCIRCLE_2_H

#include <CGAL/predicates/Segment_Voronoi_diagram_vertex_2.h>
#include <CGAL/predicates/Svd_are_same_points_C2.h>

CGAL_BEGIN_NAMESPACE

//---------------------------------------------------------------------

template<class K, class Method_tag>
class Svd_incircle_2
{
private:
  typedef typename K::Point_2                       Point_2;
  typedef typename K::Segment_2                     Segment_2;
  typedef typename K::Site_2                        Site_2;
  typedef CGAL::Svd_voronoi_vertex_2<K,Method_tag>  Voronoi_vertex_2;

  typedef typename K::FT                            FT;
  typedef typename K::RT                            RT;

  typedef Svd_are_same_points_C2<K>  Are_same_points_C2;

private:
  Are_same_points_C2  are_same;

  bool same_segments(const Site_2& p, const Site_2& q) const
  {
    CGAL_precondition( p.is_segment() && q.is_segment() );

    return
      ( are_same(p.source_site(), q.source_site()) &&
        are_same(p.target_site(), q.target_site()) ) ||
      ( are_same(p.source_site(), q.target_site()) &&
        are_same(p.target_site(), q.source_site()) );
  }

  bool is_on_common_support(const Site_2& s1, const Site_2& s2,
			    const Point_2& p) const
  {
    CGAL_precondition( !s1.is_exact() && !s2.is_exact() );

    if (  same_segments(s1.supporting_site(0),
			s2.supporting_site(0)) ||
	  same_segments(s1.supporting_site(0),
			s2.supporting_site(1))  ) {
      Site_2 support = s1.supporting_site(0);
      return (  are_same(support.source_site(), p) ||
		are_same(support.target_site(), p)  );
    } else if (  same_segments(s1.supporting_site(1),
			       s2.supporting_site(1)) ||
		 same_segments(s1.supporting_site(1),
			       s2.supporting_site(0))  ) {
      Site_2 support = s1.supporting_site(1);
      return (  are_same(support.source_site(), p) ||
		are_same(support.target_site(), p)  );      
    }
    return false;
  }

  bool have_common_support(const Site_2& p, const Site_2& q) const
  {
    CGAL_precondition( !p.is_exact() && !q.is_exact() );

    return
      same_segments(p.supporting_site(0), q.supporting_site(0)) ||
      same_segments(p.supporting_site(0), q.supporting_site(1)) ||
      same_segments(p.supporting_site(1), q.supporting_site(1)) ||
      same_segments(p.supporting_site(1), q.supporting_site(0));
  }

  bool have_common_support(const Site_2& s, const Point_2& p1,
			   const Point_2& p2) const
  {
    CGAL_precondition( !s.is_exact() );

    Site_2 t(p1, p2);
    return ( same_segments(s.supporting_site(0), t) ||
	     same_segments(s.supporting_site(1), t) );
  }

private:
  Sign incircle_p(const Site_2& p, const Site_2& q,
		  const Site_2& t) const
  {
    CGAL_precondition( t.is_point() );

    if ( p.is_point() && q.is_point() ) {

      Orientation o;

      // do some geometric filtering...
      bool p_exact = p.is_exact();
      bool q_exact = q.is_exact();
      bool t_exact = t.is_exact();
      bool filtered = false;
      // the following if-statement does the gometric filtering...
      // maybe it is not so important since this will only be
      // activated if a lot of intersection points appear on the
      // convex hull
      if ( !p_exact || !q_exact || !t_exact ) {
	if ( !p_exact && !q_exact && !t_exact ) {
	  if ( have_common_support(p, q) &&
	       have_common_support(q, t) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	} else if ( !p_exact && !q_exact && t_exact ) {
	  if ( is_on_common_support(p, q, t.point()) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	} else if ( !p_exact && q_exact && !t_exact ) {
	  if ( is_on_common_support(p, t, q.point()) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	} else if ( p_exact && !q_exact && !t_exact ) {
	  if ( is_on_common_support(t, q, p.point()) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	} else if ( !p_exact && q_exact && t_exact ) {
	  if ( have_common_support(p, q.point(), t.point()) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	} else if ( p_exact && !q_exact && t_exact ) {
	  if ( have_common_support(q, p.point(), t.point()) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	} else if ( p_exact && q_exact && !t_exact ) {
	  if ( have_common_support(t, p.point(), q.point()) ) {
	    o = COLLINEAR;
	    filtered = true;
	  }
	}
      }

      Point_2 pp = p.point(), qp = q.point(), tp = t.point();

      if ( !filtered ) {
	// MK::ERROR: here I should call a kernel object, not a
	// function...
	o = orientation(pp, qp, tp);
      }

      if ( o != COLLINEAR ) {
	return (o == LEFT_TURN) ? POSITIVE : NEGATIVE;
      }

      // MK::ERROR: change the following code to use the compare_x_2
      // and compare_y_2 stuff...
      RT dtpx = pp.x() - tp.x();
      RT dtpy = pp.y() - tp.y();
      RT dtqx = qp.x() - tp.x();
      RT minus_dtqy = -qp.y() + tp.y();
      
      Sign s = sign_of_determinant2x2(dtpx, dtpy, minus_dtqy, dtqx);

      CGAL_assertion( s != ZERO );

      return s;
    }

    CGAL_assertion( p.is_point() || q.is_point() );

    Orientation o;
    if ( p.is_point() && q.is_segment() ) {
      Point_2 pq = are_same(p, q.source_site()) ? q.target() : q.source();
      o = orientation(p.point(), pq, t.point());
    } else { // p is a segment and q is a point
      Point_2 pp = are_same(q, p.source_site()) ? p.target() : p.source();
      o = orientation(pp, q.point(), t.point());
    }
    return ( o == RIGHT_TURN ) ? NEGATIVE : POSITIVE;
  }

  //-----------------------------------------------------------------------


  Sign incircle_pps(const Site_2& p, const Site_2& q,
		    const Site_2& t) const
  {
    CGAL_precondition( p.is_point() && q.is_point() );
    //    CGAL_precondition( t.is_segment() );

    bool is_p_tsrc = are_same(p, t.source_site());
    bool is_p_ttrg = are_same(p, t.target_site());

    bool is_q_tsrc = are_same(q, t.source_site());
    bool is_q_ttrg = are_same(q, t.target_site());

    bool is_p_on_t = is_p_tsrc || is_p_ttrg;
    bool is_q_on_t = is_q_tsrc || is_q_ttrg;

    if ( is_p_on_t && is_q_on_t ) {
	// if t is the segment joining p and q then t must be a vertex
	// on the convex hull
	return NEGATIVE;
    } else if ( is_p_on_t ) {
      // p is an endpoint of t
      // in this case the p,q,oo vertex is destroyed only if the
      // other endpoint of t is beyond
      Point_2 pt = is_p_tsrc ? t.target() : t.source();
      Orientation o = orientation(p.point(), q.point(), pt);

      return (o == RIGHT_TURN) ? NEGATIVE : POSITIVE;
    } else if ( is_q_on_t ) {
      Point_2 pt = is_q_tsrc ? t.target() : t.source();
      Orientation o = orientation(p.point(), q.point(), pt);

      return (o == RIGHT_TURN) ? NEGATIVE : POSITIVE;
    } else {
      // maybe here I should immediately return POSITIVE;
      // since we insert endpoints of segments first, p and q cannot
      // be consecutive points on the convex hull if one of the
      // endpoints of t is to the right of the line pq.
      Point_2 pp = p.point(), qq = q.point();
      Orientation o1 = orientation(pp, qq, t.source());
      Orientation o2 = orientation(pp, qq, t.target());

      if ( o1 == RIGHT_TURN || o2 == RIGHT_TURN ) {
	return NEGATIVE;
      }
      return POSITIVE;
    }
  }


  Sign incircle_sps(const Site_2& p, const Site_2& q,
		    const Site_2& t) const
  {
    CGAL_precondition( p.is_segment() && q.is_point() );
    //    CGAL_precondition( t.is_segment() );

    bool is_q_tsrc = are_same(q, t.source_site());
    bool is_q_ttrg = are_same(q, t.target_site());

    bool is_q_on_t = is_q_tsrc || is_q_ttrg;

    //    if ( q == t.source() && q == t.target() ) {
    if ( is_q_on_t ) {
      Point_2 pp = are_same(q, p.source_site()) ? p.target() : p.source();
      Point_2 pt = is_q_tsrc ? t.target() : t.source();

      Orientation o = orientation(pp, q.point(), pt);

      return (o == RIGHT_TURN) ? NEGATIVE : POSITIVE;
    } else {
      return POSITIVE;
    }
  }


  Sign incircle_pss(const Site_2& p, const Site_2& q,
		    const Site_2& t) const
  {
    CGAL_precondition( p.is_point() && q.is_segment() );
    //    CGAL_precondition( t.is_segment() );

    bool is_p_tsrc = are_same(p, t.source_site());
    bool is_p_ttrg = are_same(p, t.target_site());

    bool is_p_on_t = is_p_tsrc || is_p_ttrg;

    if ( is_p_on_t ) {
      Point_2 pq = are_same(p, q.source_site()) ? q.target() : q.source();
      Point_2 pt = is_p_tsrc ? t.target() : t.source();

      Orientation o = orientation(p.point(), pq, pt);

      return (o == RIGHT_TURN) ? NEGATIVE : POSITIVE;
    } else {
      // if p is not an endpoint of t, then either p and q should
      // not be on the convex hull or t does not affect the vertex
      // of p and q.
      return POSITIVE;
    }
  }


  Sign incircle_s(const Site_2& p, const Site_2& q,
		  const Site_2& t) const
  {
    CGAL_precondition( t.is_segment() );

    if ( p.is_point() && q.is_point() ) {
      return incircle_pps(p, q, t);
    } else if ( p.is_point() && q.is_segment() ) {
      return incircle_pss(p, q, t);
    } else { // p is a segment and q is a point
      return incircle_sps(p, q, t);
    }
  }


public:

  Sign operator()(const Site_2& p, const Site_2& q,
		  const Site_2& r, const Site_2& t) const
  {
#if 0
    if ( p.is_point() && q.is_point() &&
	 r.is_point() && t.is_point() ) {
      RT x = p.point().x();
      Object o = make_object(x);
      Gmpq qx;
      if ( assign(qx, o) ) {
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "inside vertex conflict top "
		  << "level operator()" << std::endl;
	std::cout << "p: " << p << " exact? " << p.is_exact() << std::endl;
	std::cout << "q: " << q << " exact? " << q.is_exact() << std::endl;
	std::cout << "r: " << r << " exact? " << r.is_exact() << std::endl;
	std::cout << "t: " << t << " exact? " << t.is_exact() << std::endl;
	std::cout << "-------------------------------------" <<	std::endl;
      }
    }
#endif

    Voronoi_vertex_2 v(p, q, r);

    return v.incircle(t);
  }


  

  Sign operator()(const Site_2& p, const Site_2& q,
		  const Site_2& t) const
  {
#if 0
    if ( p.is_point() && q.is_point() && t.is_point() ) {
      RT x = p.point().x();
      Object o = make_object(x);
      Gmpq qx;
      if ( assign(qx, o) ) {
	std::cout << "+++++++++++++++++++++++++++++++++++++" << std::endl;
	std::cout << "inside vertex conflict top "
		  << "level operator()" << std::endl;
	std::cout << "p: " << p << " exact? " << p.is_exact() << std::endl;
	std::cout << "q: " << q << " exact? " << q.is_exact() << std::endl;
	std::cout << "t: " << t << " exact? " << t.is_exact() << std::endl;
	std::cout << "-------------------------------------" << std::endl;
      }
    }
#endif

    CGAL_assertion( !(p.is_segment() && q.is_segment()) );

    if ( p.is_point() && q.is_segment() ) {
      // p must be an endpoint of q
      CGAL_assertion( are_same(p, q.source_site()) ||
		      are_same(p, q.target_site()) );
    } else if ( p.is_segment() && q.is_point() ) {
      // q must be an endpoint of p
      CGAL_assertion( are_same(p.source_site(), q) ||
		      are_same(p.target_site(), q) );
    }

    if ( t.is_point() ) {
      return incircle_p(p, q, t);
    }

    return incircle_s(p, q, t);
  }


};

//---------------------------------------------------------------------

CGAL_END_NAMESPACE

#endif // CGAL_SVD_INCIRCLE_2_H
