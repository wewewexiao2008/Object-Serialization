#ifndef __BINARY_SERIALIZATION__
#define __BINARY_SERIALIZATION__

#include <set>
#include <map>
#include <list>
#include <string>
#include <memory>
#include <vector>
#include <cstdio>
#include <fstream>
#include <cstring>
#include <iostream>
#include <exception>
#include <type_traits>
using namespace std;

/*
    binary serialization steps:
    Serial Function:
        1 Open File
        2 Output Data
        3 Close File

    For the step 2
    Output Data:
        1.try to find a particular template
        2.fit, then output the nested template
        3.no fit, use the trivially copyable to check
            3.1 trivially copyable: copy from the memory
            3.2 throw an error message 
*/
namespace binary_serialization{
    
    template <class T> void out_data(fstream & fout, T & object);                       //trivially copyable
    template <class T> void out_data(fstream & fout, vector <T> & object);              //std::vector
    template <> void out_data(fstream & fout, string & object);                         //std::string
    template <class T1, class T2> void out_data(fstream & fout, pair<T1,T2> & object);  //std::pair
    template <class T> void out_data(fstream & fout, set <T> & object);                 //std::set
    template <class T> void out_data(fstream & fout, list <T> & object);                //std::list
    template <class T1, class T2> void out_data(fstream & fout, map <T1,T2> & object);  //std::map
    template <class T> void out_data(fstream & fout, unique_ptr <T> & object);          //std::unique_ptr
    template <class T> void out_data(fstream & fout, weak_ptr <T> & object);            //std::weak_ptr
    template <class T> void out_data(fstream & fout, shared_ptr <T> & object);          //std::shared_ptr

    template <class T> void in_data(fstream & fin, T & object);                         //trivially copyable
    template <class T> void in_data(fstream & fin, vector <T> & object);                //std::vector
    template <> void in_data(fstream & fin, string & object);                           //std::string
    template <class T1, class T2> void in_data(fstream & fin, pair<T1,T2> & object);    //std::pair
    template <class T> void in_data(fstream & fin, set <T> & object);                   //std::set
    template <class T> void in_data(fstream & fin, list <T> & object);                  //std::list
    template <class T1, class T2> void in_data(fstream & fin, map <T1,T2> & object);    //std::map
    template <class T> void in_data(fstream & fin, unique_ptr <T> & object);            //std::unique_ptr
    template <class T> void in_data(fstream & fin, weak_ptr <T> & object);              //std::weak_ptr
    template <class T> void in_data(fstream & fin, shared_ptr <T> & object);            //std::shared_ptr

    void open_out_file(fstream & fout, string FileName);                                //open output file
    void open_in_file(fstream & fin, string FileName);                                  //open input file
    template <class T> void serialize(T & object, string FileName);                     //binary serialize
    template <class T> void deserialize(T & object, string FileName);                   //binary deserialize

    class serial_error : public exception{
        private:
            string message;
        public:
            serial_error();
            serial_error(string s):message(s){}
            string what(){return message;}
    };

    serial_error file_cant_open("Can't Open the File");
    serial_error no_template_in_data("No Particular Template for In Data");
    serial_error no_template_out_data("No Particular Template for Out Data");

    template <class T> void out_data(fstream & fout, T & object)            //trivially_copyable
    {
        if (!is_trivially_copyable<T>::value){
            throw no_template_out_data;
        } else {
            fout.write( (char *) &object, sizeof object);
        }
    }

    template <class T> void out_data(fstream & fout, vector <T> & object)   //std::vector
    {
        int count = object.size();
        out_data(fout, count);
        for (int i=0; i<object.size();++i){
            out_data(fout,object[i]);
        }
    }

    template <> void out_data(fstream & fout, string & object)              //std::string
    {
        int length = object.size();
        out_data(fout, length);
        for (int i=0; i<object.size(); ++i)
            out_data(fout,object[i]);
    }

    template <class T1, class T2> void out_data(fstream & fout, pair<T1,T2> & object)   //std::pair
    {
        out_data(fout, object.first);
        out_data(fout, object.second);
    }

    template <class T> void out_data(fstream & fout, set <T> & object)      //std::set
    {
        int count = object.size();
        out_data(fout, count);
        T tempT;
        for (auto i = object.begin(); i<object.end(); ++i){
            tempT = *i;
            out_data(fout, tempT);
        }
    }

