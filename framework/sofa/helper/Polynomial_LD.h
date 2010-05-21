/******************************************************************************
*       SOFA, Simulation Open-Framework Architecture, version 1.0 beta 4      *
*                (c) 2006-2009 MGH, INRIA, USTL, UJF, CNRS                    *
*                                                                             *
* This library is free software; you can redistribute it and/or modify it     *
* under the terms of the GNU Lesser General Public License as published by    *
* the Free Software Foundation; either version 2.1 of the License, or (at     *
* your option) any later version.                                             *
*                                                                             *
* This library is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or       *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License *
* for more details.                                                           *
*                                                                             *
* You should have received a copy of the GNU Lesser General Public License    *
* along with this library; if not, write to the Free Software Foundation,     *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.          *
*******************************************************************************
*                              SOFA :: Framework                              *
*                                                                             *
* Authors: M. Adam, J. Allard, B. Andre, P-J. Bensoussan, S. Cotin, C. Duriez,*
* H. Delingette, F. Falipou, F. Faure, S. Fonteneau, L. Heigeas, C. Mendoza,  *
* M. Nesme, P. Neumann, J-P. de la Plata Alcade, F. Poyer and F. Roy          *
*                                                                             *
* Contact information: contact@sofa-framework.org                             *
******************************************************************************/
#ifndef SOFA_HELPER_POLYNOMIAL_LD_H
#define SOFA_HELPER_POLYNOMIAL_LD_H

#include <sofa/helper/helper.h>

//#include <sofa/component/component.h>
#include <sofa/defaulttype/Vec.h>
#include <sofa/defaulttype/Mat.h>
#include <sofa/helper/vector.h>
#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <cmath>
#include <cassert>
#include <cstdarg>

namespace sofa
{
namespace helper
{


using namespace sofa::defaulttype;
using namespace std;
/**
 * \file modules/sofa/component/femToolsForFEM.h
 * \namespace sofa::component::fem
 * \brief Polynomial_LD in Linear Design (opposed to Polynomial in Recurrent Design).
 *        in LD, polynomial are easier to implemented and more representative
 *        in RD, polynomial will be more efficient in evaluation operations
 *
 */
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

/**
 * \class template<typename Real, unsigned int N>Monomial_LD
 *
 * A generic monomial with <N> variable of type <Real>
 * A monomial is one term in the polynomial
 *
 * Todo this first version of polynomial is a linear presentation.
 * This presentation is efficient for arithmetic operator like +,-,\*,/ and derivative.
 * But is not optimized for the evaluating (comparing to the recurent presentation).
 */
template<typename Real, unsigned int N>
class Monomial_LD
{

public :

    typedef sofa::defaulttype::Vec<N,Real> RNpoint;
    ///to define the derivative operator m_mono.d(x), m_mono.d(y)
    enum {x,y,z,xy,yz,zx};

    Real coef;
    sofa::defaulttype::Vec<N,int>  powers;

    Monomial_LD();
    Monomial_LD(const Real &,...);
    Monomial_LD(const Monomial_LD<Real,N> & a);
    Monomial_LD<Real,N>& operator=(const Monomial_LD<Real,N> & b);

    ///Setting of Monomial_LD
    void SetCoef (const Real & m_coef) {coef=m_coef;}
    //void SetPower(const sofa::helper::vector<int> & m_powers)   {for(unsigned int i=0;i<N;i++) powers[i]=m_powers[i];}
    void SetPower(int numbervar,int powervalue) {powers[numbervar]=powervalue;}

    ///Return the total degree of monomial
    int degree();

    ///Logical operators
    bool operator ==(const Monomial_LD<Real,N> & b) const ;
    bool operator !=(const Monomial_LD<Real,N> & b) const {return !(*this == b);}
    bool isSamePowers(const Monomial_LD<Real,N> & b) const;
    bool isNULL() const {return (coef == (Real) 0.);}

    ///Mathematical operators
    Monomial_LD<Real,N> & operator*=(const Real & alpha) {this->coef*=alpha; return *this;}
    Monomial_LD<Real,N> & operator/=(const Real & alpha) {this->coef/=alpha; return *this;}
    Monomial_LD<Real,N> & operator+=(const Monomial_LD<Real,N> & b);
    Monomial_LD<Real,N> & operator-=(const Monomial_LD<Real,N> & b);
    Monomial_LD<Real,N> & operator*=(const Monomial_LD<Real,N> & b);
    Monomial_LD<Real,N> operator+ () const {Monomial_LD<Real,N> r(*this); return r;}
    Monomial_LD<Real,N> operator- () const {Monomial_LD<Real,N> r(*this); r*=(Real) -1.; return r;}

