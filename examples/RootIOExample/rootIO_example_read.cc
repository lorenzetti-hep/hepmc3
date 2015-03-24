// -*- C++ -*-
//
// This file is part of HepMC
// Copyright (C) 2014 The HepMC collaboration (see AUTHORS for details)
//
/**
 *  @file rootIO_example_read.cc
 *  @brief Basic example of use of root I/O: reading events from file
 *
 *  @author Witold Pokorski
 *  @date   16/10/14
 */
#include "HepMC/GenEvent.h"
#include "HepMC/WriterAscii.h"
#include "HepMC/ReaderRoot.h"
#include "HepMC/Print.h"

#include <iostream>

using namespace HepMC;
using std::cout;
using std::endl;

/** Main */
int main(int argc, char **argv) {

    if( argc<3 ) {
        cout << "Usage: " << argv[0] << " <input_root_file> <output_hepmc3_file>" << endl;
        exit(-1);
    }

    ReaderRoot  root_input (argv[1]);
    WriterAscii text_output(argv[2]);

    int events_parsed = 0;

    while( !root_input.rdstate() ) {

        GenEvent evt;

        root_input.read_event(evt);

        if( root_input.rdstate() ) break;

        if( events_parsed == 0 ) {
            cout << "First event: " << endl;
            Print::listing(evt);
        }

        text_output.write_event(evt);
        ++events_parsed;

        if( events_parsed%1000 == 0 ) {
            cout << "Event: " << events_parsed << endl;
        }
    }

    root_input.close();
    text_output.close();

    std::cout << "Events parsed and written: " << events_parsed << std::endl;

    return 0;
}
