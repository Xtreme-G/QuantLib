/* -*- mode: c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*
 Copyright (C) 2006 Ferdinando Ametrano
 Copyright (C) 2006 Mark Joshi

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



#include <ql/MarketModels/curvestate.hpp>

namespace QuantLib {

    void forwardsFromDiscountRatios(Size firstValidIndex,
                                    const std::vector<DiscountFactor>& ds,
                                    const std::vector<Time>& taus,
                                    std::vector<Rate>& fwds) {
        QL_REQUIRE(taus.size()==fwds.size(),
                   "taus.size()!=fwds.size()");
        QL_REQUIRE(ds.size()==fwds.size()+1,
                   "ds.size()!=fwds.size()+1");

        for (Size i=firstValidIndex; i<fwds.size(); ++i)
            fwds[i] = (ds[i]-ds[i+1])/(ds[i]*taus[i]);
    };


    void coterminalFromDiscountRatios(Size firstValidIndex,
                                      const std::vector<DiscountFactor>& ds,
                                      const std::vector<Time>& taus,
                                      std::vector<Rate>& cotSwapRates,
                                      std::vector<Real>& cotSwapAnnuities) {
        Size nCotSwapRates = cotSwapRates.size();
        QL_REQUIRE(taus.size()==nCotSwapRates,
                   "taus.size()!=cotSwapRates.size()");
        QL_REQUIRE(cotSwapAnnuities.size()==nCotSwapRates,
                   "cotSwapAnnuities.size()!=cotSwapRates.size()");
        QL_REQUIRE(ds.size()==nCotSwapRates+1,
                   "ds.size()!=cotSwapRates.size()+1");
        
        cotSwapAnnuities[nCotSwapRates-1] = taus[nCotSwapRates-1]*ds[nCotSwapRates];
        cotSwapRates[nCotSwapRates-1] = (ds[nCotSwapRates-1]-ds[nCotSwapRates])/cotSwapAnnuities[nCotSwapRates-1];
        
        for (Size i=nCotSwapRates-1; i>firstValidIndex; --i) {
            cotSwapAnnuities[i-1] = cotSwapAnnuities[i] + taus[i-1] * ds[i];
            cotSwapRates[i-1] = (ds[i-1]-ds[nCotSwapRates])/cotSwapAnnuities[i-1];
        }
    };


    void constantMaturityFromDiscountRatios(// Size i, // to be added later
                                            Size spanningForwards,
                                            Size firstValidIndex,
                                            const std::vector<DiscountFactor>& ds,
                                            const std::vector<Time>& taus,
                                            std::vector<Rate>& constMatSwapRates,
                                            std::vector<Real>& constMatSwapAnnuities) {
        Size nConstMatSwapRates = constMatSwapRates.size();
        QL_REQUIRE(taus.size()==nConstMatSwapRates,
                   "taus.size()!=nConstMatSwapRates");
        QL_REQUIRE(constMatSwapAnnuities.size()==nConstMatSwapRates,
                   "constMatSwapAnnuities.size()!=nConstMatSwapRates");
        QL_REQUIRE(ds.size()==nConstMatSwapRates+1,
                   "ds.size()!=nConstMatSwapRates+1");
        // compute the first cmsrate and cmsannuity     
        constMatSwapAnnuities[firstValidIndex]=0.;
        Size lastIndex = std::min(firstValidIndex+spanningForwards,nConstMatSwapRates);
        for (Size i=firstValidIndex; i<lastIndex; ++i) {
            constMatSwapAnnuities[firstValidIndex]+= taus[i] * ds[i+1];
        }  
        constMatSwapRates[firstValidIndex] = 
            (ds[firstValidIndex]-ds[lastIndex])/
                constMatSwapAnnuities[firstValidIndex];

        // compute all the other cmas rates and cms annuities
        for (Size i=firstValidIndex+1; i<nConstMatSwapRates; ++i) {
            Size lastIndex = std::min(i+spanningForwards,nConstMatSwapRates);
            constMatSwapAnnuities[i] = constMatSwapAnnuities[i-1] 
                                       - taus[i-1] * ds[i];
            if(lastIndex<nConstMatSwapRates)
               constMatSwapAnnuities[i] += taus[lastIndex-1] * ds[lastIndex];
            constMatSwapRates[i] = 
                (ds[i]-ds[lastIndex])/
                    constMatSwapAnnuities[i];
        }
    };

}