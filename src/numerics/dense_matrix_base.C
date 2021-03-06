// The libMesh Finite Element Library.
// Copyright (C) 2002-2014 Benjamin S. Kirk, John W. Peterson, Roy H. Stogner

// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.

// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.

// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

// C++ includes
#include <iomanip> // for std::setw()

// Local Includes
#include "libmesh/dense_matrix_base.h"
#include "libmesh/dense_vector_base.h"

namespace libMesh
{



template<typename T>
void DenseMatrixBase<T>::multiply (DenseMatrixBase<T>& M1,
				   const DenseMatrixBase<T>& M2,
				   const DenseMatrixBase<T>& M3)
{
  // Assertions to make sure we have been
  // passed matrices of the correct dimension.
  libmesh_assert_equal_to (M1.m(), M2.m());
  libmesh_assert_equal_to (M1.n(), M3.n());
  libmesh_assert_equal_to (M2.n(), M3.m());

  const unsigned int m_s = M2.m();
  const unsigned int p_s = M2.n();
  const unsigned int n_s = M1.n();

  // Do it this way because there is a
  // decent chance (at least for constraint matrices)
  // that M3(k,j) = 0. when right-multiplying.
  for (unsigned int k=0; k<p_s; k++)
    for (unsigned int j=0; j<n_s; j++)
      if (M3.el(k,j) != 0.)
	for (unsigned int i=0; i<m_s; i++)
	  M1.el(i,j) += M2.el(i,k) * M3.el(k,j);
}



template<typename T>
void DenseMatrixBase<T>::condense(const unsigned int iv,
				  const unsigned int jv,
				  const T val,
				  DenseVectorBase<T>& rhs)
{
  libmesh_assert_equal_to (this->_m, rhs.size());
  libmesh_assert_equal_to (iv, jv);


  // move the known value into the RHS
  // and zero the column
  for (unsigned int i=0; i<this->m(); i++)
    {
      rhs.el(i) -= this->el(i,jv)*val;
      this->el(i,jv) = 0.;
    }

  // zero the row
  for (unsigned int j=0; j<this->n(); j++)
    this->el(iv,j) = 0.;

  this->el(iv,jv) = 1.;
  rhs.el(iv) = val;

}


template<typename T>
void DenseMatrixBase<T>::print_scientific (std::ostream& os) const
{
#ifndef LIBMESH_BROKEN_IOSTREAM

  // save the initial format flags
  std::ios_base::fmtflags os_flags = os.flags();

  // Print the matrix entries.
  for (unsigned int i=0; i<this->m(); i++)
    {
      for (unsigned int j=0; j<this->n(); j++)
	os << std::setw(15)
	   << std::scientific
	   << std::setprecision(8)
	   << this->el(i,j) << " ";

      os << std::endl;
    }

  // reset the original format flags
  os.flags(os_flags);

#else

  // Print the matrix entries.
  for (unsigned int i=0; i<this->m(); i++)
    {
      for (unsigned int j=0; j<this->n(); j++)
	os << std::setprecision(8)
	   << this->el(i,j)
	   << " ";

      os << std::endl;
    }


#endif
}



template<>
void DenseMatrixBase<Real>::print (std::ostream& os) const
{
    unsigned int width = 17, precision = 9;
    
    out << "Size: " << this->m() << ",  " << this->n() << std::endl;
    for (unsigned int i=0; i< this->m(); i++)
    {
        for (unsigned int j=0; j < this->n(); j++)
            if (this->el(i,j) == 0.0)
                out << std::setw(width) << this->el(i,j);
            else if (log10(fabs(this->el(i,j))) >= -1)
                out << std::setw(width) << std::setprecision(precision) << this->el(i,j);
            else
                out << std::setw(width) << std::showpoint << std::setprecision(precision) << this->el(i,j);
        out << std::endl;
    }

  return;
}

    
template<>
void DenseMatrixBase<Complex>::print (std::ostream& os) const
{
    unsigned int width = 30, precision = 9;

    out << "Size: " << this->m() << ",  " << this->n() << std::endl;
    for (unsigned int i=0; i< this->m(); i++)
    {
        for (unsigned int j=0; j < this->n(); j++)
            if (std::abs(this->el(i,j)) == 0.0)
                out << std::setw(width) << this->el(i,j);
            else if (log10(std::abs(this->el(i,j))) >= -1)
                out << std::setw(width) << std::setprecision(precision) << this->el(i,j);
            else
                out << std::setw(width) << std::showpoint << std::setprecision(precision) << this->el(i,j);
        out << std::endl;
    }
    
    return;
}









//--------------------------------------------------------------
// Explicit instantiations
template class DenseMatrixBase<Real>;

#ifdef LIBMESH_USE_COMPLEX_NUMBERS
template class DenseMatrixBase<Complex>;
#endif

} // namespace libMesh