    template <class T> void out_data(fstream & fout, list <T> & object)     //std::list
    {
        int count = object.size();
        out_data(fout, count);
        for (auto i = object.begin(); i!=object.end(); ++i){
            T tempT = *i;
            out_data(fout, tempT);
        }
    }

    template <class T1, class T2> void out_data(fstream & fout, map <T1,T2> & object)      //std::map
    {
        int count = object.size();
        out_data(fout, count);
        pair <T1,T2> tempT;
        for (auto i=object.begin(); i!=object.end(); ++i){
            tempT = *i;
            out_data(fout, tempT);
        }
    }

    template <class T> void out_data(fstream & fout, unique_ptr <T> & object)            //std::unique_ptr
    {
        out_data(fout, *object);
    }

    template <class T> void out_data(fstream & fout, weak_ptr <T> & object)            //std::weak_ptr
    {
        out_data(fout, *object);
    }

    template <class T> void out_data(fstream & fout, shared_ptr <T> & object)            //std::shared_ptr
    {
        out_data(fout, *object);
    }

    template <class T> void in_data(fstream & fin, T & object)              //trivially_copyable
    {
        if (!is_trivially_copyable<T>::value){
            throw no_template_in_data;
        } else {
            fin.read((char *) (&object), sizeof object);
        }
    }

    template <class T> void in_data(fstream & fin, vector <T> & object)     //std::vector
    {
        int count;
        T tempT;
        object.clear();
        in_data(fin, count);
        for (int i=0; i<count;++i){
            in_data(fin,tempT);
            object.push_back(tempT);
        }
    }

    template <> void in_data(fstream & fin, string & object)                //std::string
    {
        object.clear();
        int count;
        char ch;
        in_data(fin, count);
        for (int i=0; i<count; ++i){
            in_data(fin,ch);
            object=object+ch;
        }
    }
    
    template <class T1, class T2> void in_data(fstream & fin, pair<T1,T2> & object) //std::pair
    {
        in_data(fin, object.first);
        in_data(fin, object.second);
    }

    template <class T> void in_data(fstream & fin, set <T> & object)        //std::set
    {
        object.clear();
        int count = object.size();
        in_data(fin, count);
        T tempT;
        for (int i=0; i<count ;++i){
            in_data(fin, tempT);
            object.insert(tempT);
        }
    }

    template <class T> void in_data(fstream & fin, list <T> & object)       //std::list
    {
        int count;
        in_data(fin, count);
        object.clear();
        T tempT;
        for (int i=0; i<count; ++i){
            in_data(fin, tempT);
            object.push_back(tempT);
        }
    }

    template <class T1, class T2> void in_data(fstream & fin, map <T1,T2> & object)         //std::map
    {
        object.clear();
        int count;
        in_data(fin, count);
        pair <T1,T2> tempT;
        for (int i=0; i<count; ++i){
            in_data(fin, tempT);
            object[tempT.first] = tempT.second;
        }
    }

    template <class T> void in_data(fstream & fin, unique_ptr <T> & object)                  //std::unique_ptr
    {
        T tempT;
        in_data(fin, tempT);
        object = unique_ptr<T>(new T (tempT));
    }

    template <class T> void in_data(fstream & fin, weak_ptr <T> & object)                  //std::weak_ptr
    {
        T tempT;
        in_data(fin, tempT);
        object = weak_ptr<T>(new T (tempT));
    }

    template <class T> void in_data(fstream & fin, shared_ptr <T> & object)                  //std::shared_ptr
    {
        T tempT;
        in_data(fin, tempT);
        object = shared_ptr<T>(new T (tempT));
    }

    void open_out_file(fstream & fout, string FileName)
    {
        fout.open(FileName.c_str(),ios::binary|ios::out);
        if (!fout.is_open()) throw file_cant_open;
    }

    void open_in_file(fstream & fin, string FileName)
    {
        fin.open(FileName.c_str(), ios::binary|ios::in);
        if (!fin.is_open()) throw file_cant_open;
    }

    template <class T> void serialize(T & object, string FileName)
    {
        fstream fout;
        open_out_file(fout, FileName);
        out_data(fout, object);
        fout.close();
    }

    template <class T> void deserialize(T & object, string FileName)
    {
        fstream fin;
        open_in_file(fin, FileName);
        in_data(fin, object);
        fin.close();
    }

};

#endif