    Monomial_LD<Real,N> operator*(const Real & alpha) {Monomial_LD<Real,N> r(*this); r*=alpha; return r;}

    Monomial_LD<Real,N> operator/(const Real & alpha) {Monomial_LD<Real,N> r(*this); r/=alpha; return r;}
    Monomial_LD<Real,N> operator+(const Monomial_LD<Real,N> & a) {Monomial_LD<Real,N> r(*this); r+=a; return r;}
    Monomial_LD<Real,N> operator-(const Monomial_LD<Real,N> & a) {Monomial_LD<Real,N> r(*this); r-=a; return r;}
    Monomial_LD<Real,N> operator*(const Monomial_LD<Real,N> & a) {Monomial_LD<Real,N> r(*this); r*=a; return r;}

    ///Evaluating value
    Real operator()(const RNpoint & x) const;
    ///Evaluating first partial derivative by idvar-th variable
    Real operator()(const RNpoint & x,unsigned int idvar) const;

    ///Evaluating partial derivative hight order by idvar-th variable in ideriv-th order
    Real operator()(const RNpoint & x,unsigned int idvar,unsigned int ideriv) const;//Todo

    ///Derivative operator alowing to write p1=p2.d(x);
    Monomial_LD<Real,N> d(const unsigned int & ideriv) const;

    void writeToStream(std::ostream & ff) const;
    void readFromStream(std::istream & ff);

    template<typename FReal, unsigned int FN> //For comutativity of operator *: Monomial_LD*Real || Real*Monomial_LD.
    friend Monomial_LD<FReal,FN> & operator*(const FReal & alpha,Monomial_LD<FReal,FN> & r);

protected :
    sofa::defaulttype::Vec<N,string> variables;
};



/**
 * \class template<typename Real, unsigned int N>Polynomial_LD
 *
 * A generic polynomial with <N> variable of type <Real>
 * A polynomial is a list composed several term of monomial
 *
 */

template<typename Real, unsigned int N>
class Polynomial_LD
{

public :

    typedef sofa::helper::vector< Monomial_LD<Real,N> > MonomialsList;
    //typedef std::vector< Monomial_LD<Real,N> > MonomialsList;
    typedef typename MonomialsList::const_iterator MonomialConstIterator;
    typedef typename MonomialsList::iterator MonomialIterator;
    typedef sofa::defaulttype::Vec<N,Real> RNpoint;

    int nbOfMonomial;
    MonomialsList listOfMonoMial;

    ///Default constructor
    Polynomial_LD();

    ///Copy constructor
    Polynomial_LD(const Polynomial_LD<Real,N> & a);
    Polynomial_LD(const Monomial_LD<Real,N> & a);

    ///Assign operator
    Polynomial_LD<Real,N> & operator=(const Polynomial_LD<Real,N> & b)
    {listOfMonoMial=b.listOfMonoMial; nbOfMonomial=b.nbOfMonomial; return *this;}

    ///setting of Polynomial
    Polynomial_LD(const unsigned  int nbofTerm,...);
    void Set(const unsigned  int nbofTerm,...);

    int degree();

    ///Return true if a and b has the same powers
    bool operator ==(const Polynomial_LD<Real,N> & b) const ;
    bool operator !=(const Polynomial_LD<Real,N> & b) const {return !((*this) == b);}

    Polynomial_LD<Real,N>  & operator*=(const Real & alpha) ;
    Polynomial_LD<Real,N>  & operator/=(const Real & alpha) ;
    Polynomial_LD<Real,N>  & operator+=(const Monomial_LD<Real,N> & b) ;
    Polynomial_LD<Real,N>  & operator+=(const Polynomial_LD<Real,N> & b) ;
    Polynomial_LD<Real,N>  & operator-=(const Polynomial_LD<Real,N> & b) ;
    Polynomial_LD<Real,N>   operator+ () const {Polynomial_LD<Real,N> r(*this); return r;}
    Polynomial_LD<Real,N>   operator- () const;

    Polynomial_LD<Real,N>  & operator*=(const Polynomial_LD<Real,N> & b);

