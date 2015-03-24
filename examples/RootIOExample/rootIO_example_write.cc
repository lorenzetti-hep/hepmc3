// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file rootIO_example_write.cc
 *  @brief Basic example of use of root I/O: writing events to file
 *
 *  @author Witold Pokorski
 *  @date   16/10/14
 */
#include "HepMC/GenEvent.h"
#include "HepMC/ReaderAscii.h"
#include "HepMC/WriterRoot.h"
#include "HepMC/Print.h"

#include <iostream>

using namespace HepMC;
using std::cout;
using std::endl;

/** Main */
int main(int argc, char **argv) {

    if( argc<3 ) {
        cout << "Usage: " << argv[0] << " <input_hepmc3_file> <output_root_file>" << endl;
        exit(-1);
    }

    ReaderAscii text_input (argv[1]);
    WriterRoot  root_output(argv[2]);

    int events_parsed = 0;

    while( !text_input.rdstate() ) {

        GenEvent evt(Units::GEV,Units::MM);

        text_input.read_event(evt);

        if( text_input.rdstate() ) break;

        if( events_parsed == 0 ) {
            cout << "First event: " << endl;
            Print::listing(evt);
        }

        root_output.write_event(evt);
        ++events_parsed;

        if( events_parsed%1000 == 0 ) {
            cout << "Event: " << events_parsed << endl;
        }
    }

    text_input.close();
    root_output.close();

    std::cout << "Events parsed and written: " << events_parsed << std::endl;

    return 0;
}
