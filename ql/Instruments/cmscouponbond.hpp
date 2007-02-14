/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Chiara Fornarola

 This file is part of QuantLib, a free-software/open-source library
 for financial quantitative analysts and developers - http://quantlib.org/

 QuantLib is free software: you can redistribute it and/or modify it
 under the terms of the QuantLib license.  You should have received a
 copy of the license along with this program; if not, please email
 <quantlib-dev@lists.sf.net>. The license is also available online at
 <http://quantlib.org/reference/license.html>.

 This program is distributed in the hope that it will be useful, but WITHOUT
 ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 FOR A PARTICULAR PURPOSE.  See the license for more details.
*/

/*! \file cmscouponbond.hpp
    \brief cms-coupon bond
*/

#ifndef quantlib_cms_coupon_bond_hpp
#define quantlib_cms_coupon_bond_hpp

#include <ql/Instruments/bond.hpp>
#include <ql/CashFlows/iborcoupon.hpp>
#include <ql/CashFlows/cmscoupon.hpp>
#include <ql/Indexes/swapindex.hpp>
#include <ql/swaptionvolstructure.hpp>

namespace QuantLib {

    //! cms-coupon bond
    /*! \ingroup instruments

        \test calculations are tested by checking results against
              cached values.
    */
    class CmsCouponBond : public Bond {
      public:
           CmsCouponBond(Real faceAmount,
                         const Date& issueDate,
                         const Date& datedDate,
                         const Date& maturityDate,
                         Integer settlementDays,
                         const boost::shared_ptr<SwapIndex>& index,
                         Integer fixingDays,
                         const std::vector<Real>& gearings,
                         const std::vector<Spread>& spreads,
                         Frequency couponFrequency,
                         const Calendar& calendar,
                         const DayCounter& dayCounter,
                         const boost::shared_ptr<CmsCouponPricer>& pricer,
                         const std::vector<Rate>& caps,
                         const std::vector<Rate>& floors,
                         BusinessDayConvention accrualConvention = Following,
                         BusinessDayConvention paymentConvention = Following,
                         Real redemption = 100.0,
                         const Handle<YieldTermStructure>& discountCurve
                                              = Handle<YieldTermStructure>(),
                         const Date& stub = Date(),
                         bool fromEnd = true);
    };

}

#endif