    Polynomial_LD<Real,N>  operator*(          const Real & alpha) {Polynomial_LD<Real,N> r(*this); r*=alpha; return r;}
    Polynomial_LD<Real,N>  operator/(          const Real & alpha) {Polynomial_LD<Real,N> r(*this); r/=alpha; return r;}
    Polynomial_LD<Real,N>  operator+(const Monomial_LD<Real,N>   & a) {Polynomial_LD<Real,N> r(*this); r+=a; return r;}
    Polynomial_LD<Real,N>  operator+(const Polynomial_LD<Real,N> & a) {Polynomial_LD<Real,N> r(*this); r+=a; return r;}
    Polynomial_LD<Real,N>  operator-(const Polynomial_LD<Real,N> & a) {Polynomial_LD<Real,N> r(*this); r-=a; return r;}
    Polynomial_LD<Real,N>  operator*(const Polynomial_LD<Real,N> & a) {Polynomial_LD<Real,N> r(*this); r*=a; return r;}

    ///Evaluating
    Real operator()(const RNpoint & x) const;
    ///Evaluating first partial derivative by idvar-th variable
    Real operator()(const RNpoint & x,unsigned int idvar) const;

    ///Evaluating partial derivative hight order by idvar-th variable in ideriv-th order
    Real operator()(const RNpoint & x,unsigned int idvar,unsigned int ideriv) const;//Todo

    ///Derivative operator alowing to write p1=p2.d(x);
    Polynomial_LD<Real,N>  d(const unsigned int & ideriv) const;

    void setnbOfMonomial(int m_nbofmonomial);
    void writeToStream(std::ostream & stream) const;
    void readFromStream(std::istream & stream);
    std::string getString() const;

    ///Comutativity of operator*(Real):
    ///Allowing to write p1=r*p2;   or   p1=p2*r;
    ///Polynomial_LD =  Polynomial_LD*Real || Real*Polynomial_LD.
    template<typename FReal, unsigned int FN>
    friend Polynomial_LD<FReal,FN> & operator*(const FReal & alpha,Polynomial_LD<FReal,FN> & r);

    /// poly=poly+mono || mono + poly
    template<typename FReal, unsigned int FN>
    friend Polynomial_LD<FReal,FN> & operator*(const Monomial_LD<FReal,FN>   & a, Polynomial_LD<FReal,FN> & r);

    void sort();
    void testPolynomial_LD();
protected :

    ///The sort must be done after each constructor and each operation where monomials are inserted
    void exchangeMonomial(unsigned int ithMono,unsigned  int jthMono);
    void sortByVar(unsigned int idVar);
};
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
template<typename FReal, unsigned int FN>
inline std::ostream & operator <<(std::ostream & out,const Monomial_LD<FReal,FN> & m_monomial)
{m_monomial.writeToStream(out); return out;}

template<typename FReal, unsigned int FN>
inline std::istream & operator >>(std::istream & in, Monomial_LD<FReal,FN> & m_monomial)
{m_monomial.readFromStream(in); return in;}

///////////////////////////////////////////

template<typename FReal, unsigned int FN>
inline std::ostream & operator<<(std::ostream & stream, const Polynomial_LD<FReal,FN> & m_polynomial )
{m_polynomial.writeToStream(stream); return stream;}

template<typename FReal, unsigned int FN>
inline std::istream & operator>>(std::istream & stream, Polynomial_LD<FReal,FN> & m_polynomial )
{m_polynomial.readFromStream(stream); m_polynomial.sort(); return stream;}
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

#if defined(WIN32) && !defined(SOFA_BUILD_HELPER)
#ifdef SOFA_DOUBLE
extern template class SOFA_HELPER_API Polynomial_LD<double,1>;
extern template class SOFA_HELPER_API Polynomial_LD<double,2>;
extern template class SOFA_HELPER_API Polynomial_LD<double,3>;
extern template class SOFA_HELPER_API Polynomial_LD<double,4>;
extern template class SOFA_HELPER_API Polynomial_LD<double,5>;
#endif

#ifdef SOFA_FLOAT
extern template class SOFA_HELPER_API Polynomial_LD<float,1>;
extern template class SOFA_HELPER_API Polynomial_LD<float,2>;
extern template class SOFA_HELPER_API Polynomial_LD<float,3>;
extern template class SOFA_HELPER_API Polynomial_LD<float,4>;
extern template class SOFA_HELPER_API Polynomial_LD<float,5>;
#endif
#endif



} // namespace helper

} // namespace sofa

#endif

