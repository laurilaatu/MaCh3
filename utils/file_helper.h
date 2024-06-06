#pragma once

// Created : 5.6.24, Henry Wallace, hwallace@rhul.ac.uk


#include "manager/MaCh3Logger.h"


#include <string>
#include <iostream>
#include <memory>

/**
 * @file  file_helper.h
 * @brief contains pure virtual helper functions
 *
 * Using files requires a LOT of boiler plate
 * this aims to move all of this somewhere nice (with the added benefit that hopefully
 * we can swap it out for any other format and only need to change these)
 */

namespace file_helper{

class FileHelper{
    public:
        FileHelper(std::string input_file_name, std::string opts=""){
            /**
             * Basic Constructor
             */
            _input_file_name = input_file_name;
            open_file(opts);
        }

        virtual ~FileHelper(){
            /**
             * Basic Destructor
             */
            close_file();
        }

        // Basic I/O
        virtual void open_file(std::string){}; /** virtual, opens file*/
        virtual void close_file(){}; /** virtual, closes file*/

        // Getters
        std::unique_ptr<void> get_by_name(std::string); /**virtual, gets object by name*/
        virtual void write_to_file(){} /** virtual, writes to file*/
        std::string get_file_name(){return _input_file_name;} /** Gets file name */

protected:
        std::string _input_file_name;

        void _file_exception(){
            MACH3LOG_ERROR("Cannot open file {}", _input_file_name);
            throw;
        }

        void _object_exception(std::string object_name){
            MACH3LOG_ERROR("Cannot find {} in {}", object_name, _input_file_name);
            throw;
        }
};

} // file_helper namespace
