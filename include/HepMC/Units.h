// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014-2015 The HepMC collaboration (see AUTHORS for details)
//
#ifndef HEPMC_UNITS_H
#define HEPMC_UNITS_H
/**
 *  @file Units.h
 *  @brief Definition of \b class Units
 *
 *  @class HepMC::Units
 *  @brief Stores units-related enums and conversion functions
 *
 *  Manages units used by HepMC::GenEvent
 *
 */

#include "HepMC/Errors.h"
#include "HepMC/FourVector.h"

namespace HepMC {


class Units {
public:
    /** @brief Momentum units */
    enum MomentumUnit { MEV, GEV };

    /** @brief Position units */
    enum LengthUnit   { MM,  CM  };

public:
    /** @brief Get momentum unit based on its name*/
  static MomentumUnit momentum_unit( const std::string& name ) {
        if( name.compare(0,3,"GEV") == 0 ) return GEV;
        if( name.compare(0,3,"MEV") == 0 ) return MEV;

        ERROR("Units::momentum_unit: unrecognised unit name: '" << name <<"', setting to GEV" )

        return GEV;
    }

    /** @brief Get length unit based on its name*/
    static LengthUnit length_unit( const std::string& name ) {
        if( name.compare(0,2,"CM") == 0 ) return CM;
        if( name.compare(0,2,"MM") == 0 ) return MM;

        ERROR("Units::length_unit: unrecognised unit name: '" << name <<"', setting to CM" )

        return CM;
    }

    /** @brief Get name of momentum unit */
    static std::string name( MomentumUnit u ) {
        switch(u) {
            case MEV: return "MEV";
            case GEV: return "GEV";
        }

        return "<UNDEFINED>";
    }

    /** @brief Get name of length unit */
    static std::string name( LengthUnit u ) {
        switch(u) {
            case MM: return "MM";
            case CM: return "CM";
        }

        return "<UNDEFINED>";
    }

    /** @brief Convert FourVector to different momentum unit */
    static void convert( FourVector &m, MomentumUnit from, MomentumUnit to ) {
        if( from == to ) return;

        if( from == GEV ) {
            // GEV -> MEV
            m *= 1000.;
        }
        else if( from == MEV ) {
            // MEV -> GEV
            m *= 0.001;
        }
    }

    /** @brief Convert FourVector to different length unit */
    static void convert( FourVector &m, LengthUnit from, LengthUnit to ) {
        if( from == to ) return;

        if( from == CM ) {
            // CM -> MM
            m *= 100.;
        }
        else if( from == MM ) {
            // MM -> CM
            m *= 0.01;
        }
    }

};

} // namespace HepMC

#endif
