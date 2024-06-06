#pragma once

#include "TFile.h"

#include "file_helper.h"

// Created : 5.6.24, Henry Wallace, hwallace@rhul.ac.uk

/**
 * @file root_utils.h
 * @brief contains ROOT helper functions
 *
 * Using ROOT requires a LOT of boiler plate
 * this aims to move all of this somewhere nice (with the added benefit that hopefully
 * we can swap it out for any other format and only need to change these)
 */

namespace file_helper{
class ROOTHelper : public FileHelper{
public:

    // Just wraps around base-class implementation
    ROOTHelper(std::string input_file_name, std::string opts="open")
        : FileHelper(input_file_name, opts) {} /**Base class implementation */

    // IO operations
    void open_file(std::string opts) override{
    /**
     * @brief Opens file specified in the constructor
     * @param opt I/O options (see https://root.cern.ch/doc/master/classTFile.html)
     */

        // No need to if the file is already open!
        if(!_input_file->IsZombie()){
            return;
        }

        _input_file = std::unique_ptr<TFile>{TFile::Open(get_file_name().c_str(), opts.c_str())};

        if(_input_file->IsZombie()){
            _file_exception();
        }
    }

    void close_file() override{
        /**
         * @brief Closes ROOT file
         */

        // It's not there! ... be careful with connected objects!
        if(_input_file->IsZombie()){
            return;
        }
        _input_file->Close();
    }

    template <typename T>
    std::unique_ptr<T> get_by_name(std::string obj_name){
        /**
         * @brief Gets object stored in file with base type T
         * @param obj_name : Name of object in ROOT file
         */
        // NOTE: Detech from file will NOT delete your object when the TFile closes!
        std::unique_ptr<T> obj (dynamic_cast<T*>(_input_file->Get(obj_name.c_str())));

        if(obj==nullptr){
            _object_exception(obj_name);
        }
        // Hopefully this will just work!
        return obj;
    }

    template <typename T>
    void write_to_file(T object_to_write){
        /**
         * Writes object to ROOT file
         * @param object_to_write : Object to write (NOTE this expected to be a pointer!)
         */
        if(_input_file->IsZombie()){
            _file_exception();
        }

        _input_file->cd();
        object_to_write->Write();
    }



protected:
    std::unique_ptr<TFile> _input_file;
};
} // file_helper namespace
